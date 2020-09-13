/*
* Vulkan Model loader using ASSIMP
*
* Copyright(C) 2016-2017 by Sascha Willems - www.saschawillems.de
*
* This code is licensed under the MIT license(MIT) (http://opensource.org/licenses/MIT)
*/

#pragma once

#include <stdlib.h>
#include <string>
#include <fstream>
#include <vector>

#include "vulkan/vulkan.h"

#include "assimp/Importer.hpp" 
#include "assimp/scene.h"     
#include "assimp/postprocess.h"
#include "assimp/cimport.h"
#include "../Platform.h"
#include "../Config.h"
#include "../Graphics/Renderer.h"
#include "../Math/Vector.h"
namespace Dae
{
	/** @brief Vertex layout components */
	typedef enum Component {
		VERTEX_COMPONENT_POSITION = 0x0,
		VERTEX_COMPONENT_NORMAL = 0x1,
		VERTEX_COMPONENT_COLOR = 0x2,
		VERTEX_COMPONENT_UV = 0x3,
		VERTEX_COMPONENT_TANGENT = 0x4,
		VERTEX_COMPONENT_BITANGENT = 0x5,
		VERTEX_COMPONENT_DUMMY_FLOAT = 0x6,
		VERTEX_COMPONENT_DUMMY_VEC4 = 0x7
	} Component;

	/** @brief Stores vertex layout components for model loading and Vulkan vertex input and atribute bindings  */
	struct VertexLayout {
	public:
		/** @brief Components used to generate vertices from */
		std::vector<Component> components;

		VertexLayout(std::vector<Component> components)
		{
			this->components = std::move(components);
		}

		uint32_t stride()
		{
			uint32_t res = 0;
			for (auto& component : components)
			{
				switch (component)
				{
				case VERTEX_COMPONENT_UV:
					res += 2 * sizeof(float);
					break;
				case VERTEX_COMPONENT_DUMMY_FLOAT:
					res += sizeof(float);
					break;
				case VERTEX_COMPONENT_DUMMY_VEC4:
					res += 4 * sizeof(float);
					break;
				default:
					// All components except the ones listed above are made up of 3 floats
					res += 3 * sizeof(float);
				}
			}
			return res;
		}
	};

	/** @brief Used to parametrize model loading */
	struct ModelCreateInfo {
		vec3 center;
		vec3 scale;
		vec2 uvscale;
		VkMemoryPropertyFlags memoryPropertyFlags = 0;

		ModelCreateInfo() : center(vec3(0, 0, 0)), scale(vec3(1, 1, 1)), uvscale(vec2(1, 1)) {};

		ModelCreateInfo(vec3 scale, vec2 uvscale, vec3 center)
		{
			this->center = center;
			this->scale = scale;
			this->uvscale = uvscale;
		}

		ModelCreateInfo(float scale, float uvscale, float center)
		{
			this->center = vec3(center, center, center);
			this->scale = vec3(scale, scale, scale);
			this->uvscale = vec2(uvscale, uvscale);
		}

	};

	struct Model {
		Device device = nullptr;
		Buffer vertices;
		Buffer indices;
		VertexSetup m_Setup;
		uint32_t indexCount = 0;
		uint32_t vertexCount = 0;

		/** @brief Stores vertex and index base and counts for each part of a model */
		struct ModelPart {
			uint32_t vertexBase;
			uint32_t vertexCount;
			uint32_t indexBase;
			uint32_t indexCount;
		};
		std::vector<ModelPart> parts;
		VertexSetup GetVertexSetup() const{ return m_Setup; };
		static const int defaultFlags = aiProcess_FlipWindingOrder | aiProcess_Triangulate | aiProcess_PreTransformVertices | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals;

		struct Dimension
		{
			vec3 min = vec3(FLT_MAX, FLT_MAX, FLT_MAX);
			vec3 max = vec3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
			vec3 size;
		} dim;
		vec3 model_center;

		/** @brief Release all Vulkan resources of this model */
		void destroy()
		{
			assert(device);
			DestroyBuffer(device, vertices);
			if (indices != nullptr)
			{
				DestroyBuffer(device, indices);
			}
			DestroyVertexSetup(device, m_Setup);
			
		}

		/**
		* Loads a 3D model from a file into Vulkan buffers
		*
		* @param device Pointer to the Vulkan device used to generated the vertex and index buffers on
		* @param filename File to load (must be a model format supported by ASSIMP)
		* @param layout Vertex layout components (position, normals, tangents, etc.)
		* @param createInfo MeshCreateInfo structure for load time settings like scale, center, etc.
		* @param copyQueue Queue used for the memory staging copy commands (must support transfer)
		*/
		bool loadFromFile(const std::string& filename, VertexLayout layout, ModelCreateInfo* createInfo, Device device)
		{
			this->device = device;

			Assimp::Importer Importer;
			const aiScene* pScene;

			// Load file

			pScene = Importer.ReadFile(filename.c_str(), defaultFlags);
			if (!pScene) {
				std::string error = Importer.GetErrorString();
				//			tools::exitFatal(error + "\n\nThe file may be part of the additional asset pack.\n\nRun \"download_assets.py\" in the repository root to download the latest version.", -1);
			}


			if (pScene)
			{
				parts.clear();
				parts.resize(pScene->mNumMeshes);

				vec3 scale(1.0f, 1.0f, 1.0f);
				vec2 uvscale(1.0f, 1.0f);
				vec3 center(0.0f, 0.0f, 0.0f);
				if (createInfo)
				{
					scale = createInfo->scale;
					uvscale = createInfo->uvscale;
					center = createInfo->center;
				}

				std::vector<float> vertexBuffer;
				std::vector<uint32_t> indexBuffer;

				vertexCount = 0;
				indexCount = 0;

				// Load meshes
				for (unsigned int i = 0; i < pScene->mNumMeshes; i++)
				{
					const aiMesh* paiMesh = pScene->mMeshes[i];

					parts[i] = {};
					parts[i].vertexBase = vertexCount;
					parts[i].indexBase = indexCount;

					vertexCount += pScene->mMeshes[i]->mNumVertices;

					aiColor3D pColor(0.f, 0.f, 0.f);
					pScene->mMaterials[paiMesh->mMaterialIndex]->Get(AI_MATKEY_COLOR_DIFFUSE, pColor);

					const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

					for (unsigned int j = 0; j < paiMesh->mNumVertices; j++)
					{
						const aiVector3D* pPos = &(paiMesh->mVertices[j]);
						const aiVector3D* pNormal = &(paiMesh->mNormals[j]);
						const aiVector3D* pTexCoord = (paiMesh->HasTextureCoords(0)) ? &(paiMesh->mTextureCoords[0][j]) : &Zero3D;
						const aiVector3D* pTangent = (paiMesh->HasTangentsAndBitangents()) ? &(paiMesh->mTangents[j]) : &Zero3D;
						const aiVector3D* pBiTangent = (paiMesh->HasTangentsAndBitangents()) ? &(paiMesh->mBitangents[j]) : &Zero3D;

						for (auto& component : layout.components)
						{
							switch (component) {
							case VERTEX_COMPONENT_POSITION:
								vertexBuffer.push_back(pPos->x * scale.x + center.x);
								vertexBuffer.push_back(-pPos->y * scale.y + center.y);
								vertexBuffer.push_back(pPos->z * scale.z + center.z);
								break;
							case VERTEX_COMPONENT_NORMAL:
								vertexBuffer.push_back(pNormal->x);
								vertexBuffer.push_back(-pNormal->y);
								vertexBuffer.push_back(pNormal->z);
								break;
							case VERTEX_COMPONENT_UV:
								vertexBuffer.push_back(pTexCoord->x * uvscale.x);
								vertexBuffer.push_back(pTexCoord->y * uvscale.y);
								break;
							case VERTEX_COMPONENT_COLOR:
								vertexBuffer.push_back(pColor.r);
								vertexBuffer.push_back(pColor.g);
								vertexBuffer.push_back(pColor.b);
								break;
							case VERTEX_COMPONENT_TANGENT:
								vertexBuffer.push_back(pTangent->x);
								vertexBuffer.push_back(pTangent->y);
								vertexBuffer.push_back(pTangent->z);
								break;
							case VERTEX_COMPONENT_BITANGENT:
								vertexBuffer.push_back(pBiTangent->x);
								vertexBuffer.push_back(pBiTangent->y);
								vertexBuffer.push_back(pBiTangent->z);
								break;
								// Dummy components for padding
							case VERTEX_COMPONENT_DUMMY_FLOAT:
								vertexBuffer.push_back(0.0f);
								break;
							case VERTEX_COMPONENT_DUMMY_VEC4:
								vertexBuffer.push_back(0.0f);
								vertexBuffer.push_back(0.0f);
								vertexBuffer.push_back(0.0f);
								vertexBuffer.push_back(0.0f);
								break;
							};
						}

						dim.max.x = fmax(pPos->x, dim.max.x);
						dim.max.y = fmax(pPos->y, dim.max.y);
						dim.max.z = fmax(pPos->z, dim.max.z);

						dim.min.x = fmin(pPos->x, dim.min.x);
						dim.min.y = fmin(pPos->y, dim.min.y);
						dim.min.z = fmin(pPos->z, dim.min.z);
					}

					dim.size = dim.max - dim.min;
					model_center = (dim.max + dim.min) / 2;
					parts[i].vertexCount = paiMesh->mNumVertices;

					uint32_t indexBase = static_cast<uint32_t>(indexBuffer.size());
					for (unsigned int j = 0; j < paiMesh->mNumFaces; j++)
					{
						const aiFace& Face = paiMesh->mFaces[j];
						if (Face.mNumIndices != 3)
							continue;
						indexBuffer.push_back(indexBase + Face.mIndices[0]);
						indexBuffer.push_back(indexBase + Face.mIndices[1]);
						indexBuffer.push_back(indexBase + Face.mIndices[2]);
						parts[i].indexCount += 3;
						indexCount += 3;
					}
				}


				uint32_t vBufferSize = static_cast<uint32_t>(vertexBuffer.size()) * sizeof(float);
				uint32_t iBufferSize = static_cast<uint32_t>(indexBuffer.size()) * sizeof(uint32_t);

				SBufferParams vbParams = { vBufferSize, HeapType::HEAP_DEFAULT, Usage::VERTEX_BUFFER, "" };
				SBufferParams ibParams = { iBufferSize, HeapType::HEAP_DEFAULT, Usage::INDEX_BUFFER, "" };

				vertices = CreateBuffer(device, vbParams);
				indices = CreateBuffer(device, ibParams);

				SMapBufferParams vbMapParams = { GetMainContext(device), vertices, 0, vBufferSize };
				SMapBufferParams ibMapParams = { GetMainContext(device), indices, 0, iBufferSize };

				float* vbData = (float*)MapBuffer(vbMapParams);
				uint32_t* ibData = (uint32_t*)MapBuffer(ibMapParams);
				memcpy(vbData, vertexBuffer.data(), vBufferSize);
				memcpy(ibData, indexBuffer.data(), iBufferSize);
				UnmapBuffer(vbMapParams);
				UnmapBuffer(ibMapParams);

				m_Setup = CreateVertexSetup(device, vertices, layout.stride(), indices, sizeof(uint32_t));
				return true;
			}
			else
			{
				printf("Error parsing '%s': '%s'\n", filename.c_str(), Importer.GetErrorString());
#if defined(__ANDROID__)
				LOGE("Error parsing '%s': '%s'", filename.c_str(), Importer.GetErrorString());
#endif
				return false;
			}
		};

		/**
		* Loads a 3D model from a file into Vulkan buffers
		*
		* @param device Pointer to the Vulkan device used to generated the vertex and index buffers on
		* @param filename File to load (must be a model format supported by ASSIMP)
		* @param layout Vertex layout components (position, normals, tangents, etc.)
		* @param scale Load time scene scale
		* @param copyQueue Queue used for the memory staging copy commands (must support transfer)
		*/
		bool loadFromFile(const std::string& filename, VertexLayout layout, float scale, Device device)
		{
			ModelCreateInfo modelCreateInfo(scale, 1.0f, 0.0f);
			return loadFromFile(filename, layout, &modelCreateInfo, device);
		}
	};
}