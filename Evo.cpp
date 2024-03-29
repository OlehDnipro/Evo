/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\ 
*Copyright 2021 Oleh Sopilniak																																	*
*																																								*
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software																	*
* and associated documentation files (the "Software"), to deal in the Software without restriction,																*
* including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,															*
* and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,															*
*  subject to the following conditions:																															*
*																																								*
* The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.								*	
*																																								*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,			*
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, *
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.    *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  */

#include "Humus/App/DemoApp.h"

#include "Humus/Windows/Resource.h"

#include "Techniques/ShadowMapCascade.h"
#include "Techniques/PBR.h"
#include "Techniques/SH.h"
#include "Techniques/IntegrateEnv.h"

#include "ParameterProviderRegistry.h"
/*
#include "../CPU.h"
#include <direct.h>

#include <mmsystem.h>
#pragma comment (lib, "winmm.lib")
*/

#ifdef _DEBUG
#include <crtdbg.h>
#endif
#include "Techniques/QuadTasks.h"

#include <stdio.h>
class CTreeFieldCollection : public IObjectCollection
{
public:
	void Create(Device device)
	{
		m_vertexLayout.Init({ VERTEX_COMPONENT_POSITION,
							VERTEX_COMPONENT_UV,
							VERTEX_COMPONENT_COLOR,
							VERTEX_COMPONENT_NORMAL });
		float4x4 mtx; mtx.identity();
		m_Objects.resize(3);
		m_Objects[0].Init(device, m_vertexLayout, "../../models/terrain_simple.dae", "../../Textures/gridlines.dds", 1.0f); m_Objects[0].SetMark(1);
		m_Objects[1].Init(device, m_vertexLayout, "../../models/oak_trunk.dae", "../../Textures/oak_bark.dds", 2.0f); m_Objects[1].SetMark(2);
		m_Objects[2].Init(device, m_vertexLayout, "../../models/oak_leafs.dae", "../../Textures/oak_leafs.dds", 2.0f, true); m_Objects[2].SetMark(2);
		m_ObjectInstances.resize(9);
		m_ObjectInstances[0] = new SimpleObjectInstance(m_Objects[0], device, mtx);

		const std::vector<vec3> positions = {
//		vec3(0.0f, 0.0f, 0.0f),
		vec3(3.75,  -0.2f, 4),
		vec3(5.65, -0.3f, 1.75f),
		vec3(3.75,  -0.12f, -0.75f),
		vec3(2, -0.3f, 1.75f),
		};
		/*
		{ { {2, -0.25, 4}, { 0, 0 } },
		{ { 5.5, -0.25,  4},   {1, 0} },
		{ { 5.5, -0.25, -0.5},   {1, 1} },
		{ {2, -0.25, -0.5},   {0, 1} }*/
		for (int i = 1; i < 9; i++)
		{
			mtx = translate(positions[(i - 1) / 2]);
			if (i % 2 == 0)
			{
				m_ObjectInstances[i] = new SimpleObjectInstance(m_Objects[1], device, mtx);
			}
			else
			{
				m_ObjectInstances[i] = new SimpleObjectInstance(m_Objects[2], device, mtx);
			}
		}
	}
	void DefineVertexFormat(vector<AttribDesc>& format)
	{
		for (int i = 0; i < m_vertexLayout.components.size(); i++)
		{
			switch (m_vertexLayout.components[i])
			{
			case VERTEX_COMPONENT_POSITION:
				format.push_back({ 0, VF_FLOAT3, "Position" });
				break;
			case VERTEX_COMPONENT_NORMAL:
				format.push_back({ 0, VF_FLOAT3, "Normal" });
				break;
			case VERTEX_COMPONENT_COLOR:
				format.push_back({ 0, VF_FLOAT3, "Color" });
				break;
			case VERTEX_COMPONENT_UV:
				format.push_back({ 0, VF_FLOAT2, "TexCoord" });
				break;
			}
		};
	}
	PrimitiveType GetPrimType() { return PRIM_TRIANGLES; };

};
#define WATERTEX_SPEED 0.125
#define WATERDROP_SPEED 0.025

#define WAVE_OFFSET 20

class EvoApp : public DemoApp
{
	Texture m_ColorBuffer, m_ShadowMap;
	Texture m_DepthBuffer, m_CubeDepth;

	ShadowMapCascade m_Shadows;
	CTreeFieldCollection m_TreeField;
	Texture m_RingDrop[2];
	Texture m_CubeMapLayered, m_CubeMapSingle, m_SHCubeMap, m_SpecularEnvMap, m_BRDFLookup;
	Texture m_Reflection;
	Texture m_NormalTile;
	CTexturedQuadGeometry m_Quad;
	CTexturedQuadGeometry m_WaterQuad;

	CPolygonGeometry m_Poly;
	CPolygonTask m_PolyTask;
	CWaterDropTask m_DropTask;
	CWaterTask m_WaterTask;
	CComputeSHTask m_ComputeSHTask;
	CIntegrateEnvTask m_IntegrateEnvTask;
	CPBRTask m_PBR;
	CSphereGeometry m_Spheres;
	uint m_CurDropTex = 1;
	float m_dropTime = 0, m_dropUpdateTime = 0;
public:
	EvoApp() :DemoApp(), m_Poly(8)
	{
		m_InitialUploadBufferSize = m_UploadBufferSize = 1024 * 1024 * 8;
	}
	bool Init()
	{
		m_TreeField.Create(m_Device);
		m_Poly.Create(m_Device);
		m_Quad.Create(m_Device);
		m_WaterQuad.Create(m_Device, { { {2, -0.25,  4},   {0, 0} },
										{ { 5.5, -0.25,  4},   {1, 0} },
										{ {2, -0.25, -0.5},   {0, 1} },
										{ { 5.5, -0.25, -0.5},   {1, 1} }
										
			});
		m_Spheres.Create(m_Device);
		m_PolyTask.SetGeometry(&m_Poly);
		m_DropTask.SetGeometry(&m_Quad);
		m_PolyTask.CreateResources(m_Device);
		m_DropTask.CreateResources(m_Device);
		m_Shadows.SetGeometry(&m_TreeField);
		m_Shadows.CreateResources(m_Device);
		m_WaterTask.CreateResources(m_Device);
		m_WaterTask.SetGeometry(&m_WaterQuad);
		m_PBR.CreateResources(m_Device);
		m_PBR.SetGeometry(&m_Spheres);
		CreateRenderSetups();

		return true;
	}

	void RenderEnvMap(Context context, Texture cubeMap,vec3 eye, int mip)
	{
		for (int i = 0; i < 6; i++)
		{
			SetRenderTarget(context, { cubeMap, {mip, -1, i} }, 0);
			SetDepthTarget(context, m_CubeDepth);
			//if (i == 5)
			//	continue;
			vec3 target;
			vec3 up;
			switch (i)
			{
			case 0:
				target = eye + vec3(1, 0, 0);//x+
				up = vec3(0, 1, 0);
				break;
			case 1:
				target = eye + vec3(-1, 0, 0);//x
				up = vec3(0, 1, 0);
				break;
			case 2:
				target = eye + vec3(0, 1, 0);//y+
				up = vec3(0, 0, -1);
				break;
			case 3:
				target = eye + vec3(0, -1, 0);//y-
				up = vec3(0, 0, 1);
				break;
			case 4:
				target = eye + vec3(0, 0, 1);//z+
				up = vec3(0, 1, 0);
				break;
			case 5:
				target = eye + vec3(0, 0, -1);//z-
				up = vec3(0, 1, 0);
				break;
			}
			m_Shadows.SetCameraLookAt(eye, target, up);
			BeginRenderPass(context, "CubeRender");
			m_Shadows.SetPassParameters(context, ShadowMapCascade::PassEnum::NoShadow);
			m_Shadows.Draw(context);
			EndRenderPass(context);
		}
	}

	void CreateRenderSetups()
	{
		STextureParams db_params;
		db_params.m_Width = m_DeviceParams.m_Width;
		db_params.m_Height = m_DeviceParams.m_Height;
		db_params.m_Format = IMGFMT_D16;
		db_params.m_MSAASampleCount = 1;
		db_params.m_DepthTarget = true;
		m_DepthBuffer = CreateTexture(m_Device, db_params);

		STextureParams refl_params;
		refl_params.m_Width = m_DeviceParams.m_Width;
		refl_params.m_Height = m_DeviceParams.m_Height;
		refl_params.m_Format = IMGFMT_RGBA8;
		refl_params.m_MSAASampleCount = 1;
		refl_params.m_ShaderResource = true;
		refl_params.m_RenderTarget = true;
		m_Reflection = CreateTexture(m_Device, refl_params);

		db_params.m_Width = 1024;
		db_params.m_Height = 1024;

		m_CubeDepth = CreateTexture(m_Device, db_params);

		STextureParams tb_params;
		tb_params.m_Type = TextureType::TEX_2D_ARRAY;
		tb_params.m_Width = 3072;
		tb_params.m_Height = 3072;
		tb_params.m_Format = IMGFMT_D16;
		tb_params.m_MSAASampleCount = 1;
		tb_params.m_DepthTarget = true;
		tb_params.m_ShaderResource = true;

		tb_params.m_Slices = SHADOW_MAP_CASCADE_COUNT;
		m_ShadowMap = CreateTexture(m_Device, tb_params);

		STextureParams drop_params;

		drop_params.m_Type = TextureType::TEX_2D;
		drop_params.m_Width = 512;
		drop_params.m_Height = 512;
		drop_params.m_Format = IMGFMT_RGBA8;
		drop_params.m_MSAASampleCount = 1;
		drop_params.m_RenderTarget = true;
		drop_params.m_ShaderResource = true;
		m_RingDrop[0] = CreateTexture(m_Device, drop_params);
		m_RingDrop[1] = CreateTexture(m_Device, drop_params);

		STextureParams cube_params;

		cube_params.m_Type = TextureType::TEX_CUBE_ARRAY;
		cube_params.m_Width = 1024;
		cube_params.m_Height = 1024;
		cube_params.m_Format = IMGFMT_RGBA8;
		cube_params.m_MSAASampleCount = 1;
		cube_params.m_RenderTarget = true;
		cube_params.m_ShaderResource = true;
		cube_params.m_Slices = 2;
		m_CubeMapLayered = CreateTexture(m_Device, cube_params);

		cube_params.m_Width = 512;
		cube_params.m_Height = 512;

		cube_params.m_Slices = 1;
		cube_params.m_UnorderedAccess = true;
		cube_params.m_Type = TextureType::TEX_CUBE;
		cube_params.m_Name = "SphericalHarmonicsDiffuseMap";
		m_SHCubeMap = CreateTexture(m_Device, cube_params);


		cube_params.m_Type = TextureType::TEX_CUBE;
		cube_params.m_MipCount = 1;
		cube_params.m_Name = "EnvMap";
		m_CubeMapSingle = CreateTexture(m_Device, cube_params);

		cube_params.m_Type = TextureType::TEX_CUBE;
		cube_params.m_MipCount = 6;
		cube_params.m_Name = "SpecularEnvMap";
		m_SpecularEnvMap = CreateTexture(m_Device, cube_params);

		STextureParams lut_params;
		lut_params.m_Width = 512;
		lut_params.m_Height = 512;
		lut_params.m_Format = IMGFMT_RGBA8;
		lut_params.m_MSAASampleCount = 1;
		lut_params.m_ShaderResource = true;
		lut_params.m_UnorderedAccess = true;

		m_BRDFLookup = CreateTexture(m_Device, lut_params);

		m_ComputeSHTask.SetTextures({m_CubeMapSingle}, m_SHCubeMap);
		m_ComputeSHTask.CreateResources(m_Device);

		m_IntegrateEnvTask.SetTextures({ m_CubeMapSingle, {0,-1,-1} }, { m_SpecularEnvMap , {0, -1,-1} }, m_BRDFLookup);
		m_IntegrateEnvTask.CreateResources(m_Device);
		m_PBR.SetEnv(m_SpecularEnvMap, m_SHCubeMap, m_BRDFLookup);

		m_NormalTile = CreateTexture(m_Device, "../../Textures/water.dds", 1);
		float depthFar[2] = { 1,0 };
		float gray[4] = { 0.5, 0.5, 0.5, 0.5 };
		float black[4] = { 0,0,0,0 };
		float blue[4] = { 0.3,0.5,0.7, 1 };
		float blueMasked[4] = { 0.3,0.5,0.7, 0 };

		Context context = GetMainContext(m_Device);

		m_Shadows.SetShadowMap(m_ShadowMap);
		m_Shadows.SetPlanarReflectionParam(ReflectionMatrix(float3(3.75, -0.25, 1.75), float3(0, 1, 0)), float4(0, 1, 0, 0.25));
		m_Shadows.SetCubeProjection(true);
		Barrier(context, { { m_ShadowMap,  EResourceState::RS_SHADER_READ} });

		Barrier(context, { { m_CubeDepth,  EResourceState::RS_RENDER_TARGET} });

		Barrier(context, { { m_CubeMapLayered,  EResourceState::RS_RENDER_TARGET} });
		vec3 eye(3.25, 0.75, 1.75);
		

		m_TreeField.SetFilter(1);
		SetClearColors(context, blueMasked, depthFar);
		RenderEnvMap(context, m_CubeMapLayered, eye, 0);

		m_TreeField.SetFilter(2);
		SetClearColors(context, blue, depthFar);
		RenderEnvMap(context, m_CubeMapLayered, eye, 1);

		Barrier(context, { { m_CubeMapSingle,  EResourceState::RS_RENDER_TARGET} });

		m_TreeField.SetFilter(-1);
		SetClearColors(context, blue, depthFar);
		RenderEnvMap(context, m_CubeMapSingle, eye, 0);


		Barrier(GetMainContext(m_Device), { { {m_CubeMapLayered},  EResourceState::RS_SHADER_READ} });
		Barrier(GetMainContext(m_Device), { { {m_CubeMapSingle},  EResourceState::RS_SHADER_READ} });

		m_TreeField.SetFilter(-1);
		m_Shadows.SetCubeProjection(false);
		ResetCamera();
		UpdateCamera();

		m_Poly.UpdatePos(0.025, float2(0, 0));
		m_WaterTask.SetTextures(/*m_SHCubeMap*//* m_SpecularEnvMap*/m_CubeMapLayered, m_RingDrop[0], m_Reflection, m_NormalTile);
		m_WaterTask.SetBox(eye, vec3(5.5, 1.75, 4), vec3(2, -0.25, -0.5), vec3(8.5, 1.75, 7), vec3(-1, -0.25, -3.5));
		m_WaterTask.SetSpeed(WATERTEX_SPEED);
		m_WaterTask.SetWaveOffset(WAVE_OFFSET);
		m_Shadows.SetCubeProjection(false);
		m_ComputeSHTask.Execute(context, CComputeSHTask::Pass::ComputeBase);
	}
	~EvoApp()
	{
		DestroyTexture(m_Device, m_DepthBuffer);
		DestroyTexture(m_Device, m_ShadowMap);

	}
	void Update()
	{
		DemoApp::Update();
		m_WaterTask.Update(m_Timer.GetFrameTime());
	}

	void UpdateCamera()
	{
		float delta =  m_Timer.GetFrameTime()* m_CamSpeed;

		/*if (m_Keys[VK_LEFT] || m_Keys['A'])
			m_Jaw -=  delta;
		if (m_Keys[VK_RIGHT] || m_Keys['D'])
			m_Jaw +=  delta;

		if (m_Keys[VK_DOWN] || m_Keys['S'])
			m_Pitch -= delta;
		if (m_Keys[VK_UP] || m_Keys['W'])
			m_Pitch += delta;
			*/
		// assume initial dir be (0,0,1); 
		// calculate new y projecting new dir to y sin(pitch) where pitch - elevation above zx;
		// calculate new x & z from zx projection using jaw as angle from initial to positive x
		m_CamDir.x = sin(m_Jaw) * cos(m_Pitch);
		m_CamDir.y = sin(m_Pitch);
		m_CamDir.z = cos(m_Jaw) * cos(m_Pitch);
		
		vec3 aside = cross(m_CamDir, vec3(0, 1, 0));
		if (m_Keys[VK_LEFT] || m_Keys['A'])
			m_CamPos += aside *  delta;
		if (m_Keys[VK_RIGHT] || m_Keys['D'])
			m_CamPos -= aside * delta;

		if (m_Keys[VK_DOWN] || m_Keys['S'])
			m_CamPos -= m_CamDir * delta;
		if (m_Keys[VK_UP] || m_Keys['W'])
			m_CamPos += m_CamDir * delta;
		//m_CamPos = vec3(3.25, 0.75, 1.75);
		//m_CamDir = vec3(1, 0, 0);
		m_Shadows.SetCameraLookAt(m_CamPos, m_CamPos + m_CamDir, vec3(0,1,0));
		m_WaterTask.SetCameraLookAt(m_CamPos, m_CamPos + m_CamDir, vec3(0, 1, 0));
		m_PBR.SetCameraLookAt(m_CamPos, m_CamPos + m_CamDir, vec3(0, 1, 0));
	}
	virtual void ResetCamera()
	{
		m_CamPos = float3(2.5, 0.6f, -1.2);
		m_Jaw = 0;
		m_Pitch = 0;
	}

	void DrawFrame(Context context, uint buffer_index)
	{
		static uint mip = 0;
		if (mip < 6)
		{
			m_IntegrateEnvTask.Execute(context, CIntegrateEnvTask::Pass::ConvEnv, mip);
			mip++;
		}
		static bool SHReady = false;
		if (!SHReady && mip >=6)
		{
			m_IntegrateEnvTask.Execute(context, CIntegrateEnvTask::Pass::ConvBRDF);
			m_ComputeSHTask.Execute(context, CComputeSHTask::Pass::ComputeTex);
			SHReady = true;
		}

		static uint frame = 0;
		float depthFar[2] = { 1,0 };
		float gray[4] = { 0.5, 0.5, 0.5, 0.5 };
		float black[4] = { 0,0,0,0 };
		float blue[4] = { 0.3,0.5,0.7, 1 };
		SetRenderTarget(context, { (Texture)nullptr }, 0);
        Barrier(context, { { m_ShadowMap,  EResourceState::RS_RENDER_TARGET} });
		for (int i = 0; i < SHADOW_MAP_CASCADE_COUNT; i++)
		{
			SetDepthTarget(context, { m_ShadowMap, {i} });
			BeginRenderPass(context, "Shadow");
			m_Shadows.SetPassParameters(context, ShadowMapCascade::ShadowPass, i);
			m_Shadows.Draw(context);
			EndRenderPass(context);
		}

        Barrier(context, { { m_ShadowMap,  EResourceState::RS_SHADER_READ} });

        SetDepthTarget(context, { (Texture)nullptr });
		float delta = m_Timer.GetFrameTime();
		if (m_CurDropTex == 1 && (m_dropTime == 0 || m_dropTime > WATERDROP_SPEED*100))
		{
			m_dropTime -= WATERDROP_SPEED * 100;
			{
				SetClearColors(context, gray, depthFar);
				SetRenderTarget(context, { m_RingDrop[0] }, 0);

				Barrier(context, { { m_RingDrop[0],  EResourceState::RS_RENDER_TARGET} });

				BeginRenderPass(context, "Drop");
				m_PolyTask.Draw(context);
				EndRenderPass(context);

				Barrier(context, { { m_RingDrop[0],  EResourceState::RS_SHADER_READ} });

				m_Quad.SetTexture(m_RingDrop[0]);
				SetClearColors(context, blue, depthFar);

			}
		}
		m_dropTime += delta;
		m_dropUpdateTime += delta;
		if (m_dropUpdateTime > WATERDROP_SPEED)
		{
			m_dropUpdateTime -= WATERDROP_SPEED;
			SetRenderTarget(context, { m_RingDrop[m_CurDropTex] }, 0);

			Barrier(context, { { m_RingDrop[m_CurDropTex], EResourceState::RS_RENDER_TARGET} });

			BeginRenderPass(context, "Phys");
			m_DropTask.Draw(context);
			EndRenderPass(context);

			Barrier(context, { { m_RingDrop[m_CurDropTex], EResourceState::RS_SHADER_READ} });

			m_Quad.SetTexture(m_RingDrop[m_CurDropTex]);

			m_CurDropTex = 1 - m_CurDropTex;
		}
		
		SetRenderTarget(context, m_Reflection, 0);
		SetDepthTarget(context, m_DepthBuffer);
		Barrier(context, { {m_Reflection , EResourceState::RS_RENDER_TARGET} });
		BeginRenderPass(context, "Reflection");
		m_Shadows.SetPassParameters(context, ShadowMapCascade::Reflection);
		m_Shadows.Draw(context);
		EndRenderPass(context);
		Barrier(context, { { m_Reflection , EResourceState::RS_SHADER_READ} });

		Texture bb = GetBackBuffer(GetDevice(), buffer_index);
		SetRenderTarget(context, bb, 0);
		SetDepthTarget(context, m_DepthBuffer);
		Barrier(context, { {bb , EResourceState::RS_RENDER_TARGET} });
        Barrier(context, { { m_DepthBuffer,  EResourceState::RS_RENDER_TARGET} });

		BeginRenderPass(context, "Main");
		m_Shadows.SetPassParameters(context, ShadowMapCascade::MainPass);
		m_Shadows.Draw(context);
		m_PBR.Draw(context);
		EndRenderPass(context);
		
		BeginRenderPass(context, "Water", false, false);
		m_WaterTask.Draw(context);
		EndRenderPass(context);
		
		Barrier(context, { { bb , EResourceState::RS_PRESENT} });
		

		frame++;
	};
};
static DemoApp *app = nullptr; // Should come up with something prettier than this

#define GETX(l) (int(l & 0xFFFF))
#define GETY(l) (int(l) >> 16)

bool g_Active = true;

void InitializeInput(HWND hwnd)
{
	RAWINPUTDEVICE devices[] =
	{
		{ 1, 2, RIDEV_INPUTSINK, hwnd }, // Mouse
		{ 1, 6, RIDEV_INPUTSINK, hwnd }, // Keyboard
	};

	BOOL result = RegisterRawInputDevices(devices, elementsof(devices), sizeof(RAWINPUTDEVICE));
	ASSERT(result);
}


LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:
		PAINTSTRUCT paint;
		BeginPaint(hwnd, &paint);
		g_Active = !IsRectEmpty(&paint.rcPaint);
		EndPaint(hwnd, &paint);
		break;

	case WM_INPUT:
		{
			HRAWINPUT hRawInput = (HRAWINPUT) lParam;

			RAWINPUT raw_input;

			UINT size = sizeof(raw_input);
			GetRawInputData(hRawInput, RID_INPUT, &raw_input, &size, sizeof(RAWINPUTHEADER));

			const bool has_focus = (GET_RAWINPUT_CODE_WPARAM(wParam) == RIM_INPUT);


			if (raw_input.header.dwType == RIM_TYPEKEYBOARD)
			{
				const bool pressed = ((raw_input.data.keyboard.Flags & RI_KEY_BREAK) == 0);

				if (has_focus)
				{
					app->OnKey(raw_input.data.keyboard.VKey, pressed);

//	 				char str[256];
// 					sprintf(str, "Keyboard: %d, Flags: %d\n", raw_input.data.keyboard.VKey, raw_input.data.keyboard.Flags);
// 					OutputDebugStringA(str);
				}

				app->UpdateKey(raw_input.data.keyboard.VKey, pressed);

			}
			else if (raw_input.header.dwType == RIM_TYPEMOUSE)
			{
// 				char str[256];
// 				sprintf(str, "Mouse: %d, %d\n", raw_input.data.mouse.lLastX, raw_input.data.mouse.lLastY);
// 				OutputDebugStringA(str);

				if (has_focus)
				{
					app->UpdateMouse(raw_input.data.mouse.lLastX, raw_input.data.mouse.lLastY);
				}

				//app->OnMouseButton(raw_input.data.mouse.
				//raw_input.data.mouse
			}

			return DefWindowProc(hwnd, message, wParam, lParam);
		}
		break;
	case WM_MOUSEMOVE:
		app->OnMouseMove(GETX(lParam), GETY(lParam));
		break;
	case WM_LBUTTONDOWN:
		app->OnMouseButton(GETX(lParam), GETY(lParam), MOUSE_LEFT, true);
 		break;
	case WM_LBUTTONUP:
		app->OnMouseButton(GETX(lParam), GETY(lParam), MOUSE_LEFT, false);
		break;
	case WM_RBUTTONDOWN:
		app->OnMouseButton(GETX(lParam), GETY(lParam), MOUSE_RIGHT, true);
 		break;
	case WM_RBUTTONUP:
		app->OnMouseButton(GETX(lParam), GETY(lParam), MOUSE_RIGHT, false);
		break;
	case WM_MBUTTONDOWN:
		app->OnMouseButton(GETX(lParam), GETY(lParam), MOUSE_MIDDLE, true);
		break;
	case WM_MBUTTONUP:
		app->OnMouseButton(GETX(lParam), GETY(lParam), MOUSE_MIDDLE, false);
		break;
	case WM_MOUSEWHEEL:
		static int scroll;
		int s;

		scroll += GET_WHEEL_DELTA_WPARAM(wParam);
		s = scroll / WHEEL_DELTA;
		scroll %= WHEEL_DELTA;

		POINT point;
		point.x = GETX(lParam);
		point.y = GETY(lParam);
		ScreenToClient(hwnd, &point);

		if (s != 0)
			app->OnMouseWheel(point.x, point.y, s);
		break;
	case WM_SETFOCUS:
		app->SetInputFocus(true);
		break;
	case WM_KILLFOCUS:
		app->SetInputFocus(false);
		break;
	case WM_SIZE:
		// Ignore WM_SIZE during window creation
		//if (app->GetDevice()->GetWindow() != nullptr)
		if (app->GetDevice() != nullptr)
		{
			app->OnSize(LOWORD(lParam), HIWORD(lParam));
		}
		break;
/*	case WM_WINDOWPOSCHANGED:
		if ((((LPWINDOWPOS) lParam)->flags & SWP_NOSIZE) == 0)
		{
			RECT rect;
			GetClientRect(hwnd, &rect);
			int w = rect.right - rect.left;
			int h = rect.bottom - rect.top;
			if (w > 0 && h > 0) app->onSize(w, h);
		}
		break;*/
	/*case WM_SYSKEYDOWN:
		if ((lParam & (1 << 29)) && wParam == KEY_ENTER)
		{
			app->toggleFullscreen();
		}
		else
		{
			app->OnKey((unsigned int) wParam, true);
		}
		break;*/
	case WM_SYSKEYUP:
		app->OnKey((unsigned int) wParam, false);
		break;
	case WM_CREATE:
		ShowWindow(hwnd, SW_SHOW);
		InitializeInput(hwnd);
		break;
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hThisInst, HINSTANCE /*hLastInst*/, LPSTR /*lpszCmdLine*/, int /*nCmdShow*/)
{
#ifdef _DEBUG
	int flag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG); // Get current flag
	flag |= _CRTDBG_LEAK_CHECK_DF; // Turn on leak-checking bit
	flag |= _CRTDBG_CHECK_ALWAYS_DF; // Turn on CrtCheckMemory
//	flag |= _CRTDBG_DELAY_FREE_MEM_DF;
	_CrtSetDbgFlag(flag); // Set flag to the new value
#endif
#if defined(GRAPHICS_API_D3D12)  && defined(RENDERDOC)
	HMODULE handle = LoadLibraryA("renderdoc.dll"); // renderdoc must be loaded before DX dlls
#endif
//	initCPU();

	// Make sure we're running in the exe's path
	// TODO: Make unicode
	char path[MAX_PATH];
	if (GetModuleFileNameA(nullptr, path, sizeof(path)))
	{
		char *slash = strrchr(path, '\\');
		if (slash) *slash = '\0';
		//chdir(path);
		SetCurrentDirectoryA(path);
	}

	WNDCLASSEX wc;
	wc.cbSize = sizeof(wc);
	wc.style = 0;
	wc.lpfnWndProc = WinProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hThisInst;
	wc.hIcon = LoadIcon(hThisInst, MAKEINTRESOURCE(IDI_MAINICON));
	wc.hCursor = LoadCursor(nullptr, IDI_APPLICATION);
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = L"Humus";
	wc.hIconSm = nullptr;

	ATOM win_class = RegisterClassEx(&wc);
	if (!win_class) return 0;


	/*
		Force the main thread to always run on CPU 0.
		This is done because on some systems QueryPerformanceCounter returns a bit different counter values
		on the different CPUs (contrary to what it's supposed to do), which can cause negative frame times
		if the thread is scheduled on the other CPU in the next frame. This can cause very jerky behavior and
		appear as if frames return out of order.
	*/
	SetThreadAffinityMask(GetCurrentThread(), 1);

	MSG msg;
	msg.wParam = 0;
	EvoApp m_App;
	app = &m_App;
	if (app->Create())
	{
		if (app->Init())
		{
			app->Start();


			bool quit = false;
			for (;;)
			{


				while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
				{
					if (msg.message == WM_QUIT)
					{
						quit = true;
					}

					DispatchMessage(&msg);
				}

				if (quit)
					break;

				if (g_Active)
				{

					app->MakeFrame();
				}
				else
				{
					Sleep(100);
				}
			}

		}

		app->Exit();
	}

	app->Destroy();
//	delete app;

	//UnregisterClass((LPCWSTR) win_class, hThisInst);

	return (int) msg.wParam;
}
