/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\ 
*Modified 2021 Oleh Sopilniak																																	*
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

/* * * * * * * * * * * * * Author's note * * * * * * * * * * * *\
*   _       _   _       _   _       _   _       _     _ _ _ _   *
*  |_|     |_| |_|     |_| |_|_   _|_| |_|     |_|  _|_|_|_|_|  *
*  |_|_ _ _|_| |_|     |_| |_|_|_|_|_| |_|     |_| |_|_ _ _     *
*  |_|_|_|_|_| |_|     |_| |_| |_| |_| |_|     |_|   |_|_|_|_   *
*  |_|     |_| |_|_ _ _|_| |_|     |_| |_|_ _ _|_|  _ _ _ _|_|  *
*  |_|     |_|   |_|_|_|   |_|     |_|   |_|_|_|   |_|_|_|_|    *
*                                                               *
*                     http://www.humus.name                     *
*                                                                *
* This file is a part of the work done by Humus. You are free to   *
* use the code in any way you like, modified, unmodified or copied   *
* into your own work. However, I expect you to respect these points:  *
*  - If you use this file and its contents unmodified, or use a major *
*    part of this file, please credit the author and leave this note. *
*  - For use in anything commercial, please request my approval.     *
*  - Share your work and ideas too as much as you can.             *
*                                                                *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#ifdef GRAPHICS_API_VULKAN
#include "../Renderer.h"
#include "../RootSignature.h"
#include "../../Util/Array.h"
#include <map>
#include <unordered_map>
#include <vector>
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>
#pragma comment (lib, "vulkan-1.lib")
#include <assert.h>
#include <stdio.h>
//#ifdef DEBUG
//#define USE_DEBUG_MARKERS
//#define USE_DEBUG_UTILS
//#endif
#define VALIDATION_LAYER_NAME "VK_LAYER_LUNARG_standard_validation"

struct SlicedBuffer
{
	Buffer	m_Buffer;
	uint8*  m_Data;
	uint    m_Cursor;
};
typedef  std::map<uint64, RenderPass> RenderPassDictionary;
typedef std::vector<RenderSetup>	RenderSetupVector;
struct SCommandList
{
	RenderSetupVector* m_RenderSetups;
	VkFence m_WaitFence;
	VkCommandBuffer m_VkCommandBuffer;
	VkDescriptorPool m_VkDescriptorPool;
	SlicedBuffer m_Constants;

	SlicedBuffer m_Staging;
	VkQueryPool m_QueryPool;
	Buffer	    m_QueryBuffer;
	uint        m_QueryOffset;
	const char* m_QueryNames[MAX_QUERY_COUNT];

	void Init(Device device);
	void Destroy(Device device);

	void Begin(Device device, uint size);
	void End(Device device);
};

uint32 AllocateBufferSlice(Device device, SlicedBuffer& buffer, uint size, uint alignment);

static VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanCallback(VkDebugUtilsMessageSeverityFlagBitsEXT        messageSeverityBit,
	VkDebugUtilsMessageTypeFlagsEXT               messageTypeBit,
	VkDebugUtilsMessengerCallbackDataEXT const* callbackData,
	void* userData)
{
    OutputDebugStringA(callbackData->pMessage);
    OutputDebugStringA("\n\n");
	return VK_FALSE;
}
template<>
struct std::hash<SSamplerDesc>
{
	std::size_t operator()(SSamplerDesc const& s) const noexcept
	{
		std::size_t h1 = std::hash<int>{}(s.Filter);
		std::size_t h2 = std::hash<int>{}(s.Aniso) ^ (h1 << 1);
		std::size_t h3 = std::hash<int>{}(s.AddressModeU) ^ (h2 << 1);
		std::size_t h4 = std::hash<int>{}(s.AddressModeV) ^ (h3 << 1);
		std::size_t h5 = std::hash<int>{}(s.AddressModeW) ^ (h4 << 1);
		std::size_t h6 = std::hash<int>{}(s.Comparison) ^ (h5 << 1);


		return h6;
	}
};
typedef std::unordered_map<SSamplerDesc, VkSampler> TSamplerDictionary;

struct SDevice
{
	VkDevice m_VkDevice;
	VkQueue  m_VkGraphicsQueue;
	uint     m_VkGraphicsQueueIndex;

	HWND m_Window;

	Context m_MainContext;
	SCommandList m_CommandBuffers[BUFFER_FRAMES];
	RenderPassDictionary* m_RenderpassDictionary;
	TSamplerDictionary* m_SamplerDictionary;
	Texture m_BackBuffer[BUFFER_FRAMES];

	VkSemaphore m_PresentSemaphore;
	uint32 m_VkSwapChainImageId;
	bool m_WasAcquired;

	VkCommandPool m_VkCommandPool;
	VkDescriptorPool m_VkDescriptorPool;
	RenderPass m_BackBufferRenderPass;

	// Default states
	BlendState m_DefaultBlendState;

	VkPhysicalDevice m_VkPhysicalDevice;
	float m_TimestampFrequency;
	uint m_UniformBufferAlignment;
	uint m_MaxDrawMeshTasksCount;
	VkSampleCountFlags m_MSAASupportMask;
	VkPhysicalDeviceMemoryProperties m_VkMemoryProperties;

	VkSwapchainCreateInfoKHR m_VkSwapChainCreateInfo;
	VkSwapchainKHR m_VkSwapChain;
	VkSurfaceKHR m_VkSurface;
	VkInstance m_VkInstance;

	StaticArray<DisplayMode> m_DisplayModes;
};



struct SContext
{
	SCommandList* m_CmdList;
	RootSignature m_CurrentRootSignature;
	SFrameBuffer m_CurrentFrameBufferDesc;
	RenderSetup m_CurrentRenderSetup;
	float m_ClearColor[4];
	float m_ClearDepth[2];

	Device m_Device;

	bool m_IsProfiling;
	VkCommandBuffer GetVkCommandBuffer() { return m_CmdList ? m_CmdList->m_VkCommandBuffer:VK_NULL_HANDLE; }
};

static const VkImageViewType g_TextureTypes[] =
{
	VK_IMAGE_VIEW_TYPE_1D,
	VK_IMAGE_VIEW_TYPE_1D_ARRAY,
	VK_IMAGE_VIEW_TYPE_2D,
	VK_IMAGE_VIEW_TYPE_2D_ARRAY,
	VK_IMAGE_VIEW_TYPE_CUBE,
	VK_IMAGE_VIEW_TYPE_CUBE_ARRAY,
	VK_IMAGE_VIEW_TYPE_3D,
};

static const VkFormat g_Formats[] =
{
	VK_FORMAT_UNDEFINED,

	VK_FORMAT_R8_UNORM,
	VK_FORMAT_R8_UINT,

	VK_FORMAT_R8G8_UNORM,
	VK_FORMAT_R8G8_UINT,

	VK_FORMAT_R8G8B8A8_UNORM,
	VK_FORMAT_R8G8B8A8_UINT,

	VK_FORMAT_R16_UNORM,
	VK_FORMAT_R16_UINT,
	VK_FORMAT_R16_SFLOAT,

	VK_FORMAT_R16G16_UNORM,
	VK_FORMAT_R16G16_UINT,
	VK_FORMAT_R16G16_SFLOAT,

	VK_FORMAT_R16G16B16A16_UNORM,
	VK_FORMAT_R16G16B16A16_UINT,
	VK_FORMAT_R16G16B16A16_SFLOAT,

	VK_FORMAT_R32_UINT,
	VK_FORMAT_R32_SFLOAT,

	VK_FORMAT_R32G32_UINT,
	VK_FORMAT_R32G32_SFLOAT,

	VK_FORMAT_R32G32B32A32_UINT,
	VK_FORMAT_R32G32B32A32_SFLOAT,

	VK_FORMAT_BC1_RGB_UNORM_BLOCK,
	VK_FORMAT_BC2_UNORM_BLOCK,
	VK_FORMAT_BC3_UNORM_BLOCK,
	VK_FORMAT_BC4_UNORM_BLOCK,
	VK_FORMAT_BC5_UNORM_BLOCK,
	VK_FORMAT_BC7_UNORM_BLOCK,

	VK_FORMAT_D16_UNORM,

	VK_FORMAT_B8G8R8A8_UNORM,
};
static_assert(elementsof(g_Formats) == IMGFMT_COUNT, "g_Formats incorrect length");


static const VkFormat g_AttribFormats[] =
{
	VK_FORMAT_R32_SFLOAT,
	VK_FORMAT_R32G32_SFLOAT,
	VK_FORMAT_R32G32B32_SFLOAT,
	VK_FORMAT_R32G32B32A32_SFLOAT,

	VK_FORMAT_R16G16_SFLOAT,
	VK_FORMAT_R16G16B16A16_SFLOAT,

	VK_FORMAT_R32_UINT,
	VK_FORMAT_R32G32_UINT,
	VK_FORMAT_R32G32B32_UINT,
	VK_FORMAT_R32G32B32A32_UINT,
};

enum TexViewUsage
{
	SRV,
	UAV,
	RTV,
	DSV
};
enum TextureViewFlags :uint8
{
	ViewDefault = 0x0,
	ForceArray = 0x1,
	CubeAsArray = 0x2
};
struct SView
{
	VkImageViewType m_viewType;
};
union SViewKey
{
	SView view;
	uint32 combo;
};
struct SRange
{
	ushort baseMipLevel, levelCount;
	ushort baseArrayLayer, layerCount;
};
typedef std::map<uint32, VkImageView> TViewMap;
struct STextureSubresource
{
	STexture* m_Owner;
	STextureSubresource** m_Faces;//for cubemaps only 
	TViewMap* m_Views;
	SRange m_Range;
	EResourceState m_State;	
};


union SRangeKey
{
	SRange range;
	uint64 combo;
};
typedef std::map<uint64, STextureSubresource*> TextureSubresourceDictionary;
struct STexture
{
	VkImage m_Image;
	STextureSubresource** m_Subresources;
	TextureSubresourceDictionary* m_SubresourceDictionary;// to prevent range duplicates
	uint m_Width;
	uint m_Height;
	uint m_Depth;
	uint m_Slices;
	uint m_MipLevels;
	TextureType m_Type;
	ImageFormat m_Format;
	VkDeviceMemory m_Memory;
};

STextureSubresource* AcquireTextureSubresource(const SResourceDesc& resourceDesc)
{
	assert(resourceDesc.m_Type == RESTYPE_TEXTURE);
	const STextureSubresourceDesc& desc = resourceDesc.m_texRange;
	Texture texture = (Texture)resourceDesc.m_Resource;
	if (!texture->m_Subresources)
	{
		uint size = texture->m_Slices * (texture->m_MipLevels + 1) + 1;
		texture->m_Subresources = new STextureSubresource * [size];
		memset(texture->m_Subresources, 0, sizeof(STextureSubresource*) * size);
		texture->m_SubresourceDictionary = new TextureSubresourceDictionary;
	}
	
	assert(desc.slice >= 0 || desc.mip == -1);
	uint index = desc.mip ==  -1 ? desc.slice + 1 : texture->m_Slices + 1 + (texture->m_MipLevels*desc.slice + desc.mip);

 	bool isCube = texture->m_Type == TEX_CUBE || texture->m_Type == TEX_CUBE_ARRAY;

	STextureSubresource*& subResource =	texture->m_Subresources[index];
	if (!subResource)
	{
		uint ifCube6 = isCube ? 6 : 1;

		SRangeKey key;
		key.range.baseMipLevel = desc.mip >= 0 ? desc.mip : 0;
		key.range.levelCount = desc.mip >= 0 ? 1 : texture->m_MipLevels;
		key.range.baseArrayLayer = desc.slice >= 0 ? ifCube6 *desc.slice : 0;
		key.range.layerCount = desc.slice >= 0 ? ifCube6 : ifCube6 *texture->m_Slices;
		
		auto it = texture->m_SubresourceDictionary->find(key.combo);
		if (it == texture->m_SubresourceDictionary->end())
		{
			subResource = new STextureSubresource;
			subResource->m_Owner = texture;
			subResource->m_Range.baseMipLevel = key.range.baseMipLevel;
			subResource->m_Range.levelCount = key.range.levelCount;
			subResource->m_Range.baseArrayLayer = key.range.baseArrayLayer;
			subResource->m_Range.layerCount = key.range.layerCount;
			subResource->m_Views = new TViewMap;
			subResource->m_Faces = nullptr;
			
			texture->m_SubresourceDictionary->insert(TextureSubresourceDictionary::value_type(key.combo, subResource));
		}
		else
			subResource = it->second;

	}
	if (isCube && desc.face >= 0)
	{
		assert(desc.face < 6);
		if (!subResource->m_Faces)
		{
			subResource->m_Faces = new STextureSubresource * [6];
			memset(subResource->m_Faces, 0, 6 * sizeof(STextureSubresource*));
		}
		STextureSubresource*& face = subResource->m_Faces[desc.face];

		if (!face)
		{
			face = new STextureSubresource;
			face->m_Owner = texture;
			face->m_Range.baseMipLevel = subResource->m_Range.baseMipLevel;
			face->m_Range.levelCount = subResource->m_Range.levelCount;
			face->m_Range.baseArrayLayer = subResource->m_Range.baseArrayLayer + desc.face;
			face->m_Range.layerCount = 1;
			face->m_Views = new TViewMap;
			face->m_Faces = nullptr;
		}
		return face;
	}
	return subResource;
}

VkImageView AcquireTextureSubresourceView(Device device, const SResourceDesc& resourceDesc,  TexViewUsage usageType, TextureViewFlags flags = TextureViewFlags::ViewDefault)
{
	assert(resourceDesc.m_Type == RESTYPE_TEXTURE);
	const STextureSubresourceDesc& desc = resourceDesc.m_texRange;
	Texture texture = (Texture)resourceDesc.m_Resource;

	STextureSubresource* sub = AcquireTextureSubresource(resourceDesc);
	TextureType type = sub->m_Owner->m_Type;
	if (resourceDesc.m_texRange.slice >= 0)
	{
		bool arrayedType = type == TEX_1D_ARRAY || type == TEX_2D_ARRAY || type == TEX_CUBE_ARRAY;

		if (arrayedType && (!(flags & TextureViewFlags::ForceArray)))
		{
			switch (type)
			{
			case TEX_1D_ARRAY:
				type = TEX_1D;
				break;
			case TEX_2D_ARRAY:
				type = TEX_2D;
				break;
			case TEX_CUBE_ARRAY:
				type = TEX_CUBE;
				break;
			}
		}
	}

	if (type == TEX_CUBE_ARRAY || type == TEX_CUBE)
	{
		if (resourceDesc.m_texRange.face >= 0)
			type = TEX_2D;
		else
		{

			if (flags & TextureViewFlags::CubeAsArray)
			{
				type = TEX_2D_ARRAY;
			}
		}
	}
	
	VkImageViewType nativeType = g_TextureTypes[type];
	SViewKey key; key.view = { nativeType };
	TViewMap::iterator view = sub->m_Views->find(key.combo);
	if (view != sub->m_Views->end())
		return view->second;
	else
	{
		sub->m_Views->insert(TViewMap::value_type(key.combo, VK_NULL_HANDLE));
		view = sub->m_Views->find(key.combo);
		/**/
		//todo: add stencil support
		VkImageViewCreateInfo view_create_info = {};
		view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		view_create_info.image = sub->m_Owner->m_Image;
		view_create_info.viewType = nativeType;
		view_create_info.format = g_Formats[sub->m_Owner->m_Format];
		view_create_info.components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
		view_create_info.subresourceRange.baseMipLevel = sub->m_Range.baseMipLevel;
		view_create_info.subresourceRange.levelCount = sub->m_Range.levelCount;
		view_create_info.subresourceRange.baseArrayLayer = sub->m_Range.baseArrayLayer;
		view_create_info.subresourceRange.layerCount = sub->m_Range.layerCount; 
		view_create_info.subresourceRange.aspectMask = IsDepthFormat(texture->m_Format) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
		VkResult res = vkCreateImageView(device->m_VkDevice, &view_create_info, VK_NULL_HANDLE, &view->second);
		ASSERT(res == VK_SUCCESS);
		return view->second;
	}
}

struct SResourceTable
{
	VkDescriptorSet m_DescriptorSet;
	VkDescriptorPool m_Pool;
};

struct SRenderPass
{
	VkRenderPass m_RenderPass;
	RenderPassFlags m_Flags;
	uint m_MSAASamples;
};

struct SSamplerTable
{
	VkDescriptorSet m_DescriptorSet;
	uint m_Count;
	VkSampler m_Samplers[1];
	VkDescriptorPool m_Pool;
};

struct SRenderSetup
{
	VkFramebuffer m_FrameBuffer;
	uint m_Width;
	uint m_Height;
	RenderPass m_Pass;
	VkImageView m_Views[MAX_COLOR_TARGETS + 2];
};

struct SRootSignature
{
	struct SRootSlot
	{
		ItemType m_Type;
		uint32   m_Size;
		uint32   m_Offset;

		VkDescriptorSetLayout m_DescriptorSetLayout;
		SResourceTableItem* m_Items;
	};

	uint32 m_SlotCount;
	SRootSlot* m_Slots;

	VkPipelineLayout m_PipelineLayout;
};
void IterateRootSignature(SRootSignature* root, void* _callback, void* receiver)
{
	RootCallback callback = (RootCallback)_callback;
	for (int slot = 0; slot < root->m_SlotCount; slot++)
	{
		for (int i = 0; i < root->m_Slots[slot].m_Size; i++)
		{
			callback(root->m_Slots[slot].m_Items[i].m_Type, slot, i, i == 0? root->m_Slots[slot].m_Size:0, receiver);
		}
	}
}
struct SPipeline
{
	VkPipeline m_Pipeline;
	VkPipelineBindPoint m_BindPoint;
	VkShaderModule m_Modules[3];
};

struct SBlendState
{
	BlendFactor m_Src;
	BlendFactor m_Dst;
	BlendMode m_Mode;
	uint32 m_Mask;
};

struct SVertexSetup
{
	VkBuffer m_IndexBuffer;
	VkBuffer m_VertexBuffer;
	VkIndexType m_IndexType;
};

struct SBuffer
{
	VkBuffer m_Buffer;
	uint     m_Size;
	HeapType m_HeapType;
	EResourceState m_State;
	VkDeviceMemory m_Memory;
};

PFN_vkCmdDrawMeshTasksNV vkCmdDrawMeshTasks = VK_NULL_HANDLE;

#ifdef USE_DEBUG_MARKERS
PFN_vkCmdDebugMarkerBeginEXT vkCmdDebugMarkerBegin = VK_NULL_HANDLE;
PFN_vkCmdDebugMarkerEndEXT vkCmdDebugMarkerEnd = VK_NULL_HANDLE;
#endif
#ifdef USE_DEBUG_UTILS
PFN_vkCreateDebugUtilsMessengerEXT                  m_fnCreateDebugUtilsMessengerEXT = VK_NULL_HANDLE;
PFN_vkDestroyDebugUtilsMessengerEXT                 m_fnDestroyDebugUtilsMessengerEXT = VK_NULL_HANDLE;
#endif


static bool CreateBackBufferSetups(Device device, uint width, uint height, ImageFormat format)
{
	// Get the swap chain images
	VkImage back_buffers[BUFFER_FRAMES];
	uint32_t image_count = 0;
	VkResult res = vkGetSwapchainImagesKHR(device->m_VkDevice, device->m_VkSwapChain, &image_count, VK_NULL_HANDLE);
	if (res == VK_SUCCESS)
	{
		ASSERT(image_count == BUFFER_FRAMES);
		res = vkGetSwapchainImagesKHR(device->m_VkDevice, device->m_VkSwapChain, &image_count, back_buffers);
		if (res == VK_SUCCESS)
		{
			for (uint i = 0; i < BUFFER_FRAMES; i++)
			{
				//back_buffer->SetName(L"BackBuffer");

				Texture texture = new STexture();
				memset(texture, 0, sizeof(STexture));
				texture->m_Image     = back_buffers[i];
				texture->m_Width     = width;
				texture->m_Height    = height;
				texture->m_Depth     = 1;
				texture->m_Slices    = 1;
				texture->m_MipLevels = 1;
				texture->m_Type      = TEX_2D;
				texture->m_Format    = format;

				device->m_BackBuffer[i] = texture;
			}

			return true;
		}
	}

	ErrorMsg("Couldn't get backbuffers");
	return false;
}

static void DestroyBackBufferSetups(Device device)
{
	for (uint i = 0; i < BUFFER_FRAMES; i++)
	{
		device->m_BackBuffer[i]->m_Image = VK_NULL_HANDLE; // These are owned by the swapchain, so don't explicitly destroy them
		DestroyTexture(device, device->m_BackBuffer[i]);
	}
}

static int32 GetMemoryTypeIndex(const VkPhysicalDeviceMemoryProperties& mem_properties, uint32_t type_bits, VkMemoryPropertyFlags properties)
{
	for (uint32_t i = 0; i < mem_properties.memoryTypeCount; i++)
	{
		if ((type_bits & 1) != 0)
		{
			if ((mem_properties.memoryTypes[i].propertyFlags & properties) == properties)
			{
				return i;
			}
		}
		type_bits >>= 1;
	}

	ASSERT(false);
	return -1;
}

static VkPresentModeKHR GetPresentMode(VkPhysicalDevice physical_device, VkSurfaceKHR surface, bool vsync)
{
	uint32_t present_modes_count = 0;
	VkResult res = vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_modes_count, VK_NULL_HANDLE);
	ASSERT(res == VK_SUCCESS);

	VkPresentModeKHR* present_modes = StackAlloc<VkPresentModeKHR>(present_modes_count);
	res = vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_modes_count, present_modes);
	ASSERT(res == VK_SUCCESS);

	for (uint32_t i = 0; i < present_modes_count; i++)
	{
		if (present_modes[i] <= VK_PRESENT_MODE_MAILBOX_KHR)
		{
			if (!vsync)
				return present_modes[i];
		} else if (present_modes[i] <= VK_PRESENT_MODE_FIFO_RELAXED_KHR)
		{
			if (vsync)
				return present_modes[i];
		}
	}

	ASSERT(false);
	return present_modes[0];
}

static VkSurfaceFormatKHR GetSwapchainFormat(VkPhysicalDevice physical_device, VkSurfaceKHR surface)
{
	uint32_t format_count = 0;
	VkResult res = vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count, VK_NULL_HANDLE);
	ASSERT(res == VK_SUCCESS);

	VkSurfaceFormatKHR* formats = (VkSurfaceFormatKHR*) alloca(format_count * sizeof(VkSurfaceFormatKHR));
	res = vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count, formats);
	ASSERT(res == VK_SUCCESS);

	for (uint32_t i = 0; i < format_count; i++)
	{
		if (formats[i].format == VK_FORMAT_R8G8B8A8_UNORM ||
			formats[i].format == VK_FORMAT_B8G8R8A8_UNORM)
		{
			return formats[i];
		}
	}

	ASSERT(false);
	return formats[0];
}

static VkPhysicalDevice ChoosePhysicalDevice(VkInstance instance)
{
	uint32_t gpu_count = 0;
	VkResult res = vkEnumeratePhysicalDevices(instance, &gpu_count, VK_NULL_HANDLE);
	ASSERT(res == VK_SUCCESS);

	if (gpu_count)
	{
		// Enumerate devices
		VkPhysicalDevice* physical_devices = StackAlloc<VkPhysicalDevice>(gpu_count);
		res = vkEnumeratePhysicalDevices(instance, &gpu_count, physical_devices);
		ASSERT(res == VK_SUCCESS);

		// First look for any discrete GPU
		VkPhysicalDeviceProperties properties;
		for (uint32_t i = 0; i < gpu_count; i++)
		{
			vkGetPhysicalDeviceProperties(physical_devices[i], &properties);
			if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
				return physical_devices[i];
		}

		// Otherwise just pick the first
		return physical_devices[0];
	}

	return VK_NULL_HANDLE;
}

static void CreateSwapchain(Device device, bool vsync)
{
	if (device->m_VkSwapChain)
		vkDestroySwapchainKHR(device->m_VkDevice, device->m_VkSwapChain, VK_NULL_HANDLE);

	VkSurfaceCapabilitiesKHR surface_capabilities;
	VkResult res = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device->m_VkPhysicalDevice, device->m_VkSurface, &surface_capabilities);
	ASSERT(res == VK_SUCCESS);

	device->m_VkSwapChainCreateInfo.presentMode = GetPresentMode(device->m_VkPhysicalDevice, device->m_VkSurface, vsync);

	res = vkCreateSwapchainKHR(device->m_VkDevice, &device->m_VkSwapChainCreateInfo, VK_NULL_HANDLE, &device->m_VkSwapChain);
	ASSERT(res == VK_SUCCESS);
}

static bool IsExtensionSupported(const Array<VkExtensionProperties>& extensions, const char* name)
{
	uint count = extensions.GetCount();
	for (uint i = 0; i < count; i++)
	{
		if (strcmp(name, extensions[i].extensionName) == 0)
			return true;
	}
	return false;
}

Device CreateDevice(DeviceParams& params)
{
	HINSTANCE inst = GetModuleHandle(VK_NULL_HANDLE);

	DWORD style = params.m_Fullscreen? WS_POPUP : WS_OVERLAPPEDWINDOW;

	int x, y, w, h;

	if (params.m_Fullscreen)
	{
		DEVMODEW dm = {};
		dm.dmSize = sizeof(dm);
		dm.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;
		dm.dmPelsWidth  = params.m_FullscreenWidth;
		dm.dmPelsHeight = params.m_FullscreenHeight;
		ChangeDisplaySettingsW(&dm, CDS_FULLSCREEN);

		x = 0;
		y = 0;
		w = params.m_FullscreenWidth;
		h = params.m_FullscreenHeight;
	}
	else
	{
		RECT rect;
		rect.left = 0;
		rect.top  = 0;
		rect.right  = params.m_WindowedWidth;
		rect.bottom = params.m_WindowedHeight;
		AdjustWindowRectEx(&rect, style, FALSE, 0);

		x = rect.left;
		y = rect.top;
		w = rect.right - rect.left;
		h = rect.bottom - rect.top;

		RECT workarea;
		SystemParametersInfo(SPI_GETWORKAREA, 0, &workarea, 0);

		x += (workarea.left + workarea.right  - w) / 2;
		y += (workarea.top  + workarea.bottom - h) / 2;
	}

	wchar_t title[128];
	wsprintf(title, L"%s (%ux%u)", params.m_Title, params.m_Width, params.m_Height);


	HWND hwnd = CreateWindowEx(0, L"Humus", title, style, x, y, w, h, HWND_DESKTOP, VK_NULL_HANDLE, inst, VK_NULL_HANDLE);




	VkApplicationInfo app_info = {};
	app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	app_info.pApplicationName = "Vulkan";
	app_info.pEngineName = "Vulkan";
	app_info.apiVersion = VK_API_VERSION_1_0;

	static const char* instance_extensions[] =
	{
		VK_KHR_SURFACE_EXTENSION_NAME,
		VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#ifdef USE_DEBUG_UTILS
		VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
//		VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
#endif
	};

	VkInstanceCreateInfo instance_create_info = {};
	instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instance_create_info.pNext = VK_NULL_HANDLE;
	instance_create_info.pApplicationInfo = &app_info;
	instance_create_info.enabledExtensionCount = elementsof(instance_extensions);
	instance_create_info.ppEnabledExtensionNames = instance_extensions;
	
#if defined(USE_DEBUG_UTILS) && defined(USE_DEBUG_MARKERS)
	// Enable validation layer
	static const char* validation_layers[] = { VALIDATION_LAYER_NAME , "VK_LAYER_RENDERDOC_Capture" };
	instance_create_info.enabledLayerCount = 2;
	instance_create_info.ppEnabledLayerNames = validation_layers;
#elif defined (USE_DEBUG_UTILS)
	static const char* validation_layers[] = { VALIDATION_LAYER_NAME };
	instance_create_info.enabledLayerCount = 1;
	instance_create_info.ppEnabledLayerNames = validation_layers;
#elif defined(USE_DEBUG_MARKERS)
	static const char* validation_layers[] = { "VK_LAYER_RENDERDOC_Capture" };
	instance_create_info.enabledLayerCount = 1;
	instance_create_info.ppEnabledLayerNames = validation_layers;
#endif

	VkInstance instance = VK_NULL_HANDLE;
	VkResult res = vkCreateInstance(&instance_create_info, VK_NULL_HANDLE, &instance);
	ASSERT(res == VK_SUCCESS);

	VkPhysicalDevice physical_device = ChoosePhysicalDevice(instance);

	// Enumerate extensions
	uint32_t count = 0;
	res = vkEnumerateDeviceExtensionProperties(physical_device, VK_NULL_HANDLE, &count, VK_NULL_HANDLE);
	ASSERT(res == VK_SUCCESS);
	Array<VkExtensionProperties> extensions(count, count);
	res = vkEnumerateDeviceExtensionProperties(physical_device, VK_NULL_HANDLE, &count, extensions.GetArray());
	ASSERT(res == VK_SUCCESS);


	VkPhysicalDeviceFeatures features;
	vkGetPhysicalDeviceFeatures(physical_device, &features);

	VkPhysicalDeviceMemoryProperties memory_properties;
	vkGetPhysicalDeviceMemoryProperties(physical_device, &memory_properties);

	uint32_t queue_family_count = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, VK_NULL_HANDLE);
	ASSERT(queue_family_count > 0);

	VkQueueFamilyProperties* queue_family_properties = StackAlloc<VkQueueFamilyProperties>(queue_family_count);
	vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, queue_family_properties);

	// Find a queue with graphics and compute
	int graphics_queue_index = -1;
	for (uint32_t i = 0; i < queue_family_count; i++)
	{
		if (queue_family_properties[i].queueFlags & (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT))
		{
			graphics_queue_index = i;
			break;
		}
	}
	ASSERT(graphics_queue_index >= 0);




	const float default_queue_priority = 0.0f;

	VkDeviceQueueCreateInfo queueInfo = {};
	queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueInfo.queueFamilyIndex = graphics_queue_index;
	queueInfo.queueCount = 1;
	queueInfo.pQueuePriorities = &default_queue_priority;


	Array<const char*> device_extensions;
	device_extensions.Add(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

#ifdef USE_DEBUG_MARKERS
	device_extensions.Add(VK_EXT_DEBUG_MARKER_EXTENSION_NAME);

	vkCmdDebugMarkerBegin = (PFN_vkCmdDebugMarkerBeginEXT) vkGetInstanceProcAddr(instance, "vkCmdDebugMarkerBeginEXT");
	vkCmdDebugMarkerEnd   = (PFN_vkCmdDebugMarkerEndEXT)   vkGetInstanceProcAddr(instance, "vkCmdDebugMarkerEndEXT");
#endif
#ifdef USE_DEBUG_UTILS
    m_fnCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");;
	m_fnDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");;

	if (m_fnCreateDebugUtilsMessengerEXT) {
		VkDebugUtilsMessengerCreateInfoEXT const debugUtilsMessangerCreateInfo = {
			VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT, nullptr, 0,
			// VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
			// VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
			VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
			&VulkanCallback,
			nullptr
		};
		VkResult hResult = m_fnCreateDebugUtilsMessengerEXT(instance, &debugUtilsMessangerCreateInfo, nullptr, &debugMessenger);
	}
#endif
	VkPhysicalDeviceFeatures2 enabled_features = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2 };
	enabled_features.features.samplerAnisotropy = VK_TRUE;
	enabled_features.features.textureCompressionBC = VK_TRUE;
	enabled_features.features.shaderClipDistance = VK_TRUE;
	//enabled_features.features.shaderInt64 = features.shaderInt64;

	// Used to chain up features to pass to vkCreateDevice()
	void** next_feature = &enabled_features.pNext;

	VkPhysicalDeviceProperties2 properties2 = {};
	properties2.pNext = VK_NULL_HANDLE;
	properties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;

	VkPhysicalDeviceProperties properties;
	// Set up requested extensions
	VkPhysicalDeviceMeshShaderFeaturesNV features_mesh_shader = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_NV };
	VkPhysicalDeviceMeshShaderPropertiesNV mesh_shader_properties = {};
	if ((params.m_MeshShaders == Require) || (params.m_MeshShaders && IsExtensionSupported(extensions, VK_NV_MESH_SHADER_EXTENSION_NAME)))
	{
		device_extensions.Add(VK_NV_MESH_SHADER_EXTENSION_NAME);

		features_mesh_shader.taskShader = true;
		features_mesh_shader.meshShader = true;
		*next_feature = &features_mesh_shader;
		next_feature = &features_mesh_shader.pNext;

		properties2.pNext = &mesh_shader_properties;

		mesh_shader_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_PROPERTIES_NV;
		mesh_shader_properties.pNext = VK_NULL_HANDLE;

		vkCmdDrawMeshTasks = (PFN_vkCmdDrawMeshTasksNV) vkGetInstanceProcAddr(instance, "vkCmdDrawMeshTasksNV");
	}
	else
	{
		params.m_MeshShaders = Disable;
	}

	VkPhysicalDevice8BitStorageFeaturesKHR features_8bit_storage = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_8BIT_STORAGE_FEATURES_KHR };
	if ((params.m_8bitStorage == Require) || (params.m_8bitStorage && IsExtensionSupported(extensions, VK_KHR_8BIT_STORAGE_EXTENSION_NAME)))
	{
		device_extensions.Add(VK_KHR_8BIT_STORAGE_EXTENSION_NAME);

		features_8bit_storage.storageBuffer8BitAccess = true;

		*next_feature = &features_8bit_storage;
		next_feature = &features_8bit_storage.pNext;
	}
	else
	{
		params.m_8bitStorage = Disable;
	}


	VkDeviceCreateInfo device_create_info = {};
	device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	device_create_info.pNext = &enabled_features;
	device_create_info.queueCreateInfoCount = 1;
	device_create_info.pQueueCreateInfos = &queueInfo;
	device_create_info.enabledExtensionCount = device_extensions.GetCount();
	device_create_info.ppEnabledExtensionNames = device_extensions.GetArray();

	VkDevice vk_device = VK_NULL_HANDLE;
	res = vkCreateDevice(physical_device, &device_create_info, VK_NULL_HANDLE, &vk_device);
	if (res != VK_SUCCESS)
	{
		char error[2048];
		int off = sprintf_s(error, "Error creating Vulkan Device");

		if (res == VK_ERROR_EXTENSION_NOT_PRESENT)
		{
			off += sprintf_s(error + off, sizeof(error) - off, ".\nThe following required extensions are not supported by your GPU or driver:\n\n");
			for (uint i = 0; i < device_extensions.GetCount(); i++)
			{
				if (!IsExtensionSupported(extensions, device_extensions[i]))
					off += sprintf_s(error + off, sizeof(error) - off, "%s\n", device_extensions[i]);
			}
		}
		ErrorMsg(error);

		return VK_NULL_HANDLE;
	}

	// Get a graphics queue from the device
	VkQueue graphics_queue = VK_NULL_HANDLE;
	vkGetDeviceQueue(vk_device, graphics_queue_index, 0, &graphics_queue);
	ASSERT(graphics_queue);


	// Create the os-specific surface
	VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {};
	surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	surfaceCreateInfo.hinstance = inst;
	surfaceCreateInfo.hwnd = hwnd;

	VkSurfaceKHR surface = VK_NULL_HANDLE;
	res = vkCreateWin32SurfaceKHR(instance, &surfaceCreateInfo, VK_NULL_HANDLE, &surface);
	ASSERT(res == VK_SUCCESS);

	VkBool32 supports_present = false;
	vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, graphics_queue_index, surface, &supports_present);
	ASSERT(supports_present);

	VkSurfaceFormatKHR image_format = GetSwapchainFormat(physical_device, surface);

	vkGetPhysicalDeviceProperties(physical_device, &properties);

	/*if (VK_VERSION_MINOR(properties.apiVersion) > 0)
	{
		vkGetPhysicalDeviceProperties2(physical_device, &properties2);
		properties = properties2.properties;
	}*/

	// TODO: Allocate later ...
	Device device = new SDevice;
	memset(device, 0, sizeof(SDevice));
	device->m_VkDevice = vk_device;
	device->m_VkGraphicsQueue = graphics_queue;
	device->m_VkGraphicsQueueIndex = graphics_queue_index;
	device->m_Window = hwnd;
	device->m_VkPhysicalDevice = physical_device;
	device->m_TimestampFrequency = properties.limits.timestampPeriod * 1e-6f;
	device->m_UniformBufferAlignment = (uint) properties.limits.minUniformBufferOffsetAlignment;
	device->m_MaxDrawMeshTasksCount = mesh_shader_properties.maxDrawMeshTasksCount;
	device->m_MSAASupportMask = properties.limits.framebufferColorSampleCounts & properties.limits.framebufferDepthSampleCounts;
	device->m_VkMemoryProperties = memory_properties;
	device->m_VkSurface = surface;
	device->m_VkInstance = instance;
	device->m_RenderpassDictionary = new RenderPassDictionary;
	device->m_SamplerDictionary = new TSamplerDictionary;

	// Create swapchain
	VkSwapchainCreateInfoKHR& swapchain_create_info = device->m_VkSwapChainCreateInfo;
	swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchain_create_info.pNext = VK_NULL_HANDLE;
	swapchain_create_info.flags = 0;
	swapchain_create_info.surface = surface;
	swapchain_create_info.minImageCount = 2;
	swapchain_create_info.imageFormat     = image_format.format;
	swapchain_create_info.imageColorSpace = image_format.colorSpace;
	swapchain_create_info.imageExtent.width  = params.m_Width;
	swapchain_create_info.imageExtent.height = params.m_Height;
	swapchain_create_info.imageArrayLayers = 1;
	swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	swapchain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	swapchain_create_info.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	swapchain_create_info.queueFamilyIndexCount = 0;
	swapchain_create_info.pQueueFamilyIndices = VK_NULL_HANDLE;
	swapchain_create_info.oldSwapchain = VK_NULL_HANDLE;
	swapchain_create_info.clipped = VK_TRUE;
	swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

	CreateSwapchain(device, params.m_VSync);


	// Semaphores
	VkSemaphoreCreateInfo semaphore_create_info = {};
	semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	semaphore_create_info.pNext = VK_NULL_HANDLE;
	res = vkCreateSemaphore(vk_device, &semaphore_create_info, VK_NULL_HANDLE, &device->m_PresentSemaphore);
	ASSERT(res == VK_SUCCESS);


	VkCommandPoolCreateInfo cmd_pool_create_info = {};
	cmd_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	cmd_pool_create_info.queueFamilyIndex = graphics_queue_index;
	cmd_pool_create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	res = vkCreateCommandPool(vk_device, &cmd_pool_create_info, VK_NULL_HANDLE, &device->m_VkCommandPool);
	ASSERT(res == VK_SUCCESS);

	
	
	// Enumerate display modes
	int m = 0;
	DEVMODEW dm;
	dm.dmSize = sizeof(dm);
	Array<DisplayMode> modes;
	while (EnumDisplaySettings(VK_NULL_HANDLE, m, &dm))
	{
		// Remove duplicates, we only save resolution
		int n = modes.GetCount() - 1;
		while (n >= 0)
		{
			if ((DWORD) modes[n].m_Width == dm.dmPelsWidth && (DWORD) modes[n].m_Height == dm.dmPelsHeight)
				break;
			--n;
		}

		if (n < 0)
		{
			DisplayMode mode;
			mode.m_Width  = dm.dmPelsWidth;
			mode.m_Height = dm.dmPelsHeight;
			modes.Add(mode);
		}

		++m;
	}
	device->m_DisplayModes.SetCapacity(modes.GetCount());
	memcpy(device->m_DisplayModes.GetArray(), modes.GetArray(), modes.GetCount() * sizeof(DisplayMode));
	
	//permanent descriptor pool
	VkDescriptorPoolSize pool_sizes[] =
	{
		{ VK_DESCRIPTOR_TYPE_SAMPLER, 16 },
		{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 256 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 64 },
	};
	VkDescriptorPoolCreateInfo desc_pool_create_info = {};
	desc_pool_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	desc_pool_create_info.poolSizeCount = elementsof(pool_sizes);
	desc_pool_create_info.pPoolSizes = pool_sizes;
	desc_pool_create_info.maxSets = 64;
	desc_pool_create_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	res = vkCreateDescriptorPool(vk_device, &desc_pool_create_info, VK_NULL_HANDLE, &device->m_VkDescriptorPool);

	ASSERT(res == VK_SUCCESS);
	for (SCommandList& cmdList : device->m_CommandBuffers)
	{
		cmdList.Init(device);
	}

	ImageFormat format = (image_format.format == VK_FORMAT_B8G8R8A8_UNORM) ? IMGFMT_BGRA8 : IMGFMT_RGBA8;
	device->m_BackBufferRenderPass = CreateRenderPass(device, format, IMGFMT_INVALID, FLAG_NONE);
	
	if (!CreateBackBufferSetups(device, params.m_Width, params.m_Height, format))
		return nullptr;

	device->m_MainContext = CreateContext(device, false);

	device->m_DefaultBlendState = CreateBlendState(device, BF_ONE, BF_ZERO, BM_ADD, 0xF, false);


	return device;
}
void DestroyRenderPass(Device device, RenderPass& render_pass);

void DestroyDevice(Device& device)
{
	if (!device)
		return;

	Finish(device);
#ifdef USE_DEBUG_UTILS
	if (m_fnDestroyDebugUtilsMessengerEXT && VK_NULL_HANDLE != debugMessenger) {
		m_fnDestroyDebugUtilsMessengerEXT(device->m_VkInstance, debugMessenger, nullptr);
	}
#endif
	DestroyBlendState(device, device->m_DefaultBlendState);

	vkDestroyDescriptorPool(device->m_VkDevice, device->m_VkDescriptorPool, VK_NULL_HANDLE);

	DestroyBackBufferSetups(device);

	DestroyContext(device, device->m_MainContext);

	vkDestroySemaphore(device->m_VkDevice, device->m_PresentSemaphore, VK_NULL_HANDLE);
	
	for (SCommandList& cmdList : device->m_CommandBuffers)
		cmdList.Destroy(device);

	vkDestroyCommandPool(device->m_VkDevice, device->m_VkCommandPool, VK_NULL_HANDLE);

	vkDestroySwapchainKHR(device->m_VkDevice, device->m_VkSwapChain, VK_NULL_HANDLE);
	vkDestroySurfaceKHR(device->m_VkInstance, device->m_VkSurface, VK_NULL_HANDLE);
	for (auto& pass : *device->m_RenderpassDictionary)
	{
		DestroyRenderPass(device, pass.second);
	}


	vkDestroyDevice(device->m_VkDevice, VK_NULL_HANDLE);
	vkDestroyInstance(device->m_VkInstance, VK_NULL_HANDLE);

	delete device->m_RenderpassDictionary;
	delete device->m_SamplerDictionary;
	delete device;
	device = VK_NULL_HANDLE;
}

void Present(Device device, bool vsync)
{
	VkPresentInfoKHR present_info = {};
	present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	present_info.pNext = VK_NULL_HANDLE;
	present_info.swapchainCount = 1;
	present_info.pSwapchains = &device->m_VkSwapChain;
	present_info.pImageIndices = &device->m_VkSwapChainImageId;
	VkResult res = vkQueuePresentKHR(device->m_VkGraphicsQueue, &present_info);
	ASSERT(res == VK_SUCCESS);
}

HWND GetWindow(Device device)
{
	return device->m_Window;
}

Texture GetBackBuffer(Device device, uint buffer)
{
	return device->m_BackBuffer[buffer];
}

ImageFormat GetBackBufferFormat(Device device)
{
	return device->m_BackBuffer[0]->m_Format;
}

RenderPass GetBackBufferRenderPass(Device device)
{
	return device->m_BackBufferRenderPass;
}

Context GetMainContext(Device device)
{
	return device->m_MainContext;
}

uint GetBackBufferIndex(Device device)
{
	if (!device->m_WasAcquired)
	{
		device->m_WasAcquired = true;

		// TODO: Evaluate if this really belongs here ...

		VkResult res = vkAcquireNextImageKHR(device->m_VkDevice, device->m_VkSwapChain, UINT64_MAX, device->m_PresentSemaphore, VK_NULL_HANDLE, &device->m_VkSwapChainImageId);
		ASSERT(res == VK_SUCCESS);
	}

	return device->m_VkSwapChainImageId;
}

uint GetDisplayModeCount(Device device)
{
	return device->m_DisplayModes.GetCount();
}

const DisplayMode& GetDisplayMode(Device device, uint index)
{
	return device->m_DisplayModes[index];
}

void SetDisplayMode(Device device, DeviceParams& params)
{
	params.m_ModeChangeInProgress = true;

	DWORD style = params.m_Fullscreen? WS_POPUP : WS_OVERLAPPEDWINDOW;

	int x, y, w, h;

	if (params.m_Fullscreen)
	{
		DEVMODEW dm = {};
		dm.dmSize = sizeof(dm);
		dm.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;
		dm.dmPelsWidth  = params.m_FullscreenWidth;
		dm.dmPelsHeight = params.m_FullscreenHeight;
		ChangeDisplaySettingsW(&dm, CDS_FULLSCREEN);

		DebugString("SetDisplayMode(%d, %d, 1);", params.m_FullscreenWidth, params.m_FullscreenHeight);

		x = 0;
		y = 0;
		w = params.m_FullscreenWidth;
		h = params.m_FullscreenHeight;
	}
	else
	{
		ChangeDisplaySettingsW(VK_NULL_HANDLE, 0);

		DebugString("SetDisplayMode(%d, %d, 0);", params.m_WindowedWidth, params.m_WindowedHeight);

		RECT rect;
		rect.left = 0;
		rect.top  = 0;
		rect.right  = params.m_WindowedWidth;
		rect.bottom = params.m_WindowedHeight;
		AdjustWindowRectEx(&rect, style, FALSE, 0);

		x = rect.left;
		y = rect.top;
		w = rect.right - rect.left;
		h = rect.bottom - rect.top;

		RECT workarea;
		SystemParametersInfo(SPI_GETWORKAREA, 0, &workarea, 0);

		x += (workarea.left + workarea.right  - w) / 2;
		y += (workarea.top  + workarea.bottom - h) / 2;
	}

	SetWindowLongPtr(device->m_Window, GWL_STYLE, style);
	SetWindowPos(device->m_Window, VK_NULL_HANDLE, x, y, w, h, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOCOPYBITS | SWP_FRAMECHANGED | SWP_SHOWWINDOW);


	if (!params.m_Fullscreen)
	{
		w = params.m_WindowedWidth;
		h = params.m_WindowedHeight;
	}

	params.m_ModeChangeInProgress = false;
	Resize(device, params, w, h);
}

// This function is called in response to WM_SIZE messages, including synchronously during mode switch
bool Resize(Device device, DeviceParams& params, int width, int height)
{
	if (params.m_ModeChangeInProgress)
		return true;

	ImageFormat format = device->m_BackBuffer[0]->m_Format;
	DestroyBackBufferSetups(device);

	DebugString("Resize(%d, %d);", width, height);

	device->m_VkSwapChainCreateInfo.imageExtent.width  = width;
	device->m_VkSwapChainCreateInfo.imageExtent.height = height;
	CreateSwapchain(device, params.m_VSync);

	UpdateWindow(device, params, width, height);

	CreateBackBufferSetups(device, width, height, format);

	return true;
}

bool SupportsMSAAMode(Device device, uint samples)
{
	if (IsPowerOf2(samples))
		return (device->m_MSAASupportMask & samples) != 0;
	return false;
}


BlendState GetDefaultBlendState(Device device)
{
	return device->m_DefaultBlendState;
}


Context CreateContext(Device device, bool deferred)
{
	Context context = new SContext();
	memset(context, 0, sizeof(SContext));

	context->m_Device = device;

	return context;
}

void DestroyContext(Device device, Context& context)
{
	if (!context)
		return;

	delete context;
	context = VK_NULL_HANDLE;
}

Device GetDevice(Context context)
{
	return context->m_Device;
}

Texture CreateTexture(Device device, const STextureParams& params)
{
	VkImage image = VK_NULL_HANDLE;
	uint ifCube6 = (params.m_Type == TEX_CUBE || params.m_Type == TEX_CUBE_ARRAY) ? 6 : 1;
	VkImageCreateInfo image_create_info = {};
	image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	image_create_info.imageType = (params.m_Type <= TEX_1D_ARRAY)? VK_IMAGE_TYPE_1D : (params.m_Type <= TEX_CUBE_ARRAY)? VK_IMAGE_TYPE_2D : VK_IMAGE_TYPE_3D;
	image_create_info.format = g_Formats[params.m_Format];
	image_create_info.extent = { params.m_Width, params.m_Height, params.m_Depth };
	image_create_info.mipLevels = params.m_MipCount;
	image_create_info.arrayLayers = ifCube6*params.m_Slices;
	image_create_info.samples = VkSampleCountFlagBits(params.m_MSAASampleCount);
	image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
	//image_create_info.usage = (IsDepthFormat(params.m_Format)? VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT : VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT) | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;

	if (params.m_Type == TEX_CUBE || params.m_Type == TEX_CUBE_ARRAY)
		image_create_info.flags |= VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
	/*else if (params.m_Type == TEX_2D_ARRAY)
		image_create_info.flags |= VK_IMAGE_CREATE_2D_ARRAY_COMPATIBLE_BIT;
    */// investigate! seems to be used only on volume(3d) textures
	if (params.m_ShaderResource)
		image_create_info.usage |= VK_IMAGE_USAGE_SAMPLED_BIT;
	if (params.m_RenderTarget)
		image_create_info.usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	if (params.m_DepthTarget)
		image_create_info.usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	if (params.m_UnorderedAccess)
		image_create_info.usage |= VK_IMAGE_USAGE_STORAGE_BIT;

	if (!params.m_RenderTarget && !params.m_DepthTarget)
		image_create_info.usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;

	image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	VkResult res = vkCreateImage(device->m_VkDevice, &image_create_info, VK_NULL_HANDLE, &image);
	ASSERT(res == VK_SUCCESS);

	VkMemoryRequirements mem_reqs;
	vkGetImageMemoryRequirements(device->m_VkDevice, image, &mem_reqs);

	// Allocate memory for the image (device local) and bind it to our image
	VkMemoryAllocateInfo mem_alloc_info = {};
	mem_alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	mem_alloc_info.allocationSize = mem_reqs.size;
	mem_alloc_info.memoryTypeIndex = GetMemoryTypeIndex(device->m_VkMemoryProperties, mem_reqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	VkDeviceMemory memory = VK_NULL_HANDLE;
	res = vkAllocateMemory(device->m_VkDevice, &mem_alloc_info, VK_NULL_HANDLE, &memory);
	ASSERT(res == VK_SUCCESS);

	res = vkBindImageMemory(device->m_VkDevice, image, memory, 0);
	ASSERT(res == VK_SUCCESS);

	Texture texture = new STexture();
	memset(texture, 0, sizeof(STexture));
	texture->m_Image     = image;
	texture->m_Width     = params.m_Width;
	texture->m_Height    = params.m_Height;
	texture->m_Depth     = params.m_Depth;
	texture->m_Slices    = params.m_Slices;
	texture->m_MipLevels = params.m_MipCount;
	texture->m_Type      = params.m_Type;
	texture->m_Format    = params.m_Format;
	texture->m_Memory    = memory;

	return texture;
}
void DestroyTextureSubresource(Device device, STextureSubresource* sub)
{
	if (sub->m_Views)
	{
		for (auto& view : *sub->m_Views)
		{
			vkDestroyImageView(device->m_VkDevice, view.second, VK_NULL_HANDLE);
		}
		delete sub->m_Views;
	}
	if (sub->m_Faces)
	{
		for (int face = 0; face < 0; face++)
		{
			if (sub->m_Faces[face])
			{
				DestroyTextureSubresource(device, sub->m_Faces[face]);
			}
		}
	}
}
void DestroyTexture(Device device, Texture& texture)
{
	if (texture)
	{
		vkFreeMemory(device->m_VkDevice, texture->m_Memory, VK_NULL_HANDLE);
		if (texture->m_SubresourceDictionary)
		{
			for (auto& it : *texture->m_SubresourceDictionary)
			{
				DestroyTextureSubresource(device, it.second);
			}
		}
		vkDestroyImage(device->m_VkDevice, texture->m_Image, VK_NULL_HANDLE);
		delete texture->m_SubresourceDictionary;
		delete texture;
		texture = VK_NULL_HANDLE;
	}
}
void UpdateResourceTable(Device device, RootSignature root, uint32 slot, ResourceTable table,  const SResourceDesc* resources, uint offset, uint count, const uint8* reflectionFlags)
{
	const SRootSignature::SRootSlot& root_slot = root->m_Slots[slot];
	VkResult res;
	VkDescriptorSet descriptor_set = table->m_DescriptorSet;
	VkDescriptorImageInfo image_info = {};

	VkDescriptorBufferInfo buffer_info = {};
	buffer_info.range = VK_WHOLE_SIZE;

	VkWriteDescriptorSet descriptor_write = {};
	descriptor_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptor_write.dstSet = descriptor_set;
	descriptor_write.descriptorCount = 1;

	uint binding = offset;
	uint element = 0;

	for (uint i = offset; i < offset + count; i++)
	{
		const SResourceTableItem& item = root_slot.m_Items[binding];

		switch (item.m_Type)
		{
		case TEXTURE:
		case RWTEXTURE:
		{
			ASSERT(resources[i].m_Type == RESTYPE_TEXTURE);
			Texture texture = (Texture)resources[i].m_Resource;
			descriptor_write.descriptorType = (item.m_Type == TEXTURE) ? VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE : VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
			descriptor_write.pImageInfo = &image_info;
			descriptor_write.pBufferInfo = VK_NULL_HANDLE;
			TextureViewFlags viewFlags = ViewDefault;
			if (reflectionFlags && (reflectionFlags[i] & Refl_Flag_Array) && !(reflectionFlags[i] & Refl_Flag_Cube) 
				&& (texture->m_Type == TEX_CUBE || texture->m_Type == TEX_CUBE_ARRAY))
				viewFlags = CubeAsArray;
			image_info.imageView = AcquireTextureSubresourceView(device, resources[i], item.m_Type == TEXTURE ? TexViewUsage::SRV : TexViewUsage::UAV, viewFlags);
			image_info.imageLayout = (item.m_Type == TEXTURE) ? (IsDepthFormat(texture->m_Format)? VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL )
                                                                 : VK_IMAGE_LAYOUT_GENERAL;
			break;
		}
		case STRUCTUREDBUFFER:
		case RWSTRUCTUREDBUFFER:
		{
			ASSERT(resources[i].m_Type == RESTYPE_BUFFER);

			Buffer buffer = (Buffer)resources[i].m_Resource;

			descriptor_write.descriptorType = (item.m_Type == STRUCTUREDBUFFER) ? VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER : VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			descriptor_write.pImageInfo = VK_NULL_HANDLE;
			descriptor_write.pBufferInfo = &buffer_info;

			buffer_info.buffer = buffer->m_Buffer;
            buffer_info.offset = resources[i].m_bufRange.offset;
            buffer_info.range = resources[i].m_bufRange.size == 0 ? VK_WHOLE_SIZE : resources[i].m_bufRange.size;
			break;
		}
		case CBV:
		{
			ASSERT(resources[i].m_Type == RESTYPE_BUFFER);

			Buffer buffer = (Buffer)resources[i].m_Resource;

			descriptor_write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptor_write.pImageInfo = VK_NULL_HANDLE;
			descriptor_write.pBufferInfo = &buffer_info;

			buffer_info.buffer = buffer->m_Buffer;
			buffer_info.offset = resources[i].m_bufRange.offset;
			buffer_info.range = resources[i].m_bufRange.size == 0? VK_WHOLE_SIZE: resources[i].m_bufRange.size;
			break;
		}
		default:
			ASSERT(false);
		}

		descriptor_write.dstBinding = binding;
		descriptor_write.dstArrayElement = element;

		vkUpdateDescriptorSets(device->m_VkDevice, 1, &descriptor_write, 0, VK_NULL_HANDLE);

		++element;
		if (element >= root_slot.m_Items[binding].m_NumElements)
		{
			binding++;
			element = 0;
		}
	}

	// If you hit this assert, you have a mismatch between declared resource table in the .pipeline file and the provided resources to this call
//	ASSERT(binding == root_slot.m_Size && element == 0);
}

ResourceTable CreateResourceTable(Device device, RootSignature root, uint32 slot, const SResourceDesc* resources, uint count, Context onframe)
{
	ASSERT(slot < root->m_SlotCount);

	const SRootSignature::SRootSlot& root_slot = root->m_Slots[slot];

	VkDescriptorSet descriptor_set = VK_NULL_HANDLE;

	VkDescriptorSetAllocateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	info.descriptorPool = onframe? onframe->m_CmdList->m_VkDescriptorPool:device->m_VkDescriptorPool;
	info.descriptorSetCount = 1;
	info.pSetLayouts = &root->m_Slots[slot].m_DescriptorSetLayout;

	VkResult res = vkAllocateDescriptorSets(device->m_VkDevice, &info, &descriptor_set);
	ASSERT(res == VK_SUCCESS);
	ResourceTable rt = new SResourceTable();
	rt->m_DescriptorSet = descriptor_set;	
	rt->m_Pool = info.descriptorPool;

	if (resources)
	{
		UpdateResourceTable(device, root, slot, rt, resources, 0, count);
	}
	return rt;
}

void DestroyResourceTable(Device device, ResourceTable& table)
{
	if (table)
	{
		if (table->m_Pool != device->m_VkDescriptorPool)
		{
			delete table;
			table = VK_NULL_HANDLE;
			return;
		}
		vkFreeDescriptorSets(device->m_VkDevice, device->m_VkDescriptorPool, 1, &table->m_DescriptorSet);
		delete table;
		table = VK_NULL_HANDLE;
	}
}

SamplerTable CreateSamplerTable(Device device, RootSignature root, uint32 slot, const SSamplerDesc* sampler_descs, uint count, Context onframe)
{
	ASSERT(slot < root->m_SlotCount);
	ASSERT(count == root->m_Slots[slot].m_Size);

	VkDescriptorSet descriptor_set = VK_NULL_HANDLE;

	VkDescriptorSetAllocateInfo alloc_info = {};
	alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	alloc_info.descriptorPool = onframe ? onframe->m_CmdList->m_VkDescriptorPool : device->m_VkDescriptorPool;
	alloc_info.descriptorSetCount = 1;
	alloc_info.pSetLayouts = &root->m_Slots[slot].m_DescriptorSetLayout;

	VkResult res = vkAllocateDescriptorSets(device->m_VkDevice, &alloc_info, &descriptor_set);
	ASSERT(res == VK_SUCCESS);

	// Allocate variable size depending on count
	size_t size = offsetof(SSamplerTable, m_Samplers) + count * sizeof(VkSampler) + sizeof(VkDescriptorPool);
	void* mem = new ubyte[size];
	memset(mem, 0, size);

	SamplerTable table = new(mem) SSamplerTable();
	table->m_DescriptorSet = descriptor_set;
	table->m_Count  = count;
	table->m_Pool = alloc_info.descriptorPool;

	VkDescriptorImageInfo* image_infos = (VkDescriptorImageInfo*) alloca(count * sizeof(VkDescriptorImageInfo));
	memset(image_infos, 0, count * sizeof(VkDescriptorImageInfo));

	VkSamplerCreateInfo sampler_info = {};
	sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;

	for (uint i = 0; i < count; i++)
	{
		const SSamplerDesc& sampler_desc = sampler_descs[i];

		VkFilter filter = (sampler_desc.Filter == FILTER_POINT || sampler_desc.Filter == FILTER_POINT_MIP)? VK_FILTER_NEAREST : VK_FILTER_LINEAR;
		sampler_info.magFilter = filter;
		sampler_info.minFilter = filter;
		sampler_info.mipmapMode = (sampler_desc.Filter == FILTER_TRILINEAR)? VK_SAMPLER_MIPMAP_MODE_LINEAR : VK_SAMPLER_MIPMAP_MODE_NEAREST;
		sampler_info.addressModeU = (sampler_desc.AddressModeU == AM_WRAP)? VK_SAMPLER_ADDRESS_MODE_REPEAT : VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		sampler_info.addressModeV = (sampler_desc.AddressModeV == AM_WRAP)? VK_SAMPLER_ADDRESS_MODE_REPEAT : VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		sampler_info.addressModeW = (sampler_desc.AddressModeW == AM_WRAP)? VK_SAMPLER_ADDRESS_MODE_REPEAT : VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		sampler_info.anisotropyEnable = (sampler_desc.Aniso > 1);
		sampler_info.maxAnisotropy = (float) sampler_desc.Aniso;
		sampler_info.maxLod = (sampler_desc.Filter > FILTER_LINEAR)? FLT_MAX : 0.0f;
		sampler_info.compareEnable = sampler_desc.Comparison != EComparisonFunc::ALWAYS;
		sampler_info.compareOp = (VkCompareOp)sampler_desc.Comparison;
		VkSampler sampler = VK_NULL_HANDLE;
		res = vkCreateSampler(device->m_VkDevice, &sampler_info, VK_NULL_HANDLE, &sampler);
		ASSERT(res == VK_SUCCESS);

		table->m_Samplers[i] = sampler;
		image_infos[i].sampler = sampler;
	}

	VkWriteDescriptorSet descriptor_write = {};
	descriptor_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptor_write.dstSet = descriptor_set;
	descriptor_write.dstBinding = 0;
	descriptor_write.dstArrayElement = 0;
	descriptor_write.descriptorCount = count;
	descriptor_write.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
	descriptor_write.pImageInfo = image_infos;

	vkUpdateDescriptorSets(device->m_VkDevice, 1, &descriptor_write, 0, VK_NULL_HANDLE);

	return table;
}


void DestroySamplerTable(Device device, SamplerTable& table)
{
	if (table)
	{
		if (table->m_Pool != device->m_VkDescriptorPool)
			return;

		for (uint i = 0; i < table->m_Count; i++)
		{
			vkDestroySampler(device->m_VkDevice, table->m_Samplers[i], VK_NULL_HANDLE);
		}
		vkFreeDescriptorSets(device->m_VkDevice, device->m_VkDescriptorPool, 1, &table->m_DescriptorSet);

		delete[] table;
		table = VK_NULL_HANDLE;
	}
}

union SRenderPassDescKey
{
	SRenderPassDesc desc;
	uint64 combo;
};

RenderPass CreateRenderPass(Device device, ImageFormat color_format, ImageFormat depth_format, RenderPassFlags flags, uint msaa_samples)
{
	SRenderPassDesc desc; memset(&desc, 0, sizeof(SRenderPassDesc));
	desc.m_ColorFormats[0] = color_format;
	desc.m_DepthFormat = depth_format;
	desc.m_Flags = flags;
	desc.msaa_samples = 1;
	return AcquireRenderPass(device, desc);
}

RenderPass AcquireRenderPass(Device device, const SRenderPassDesc& desc)
{
	SRenderPassDescKey key;
	key.desc = desc;
	auto it = device->m_RenderpassDictionary->find(key.combo);
	if (it != device->m_RenderpassDictionary->end())
	{
		return it->second;
	}
	VkAttachmentDescription attachments[7] = {};
	uint attachment_count = 0;

	VkAttachmentReference color_references[5];
	VkAttachmentReference depth_reference, resolve_reference;
	uint colorRTCount = 0;
	for (auto color_format : desc.m_ColorFormats)
	{
		if (color_format)
		{
			color_references[colorRTCount] = { attachment_count, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };

			VkAttachmentDescription& attachment = attachments[attachment_count++];
			attachment.format = g_Formats[color_format];
			attachment.samples = VkSampleCountFlagBits(desc.msaa_samples);
			attachment.loadOp = (desc.m_Flags & CLEAR_COLOR) ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
			attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			attachment.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			attachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			colorRTCount++;
		}
	}
	if (desc.m_DepthFormat)
	{
		depth_reference = { attachment_count, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL };

		VkAttachmentDescription& attachment = attachments[attachment_count++];
		attachment.format = g_Formats[desc.m_DepthFormat];
		attachment.samples = VkSampleCountFlagBits(desc.msaa_samples);
		attachment.loadOp = (desc.m_Flags & CLEAR_DEPTH)? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
		attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachment.initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	}

	if (desc.msaa_samples > 1)
	{
		resolve_reference = { attachment_count, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };

		VkAttachmentDescription& attachment = attachments[attachment_count++];
		attachment.format = g_Formats[desc.m_ColorFormats[0]];
		attachment.samples = VK_SAMPLE_COUNT_1_BIT;
		attachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attachment.finalLayout = VK_IMAGE_LAYOUT_GENERAL;
	}

	VkSubpassDescription subpass_desc = {};
	subpass_desc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass_desc.colorAttachmentCount = colorRTCount;
	subpass_desc.pColorAttachments = colorRTCount ? &color_references[0] : VK_NULL_HANDLE;
	subpass_desc.pDepthStencilAttachment = desc.m_DepthFormat ? &depth_reference : VK_NULL_HANDLE;
	subpass_desc.pResolveAttachments = (desc.msaa_samples > 1)? &resolve_reference : VK_NULL_HANDLE;

	VkSubpassDependency dependencies[2];

	dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
	dependencies[0].dstSubpass = 0;
	dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
	dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	dependencies[1].srcSubpass = 0;
	dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
	dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
	dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;


	VkRenderPassCreateInfo renderpass_create_info = {};
	renderpass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderpass_create_info.attachmentCount = attachment_count;
	renderpass_create_info.pAttachments = attachments;
	renderpass_create_info.subpassCount = 1;
	renderpass_create_info.pSubpasses = &subpass_desc;
	renderpass_create_info.dependencyCount = elementsof(dependencies);
	renderpass_create_info.pDependencies = dependencies;

	VkRenderPass vk_render_pass = VK_NULL_HANDLE;
	VkResult res = vkCreateRenderPass(device->m_VkDevice, &renderpass_create_info, VK_NULL_HANDLE, &vk_render_pass);
	ASSERT(res == VK_SUCCESS);


	RenderPass render_pass = new SRenderPass();
	render_pass->m_RenderPass = vk_render_pass;
	render_pass->m_Flags = desc.m_Flags;
	render_pass->m_MSAASamples = desc.msaa_samples;
	device->m_RenderpassDictionary->insert(RenderPassDictionary::value_type(key.combo, render_pass));
	return render_pass;
}

void DestroyRenderPass(Device device, RenderPass& render_pass)
{
	if (render_pass)
	{
		vkDestroyRenderPass(device->m_VkDevice, render_pass->m_RenderPass, VK_NULL_HANDLE);

		delete render_pass;
		render_pass = VK_NULL_HANDLE;
	}
}


RenderSetup CreateRenderSetup(Device device, RenderPass render_pass, Texture* color_targets, uint color_target_count, Texture depth_target, Texture resolve_target, int depth_slice)
{
	SFrameBuffer fb; memset(&fb, 0, sizeof(SFrameBuffer));
	for (int i = 0; i < color_target_count; i++)
	{
		fb.m_ColorTargets[i] = color_targets[i];
	}
	fb.m_DepthTarget = { depth_target, {depth_slice} };
	fb.m_ResolveTarget = resolve_target;
	fb.m_ColorTargetCount = color_target_count;
	return CreateRenderSetup(device, render_pass, fb);
}
RenderSetup CreateRenderSetup(Device device, RenderPass render_pass, SFrameBuffer& fb)
{
	VkImageView attachments[7];
	uint attachment_count = 0;

	for (SResourceDesc& color_target : fb.m_ColorTargets)
	{
		if (color_target.m_Resource)
		{
			VkImageView color_view = AcquireTextureSubresourceView(device, color_target, TexViewUsage::RTV);
			ASSERT(color_view != VK_NULL_HANDLE);

			attachments[attachment_count++] = color_view;
		}
	}
	if (fb.m_DepthTarget.m_Resource)
	{
		VkImageView depth_view = AcquireTextureSubresourceView(device, fb.m_DepthTarget, TexViewUsage::DSV);
		ASSERT(depth_view != VK_NULL_HANDLE);

		attachments[attachment_count++] = depth_view;
	}

	if (fb.m_ResolveTarget.m_Resource)
	{
		VkImageView resolve_view = AcquireTextureSubresourceView(device, fb.m_ResolveTarget, TexViewUsage::RTV);
		ASSERT(resolve_view != VK_NULL_HANDLE);

		attachments[attachment_count++] = resolve_view;
	}

	uint32 width  = fb.m_ColorTargets[0].m_Resource ? ((Texture)fb.m_ColorTargets[0].m_Resource)->m_Width: ((Texture)fb.m_DepthTarget.m_Resource)->m_Width;
	uint32 height = fb.m_ColorTargets[0].m_Resource ? ((Texture)fb.m_ColorTargets[0].m_Resource)->m_Height: ((Texture)fb.m_DepthTarget.m_Resource)->m_Height;


	VkFramebufferCreateInfo framebuffer_create_info = {};
	framebuffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebuffer_create_info.renderPass = render_pass->m_RenderPass;
	framebuffer_create_info.attachmentCount = attachment_count;
	framebuffer_create_info.pAttachments = attachments;
	framebuffer_create_info.width  = width;
	framebuffer_create_info.height = height;
	framebuffer_create_info.layers = 1;

	VkFramebuffer frame_buffer = VK_NULL_HANDLE;
	VkResult res = vkCreateFramebuffer(device->m_VkDevice, &framebuffer_create_info, VK_NULL_HANDLE, &frame_buffer);
	ASSERT(res == VK_SUCCESS);


	RenderSetup setup = new SRenderSetup();
	memset(setup, 0, sizeof(setup));
	setup->m_FrameBuffer = frame_buffer;
	setup->m_Width  = width;
	setup->m_Height = height;
	setup->m_Pass = render_pass;
	memcpy(setup->m_Views, attachments, attachment_count * sizeof(VkImageView));

	return setup;
}

void DestroyRenderSetup(Device device, RenderSetup& setup)
{
	if (setup)
	{
		vkDestroyFramebuffer(device->m_VkDevice, setup->m_FrameBuffer, VK_NULL_HANDLE);

		delete setup;
		setup = VK_NULL_HANDLE;
	}
}

RootSignature CreateRootSignature(Device device, const SCodeBlob& blob)
{
	const SRoot* root = (const SRoot*)blob.m_Code;
	VkDescriptorSetLayoutBinding* bindings = VK_NULL_HANDLE;
	// Create the pipeline layout that is used to generate the rendering pipelines that are based on this descriptor set layout
// In a more complex scenario you would have different pipeline layouts for different descriptor set layouts that could be reused
	VkPipelineLayoutCreateInfo pipeline_layout_create_info = {};
	pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	const SResourceTableItem* resource_table_items = nullptr;
	VkDescriptorSetLayout descriptor_set_layouts[16] = {};

	if (root->m_NumSlots)
	{
		const size_t root_slots_size = sizeof(SRoot) + (root->m_NumSlots - 1) * sizeof(SRootSlot);
		resource_table_items = (const SResourceTableItem*)((char*)root + root_slots_size);

		ASSERT(blob.m_Size >= root_slots_size);

		// Find largest resource/sampler table
		uint32 max_table_size = 0;
		uint32 total_table_descriptors = 0;
		const uint32 num_slots = root->m_NumSlots;
		for (uint32 i = 0; i < num_slots; i++)
		{
			const SRootSlot& slot = root->m_Slots[i];
			if (slot.m_Type == RESOURCE_TABLE || slot.m_Type == SAMPLER_TABLE)
			{
				max_table_size = max(max_table_size, slot.m_Size);
				for (uint32 d = 0; d < slot.m_Size; d++)
				{
					total_table_descriptors += resource_table_items[d].m_NumElements;
				}
				resource_table_items += slot.m_Size;
			}
		}

		ASSERT(blob.m_Size == size_t((char*)resource_table_items - (char*)root));
		// Reset pointer for reiteration
		resource_table_items = (const SResourceTableItem*)((char*)root + root_slots_size);

		VkPushConstantRange range;
		range.stageFlags = VK_SHADER_STAGE_ALL;
		range.offset = 0;
		range.size = 0;

		if (max_table_size)
		{
			size_t size = max_table_size * sizeof(VkDescriptorSetLayoutBinding);
			bindings = (VkDescriptorSetLayoutBinding*)alloca(size);
			memset(bindings, 0, size);
		}

		static const VkDescriptorType descriptor_types[] =
		{
			VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, // ROOT_CONST
			VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
			VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
			VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			VK_DESCRIPTOR_TYPE_SAMPLER,
		};

		uint32 num_descriptor_sets = 0;
		for (uint32 i = 0; i < num_slots; i++)
		{
			const SRootSlot& slot = root->m_Slots[i];

			switch (slot.m_Type)
			{
			case CONSTANT:
				range.size += slot.m_Size * sizeof(uint32);
				break;
			case CBV:
			{
				VkDescriptorSetLayoutBinding ubo_binding = {};
				ubo_binding.binding = 0;
				ubo_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
				ubo_binding.descriptorCount = 1;
				ubo_binding.stageFlags = VK_SHADER_STAGE_ALL;//VK_SHADER_STAGE_ALL_GRAPHICS;

				VkDescriptorSetLayoutCreateInfo layout_info = {};
				layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
				layout_info.bindingCount = 1;
				layout_info.pBindings = &ubo_binding;

				VkResult res = vkCreateDescriptorSetLayout(device->m_VkDevice, &layout_info, VK_NULL_HANDLE, &descriptor_set_layouts[num_descriptor_sets]);
				ASSERT(res == VK_SUCCESS);

				++num_descriptor_sets;
				break;
			}
			case RESOURCE_TABLE:
			case SAMPLER_TABLE:
			{
				for (uint j = 0; j < slot.m_Size; j++)
				{
					bindings[j].binding = j;
					bindings[j].descriptorType = descriptor_types[resource_table_items[j].m_Type];
					bindings[j].descriptorCount = resource_table_items[j].m_NumElements;
					bindings[j].stageFlags = VK_SHADER_STAGE_ALL;//VK_SHADER_STAGE_ALL_GRAPHICS;
				}
				resource_table_items += slot.m_Size;

				VkDescriptorSetLayoutCreateInfo layout_info = {};
				layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
				layout_info.bindingCount = slot.m_Size;
				layout_info.pBindings = bindings;

				VkResult res = vkCreateDescriptorSetLayout(device->m_VkDevice, &layout_info, VK_NULL_HANDLE, &descriptor_set_layouts[num_descriptor_sets]);
				ASSERT(res == VK_SUCCESS);

				++num_descriptor_sets;
				break;
			}
			default:
				ASSERT(false);
			}
		}


		if (range.size)
		{
			pipeline_layout_create_info.pushConstantRangeCount = 1;
			pipeline_layout_create_info.pPushConstantRanges = &range;
		}

		if (num_descriptor_sets)
		{
			pipeline_layout_create_info.setLayoutCount = num_descriptor_sets;
			pipeline_layout_create_info.pSetLayouts = descriptor_set_layouts;
		}
		// Reset pointer for reiteration
		resource_table_items = (const SResourceTableItem*)((char*)root + root_slots_size);
	}
	VkPipelineLayout pipeline_layout = VK_NULL_HANDLE;
	VkResult res = vkCreatePipelineLayout(device->m_VkDevice, &pipeline_layout_create_info, VK_NULL_HANDLE, &pipeline_layout);
	ASSERT(res == VK_SUCCESS);

	RootSignature root_sig = new SRootSignature();
	root_sig->m_PipelineLayout = pipeline_layout;
	root_sig->m_SlotCount = root->m_NumSlots;
	if(root_sig->m_SlotCount)
		root_sig->m_Slots = new SRootSignature::SRootSlot[root->m_NumSlots];

	uint32 push_constant_offset = 0;
	uint32 descriptor_set = 0;
	for (uint32 i = 0; i < root->m_NumSlots; i++)
	{
		SRootSignature::SRootSlot& slot = root_sig->m_Slots[i];
		const ItemType type = root->m_Slots[i].m_Type;

		slot.m_Type = root->m_Slots[i].m_Type;
		slot.m_Size = root->m_Slots[i].m_Size;
		if (type == CONSTANT)
		{
			slot.m_Offset = push_constant_offset;
			slot.m_DescriptorSetLayout = VK_NULL_HANDLE;
			push_constant_offset += slot.m_Size * sizeof(uint32);
		}
		else
		{
			slot.m_Offset = descriptor_set;
			slot.m_DescriptorSetLayout = descriptor_set_layouts[descriptor_set];
			++descriptor_set;
		}

		if (type == RESOURCE_TABLE || type == SAMPLER_TABLE)
		{
			slot.m_Items = new SResourceTableItem[slot.m_Size];
			memcpy(slot.m_Items, resource_table_items, slot.m_Size * sizeof(SResourceTableItem));
			resource_table_items += slot.m_Size;
		}
		else
		{
			slot.m_Items = VK_NULL_HANDLE;
		}
	}

	return root_sig;
}

void DestroyRootSignature(Device device, RootSignature& root)
{
	if (root)
	{
		vkDestroyPipelineLayout(device->m_VkDevice, root->m_PipelineLayout, VK_NULL_HANDLE);

		for (uint32 i = 0; i < root->m_SlotCount; i++)
		{
			SRootSignature::SRootSlot& slot = root->m_Slots[i];

			vkDestroyDescriptorSetLayout(device->m_VkDevice, slot.m_DescriptorSetLayout, VK_NULL_HANDLE);
			delete [] slot.m_Items;
		}

		delete [] root->m_Slots;

		delete root;
		root = VK_NULL_HANDLE;
	}
}

static VkPipelineShaderStageCreateInfo LoadShader(VkDevice device, const SCodeBlob& blob, VkShaderStageFlagBits stage)
{
	VkShaderModuleCreateInfo module_create_info;
	module_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	module_create_info.pNext = VK_NULL_HANDLE;
	module_create_info.codeSize = blob.m_Size;
	module_create_info.pCode = (uint32_t*) blob.m_Code;
	module_create_info.flags = 0;

	VkShaderModule shader_module = VK_NULL_HANDLE;
	VkResult res = vkCreateShaderModule(device, &module_create_info, VK_NULL_HANDLE, &shader_module);
	ASSERT(res == VK_SUCCESS);

	VkPipelineShaderStageCreateInfo shader_stage = {};
	shader_stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shader_stage.stage = stage;
	shader_stage.module = shader_module;
	shader_stage.pName = "main";

	return shader_stage;
}

Pipeline CreatePipeline(Device device, const SPipelineParams& params)
{
	ASSERT(params.m_RootSignature);

	VkPipelineInputAssemblyStateCreateInfo ia_create_info = {};
	ia_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	ia_create_info.topology = (VkPrimitiveTopology) params.m_PrimitiveType;

	VkVertexInputAttributeDescription attributes[16];
	ASSERT(params.m_AttribCount < elementsof(attributes));

	VkPipelineVertexInputStateCreateInfo vertex_input_state = {};
	vertex_input_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertex_input_state.vertexAttributeDescriptionCount = params.m_AttribCount;
	vertex_input_state.pVertexAttributeDescriptions = attributes;

	uint offsets[4] = {};
	uint stream_count = 0;
	for (uint i = 0; i < params.m_AttribCount; i++)
	{
		uint stream = params.m_Attribs[i].Stream;
		stream_count = max(stream_count, stream + 1);

		AttribFormat format = params.m_Attribs[i].Format;
		ASSERT(stream < elementsof(offsets));

		attributes[i].location = i;
		attributes[i].binding = stream;
		attributes[i].format = g_AttribFormats[format];
		attributes[i].offset = offsets[stream];

		offsets[stream] += g_AttribSizes[format];
	}

	VkVertexInputBindingDescription binding_descs[4] = {};
	for (uint i = 0; i < stream_count; i++)
	{
		binding_descs[i].binding = i;
		binding_descs[i].stride = offsets[i];
		binding_descs[i].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	}

	vertex_input_state.vertexBindingDescriptionCount = stream_count;
	vertex_input_state.pVertexBindingDescriptions = binding_descs;

	VkPipelineRasterizationStateCreateInfo rasterization_state = {};
	rasterization_state.pNext = nullptr;
	rasterization_state.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterization_state.polygonMode = VK_POLYGON_MODE_FILL;
	rasterization_state.cullMode = (VkCullModeFlagBits) params.m_CullMode;
	rasterization_state.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterization_state.depthClampEnable = VK_FALSE;
	rasterization_state.rasterizerDiscardEnable = VK_FALSE;
	rasterization_state.depthBiasEnable = VK_FALSE;
	rasterization_state.lineWidth = 1.0f;

	// Blend state
	static const VkBlendFactor blend_factors[] =
	{
		VK_BLEND_FACTOR_ZERO,
		VK_BLEND_FACTOR_ONE,
		VK_BLEND_FACTOR_SRC_COLOR,
		VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR,
		VK_BLEND_FACTOR_SRC_ALPHA,
		VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
		VK_BLEND_FACTOR_DST_COLOR,
		VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR,
		VK_BLEND_FACTOR_DST_ALPHA,
		VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA,
	};

	VkPipelineColorBlendAttachmentState blend_attachments[1] = {};
	blend_attachments[0].colorWriteMask = 0xf;
	blend_attachments[0].blendEnable = VK_FALSE;

	VkPipelineColorBlendStateCreateInfo blend_info = {};
	blend_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	blend_info.attachmentCount = 1;
	blend_info.pAttachments = blend_attachments;

	if (params.m_BlendState)
	{
		blend_attachments[0].colorWriteMask = params.m_BlendState->m_Mask;
		blend_attachments[0].blendEnable = (params.m_BlendState->m_Src != BF_ONE || params.m_BlendState->m_Dst != BF_ZERO);
		blend_attachments[0].srcColorBlendFactor = blend_factors[params.m_BlendState->m_Src];
		blend_attachments[0].dstColorBlendFactor = blend_factors[params.m_BlendState->m_Dst];
		blend_attachments[0].colorBlendOp = (VkBlendOp) params.m_BlendState->m_Mode;
		blend_attachments[0].srcAlphaBlendFactor = blend_factors[params.m_BlendState->m_Src];
		blend_attachments[0].dstAlphaBlendFactor = blend_factors[params.m_BlendState->m_Dst];
		blend_attachments[0].alphaBlendOp = (VkBlendOp) params.m_BlendState->m_Mode;
	}


	VkPipelineViewportStateCreateInfo viewport_state = {};
	viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewport_state.viewportCount = 1;
	viewport_state.scissorCount = 1;

	VkDynamicState dynamic_states[] =
	{
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR,
	};
	VkPipelineDynamicStateCreateInfo dynamic_state = {};
	dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamic_state.pDynamicStates = dynamic_states;
	dynamic_state.dynamicStateCount = elementsof(dynamic_states);

	VkPipelineDepthStencilStateCreateInfo depth_stencil_state = {};
	depth_stencil_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depth_stencil_state.depthTestEnable = params.m_DepthTest;
	depth_stencil_state.depthWriteEnable = params.m_DepthWrite;
	depth_stencil_state.depthCompareOp = (VkCompareOp) params.m_DepthFunc;
	depth_stencil_state.depthBoundsTestEnable = VK_FALSE;
	depth_stencil_state.back.failOp = VK_STENCIL_OP_KEEP;
	depth_stencil_state.back.passOp = VK_STENCIL_OP_KEEP;
	depth_stencil_state.back.compareOp = VK_COMPARE_OP_ALWAYS;
	depth_stencil_state.stencilTestEnable = VK_FALSE;
	depth_stencil_state.front = depth_stencil_state.back;

	VkPipelineMultisampleStateCreateInfo msaa_state = {};
	msaa_state.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	msaa_state.rasterizationSamples = VkSampleCountFlagBits(params.m_RenderPass->m_MSAASamples);
	msaa_state.pSampleMask = VK_NULL_HANDLE;

	// Load shaders
	ASSERT( (params.m_VS.m_Code != VK_NULL_HANDLE) ^ (params.m_MS.m_Code != VK_NULL_HANDLE) ); // Must have one or the other, not both
	VkPipelineShaderStageCreateInfo shader_stages[3] = {};
	uint num_stages = 0;

	if (params.m_TS.m_Code)
		shader_stages[num_stages++] = LoadShader(device->m_VkDevice, params.m_TS, VK_SHADER_STAGE_TASK_BIT_NV);
	if (params.m_MS.m_Code)
		shader_stages[num_stages++] = LoadShader(device->m_VkDevice, params.m_MS, VK_SHADER_STAGE_MESH_BIT_NV);
	if (params.m_VS.m_Code)
		shader_stages[num_stages++] = LoadShader(device->m_VkDevice, params.m_VS, VK_SHADER_STAGE_VERTEX_BIT);
	if (params.m_PS.m_Code)
		shader_stages[num_stages++] = LoadShader(device->m_VkDevice, params.m_PS, VK_SHADER_STAGE_FRAGMENT_BIT);

	VkSpecializationInfo specialization;
	if (params.m_NumSpecializationConstants)
	{
		VkSpecializationMapEntry* entries = StackAlloc<VkSpecializationMapEntry>(params.m_NumSpecializationConstants);
		for (uint i = 0; i < params.m_NumSpecializationConstants; i++)
			entries[i] = { i, i * sizeof(uint32), sizeof(uint32) };

		specialization = { params.m_NumSpecializationConstants, entries, params.m_NumSpecializationConstants * sizeof(uint32), params.m_SpecializationConstants };

		for (uint i = 0; i < num_stages; i++)
			shader_stages[i].pSpecializationInfo = &specialization;
	}

	// Assign the pipeline states to the pipeline creation info structure
	VkGraphicsPipelineCreateInfo pipeline_create_info = {};
	pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipeline_create_info.layout = params.m_RootSignature->m_PipelineLayout;
	pipeline_create_info.renderPass = params.m_RenderPass->m_RenderPass;
	pipeline_create_info.stageCount = num_stages;
	pipeline_create_info.pStages = shader_stages;
	pipeline_create_info.pVertexInputState = &vertex_input_state;
	pipeline_create_info.pInputAssemblyState = &ia_create_info;
	pipeline_create_info.pRasterizationState = &rasterization_state;
	pipeline_create_info.pColorBlendState = &blend_info;
	pipeline_create_info.pMultisampleState = &msaa_state;
	pipeline_create_info.pViewportState = &viewport_state;
	pipeline_create_info.pDepthStencilState = &depth_stencil_state;
	pipeline_create_info.pDynamicState = &dynamic_state;

	// Create rendering pipeline using the specified states
	VkPipeline vk_pipeline = VK_NULL_HANDLE;
	VkResult res = vkCreateGraphicsPipelines(device->m_VkDevice, VK_NULL_HANDLE, 1, &pipeline_create_info, VK_NULL_HANDLE, &vk_pipeline);
	ASSERT(res == VK_SUCCESS);


	Pipeline pipeline = new SPipeline();
	pipeline->m_Pipeline = vk_pipeline;
	pipeline->m_BindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	pipeline->m_Modules[0] = shader_stages[0].module;
	pipeline->m_Modules[1] = shader_stages[1].module;
	pipeline->m_Modules[2] = shader_stages[2].module;

	return pipeline;
}

Pipeline CreatePipeline(Device device, const SComputePipelineParams& params)
{
	VkPipelineShaderStageCreateInfo shader_stage = LoadShader(device->m_VkDevice, params.m_CS, VK_SHADER_STAGE_COMPUTE_BIT);

	VkSpecializationInfo specialization;
	if (params.m_NumSpecializationConstants)
	{
		VkSpecializationMapEntry* entries = StackAlloc<VkSpecializationMapEntry>(params.m_NumSpecializationConstants);
		for (uint i = 0; i < params.m_NumSpecializationConstants; i++)
			entries[i] = { i, i * sizeof(uint32), sizeof(uint32) };

		specialization = { params.m_NumSpecializationConstants, entries, params.m_NumSpecializationConstants * sizeof(uint32), params.m_SpecializationConstants };
		shader_stage.pSpecializationInfo = &specialization;
	}

	VkComputePipelineCreateInfo pipeline_create_info = {};
	pipeline_create_info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	pipeline_create_info.layout = params.m_RootSignature->m_PipelineLayout;
	pipeline_create_info.stage = shader_stage;

	VkPipeline vk_pipeline = VK_NULL_HANDLE;
	VkResult res = vkCreateComputePipelines(device->m_VkDevice, VK_NULL_HANDLE, 1, &pipeline_create_info, VK_NULL_HANDLE, &vk_pipeline);
	ASSERT(res == VK_SUCCESS);


	Pipeline pipeline = new SPipeline();
	pipeline->m_Pipeline = vk_pipeline;
	pipeline->m_BindPoint = VK_PIPELINE_BIND_POINT_COMPUTE;
	pipeline->m_Modules[0] = shader_stage.module;
	pipeline->m_Modules[1] = VK_NULL_HANDLE;
	pipeline->m_Modules[2] = VK_NULL_HANDLE;

	return pipeline;

}

void DestroyPipeline(Device device, Pipeline& pipeline)
{
	if (pipeline)
	{
		vkDestroyPipeline(device->m_VkDevice, pipeline->m_Pipeline, VK_NULL_HANDLE);

		for (VkShaderModule& m : pipeline->m_Modules)
			vkDestroyShaderModule(device->m_VkDevice, m, VK_NULL_HANDLE);

		delete pipeline;
		pipeline = VK_NULL_HANDLE;
	}
}

BlendState CreateBlendState(Device device, BlendFactor src, BlendFactor dst, BlendMode mode, uint32 mask, bool alpha_to_coverage)
{
	BlendState state = new SBlendState();
	state->m_Src = src;
	state->m_Dst = dst;
	state->m_Mode = mode;
	state->m_Mask = mask;

	return state;
}

void DestroyBlendState(Device device, BlendState& state)
{
	if (state)
	{
		delete state;
		state = VK_NULL_HANDLE;
	}
}

VertexSetup CreateVertexSetup(Device device, Buffer vertex_buffer, uint vb_stride, Buffer index_buffer, uint ib_stride)
{
	VertexSetup setup = new SVertexSetup();
	memset(setup, 0, sizeof(SVertexSetup));

	if (index_buffer)
	{
		ASSERT(ib_stride == sizeof(uint16) || ib_stride == sizeof(uint32));

		setup->m_IndexBuffer = index_buffer->m_Buffer;
		setup->m_IndexType = (ib_stride == sizeof(uint16))? VK_INDEX_TYPE_UINT16 : VK_INDEX_TYPE_UINT32;
	}

	if (vertex_buffer)
	{
		ASSERT(vb_stride > 0);

		setup->m_VertexBuffer = vertex_buffer->m_Buffer;
	}


	return setup;
}

void DestroyVertexSetup(Device device, VertexSetup& state)
{
	if (state)
	{
		delete state;
		state = VK_NULL_HANDLE;
	}
}

static void CreateUniformBufferDescriptorSet(Device device, VkBuffer buffer, size_t size, VkDescriptorSetLayout* descriptor_set_layout, VkDescriptorSet& descriptor_set)
{
	VkDescriptorSetAllocateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	info.descriptorPool = device->m_VkDescriptorPool;
	info.descriptorSetCount = 1;
	info.pSetLayouts = descriptor_set_layout;
	vkAllocateDescriptorSets(device->m_VkDevice, &info, &descriptor_set);


	VkDescriptorBufferInfo buffer_info;
	buffer_info.buffer = buffer;
	buffer_info.offset = 0;
	buffer_info.range  = size;

	VkWriteDescriptorSet descriptor_write = {};
	descriptor_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptor_write.dstSet = descriptor_set;
	descriptor_write.dstBinding = 0;
	descriptor_write.dstArrayElement = 0;
	descriptor_write.descriptorCount = 1;
	descriptor_write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	descriptor_write.pBufferInfo = &buffer_info;
	vkUpdateDescriptorSets(device->m_VkDevice, 1, &descriptor_write, 0, VK_NULL_HANDLE);
}

Buffer CreateBuffer(Device device, const SBufferParams& params)
{
	VkBufferCreateInfo buffer_create_info = {};
	buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	buffer_create_info.size = params.m_Size;
	buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (params.m_Usage & VERTEX_BUFFER)
		buffer_create_info.usage |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	if (params.m_Usage & INDEX_BUFFER)
		buffer_create_info.usage |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
	if (params.m_Usage & SHADER_RESOURCE)
		buffer_create_info.usage |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
	if (params.m_Usage & CONSTANT_BUFFER)
		buffer_create_info.usage |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
	if (params.m_Usage & INDIRECT_PARAM)
		buffer_create_info.usage |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;

	if (params.m_HeapType != HEAP_READBACK)
		buffer_create_info.usage |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

	if (params.m_HeapType != HEAP_UPLOAD)
		buffer_create_info.usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;


	VkBuffer vk_buffer = VK_NULL_HANDLE;
	VkResult res = vkCreateBuffer(device->m_VkDevice, &buffer_create_info, VK_NULL_HANDLE, &vk_buffer);
	if (res != VK_SUCCESS)
		return VK_NULL_HANDLE;

	VkMemoryRequirements mem_reqs;
	vkGetBufferMemoryRequirements(device->m_VkDevice, vk_buffer, &mem_reqs);

	uint32 memory_bits = (params.m_HeapType == HEAP_DEFAULT)? VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT : (VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	VkMemoryAllocateInfo mem_alloc_info = {};
	mem_alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	mem_alloc_info.allocationSize = mem_reqs.size;
	mem_alloc_info.memoryTypeIndex = GetMemoryTypeIndex(device->m_VkMemoryProperties, mem_reqs.memoryTypeBits, memory_bits);

	VkDeviceMemory memory = VK_NULL_HANDLE;
	res = vkAllocateMemory(device->m_VkDevice, &mem_alloc_info, VK_NULL_HANDLE, &memory);
	if (res != VK_SUCCESS)
		return VK_NULL_HANDLE;

	vkBindBufferMemory(device->m_VkDevice, vk_buffer, memory, 0);

	Buffer buffer = new SBuffer();
	buffer->m_Buffer = vk_buffer;
	buffer->m_Size = params.m_Size;
	buffer->m_HeapType = params.m_HeapType;
	buffer->m_Memory = memory;
	return buffer;
}

void DestroyBuffer(Device device, Buffer& buffer)
{
	if (buffer)
	{
		vkFreeMemory(device->m_VkDevice, buffer->m_Memory, VK_NULL_HANDLE);
		vkDestroyBuffer(device->m_VkDevice, buffer->m_Buffer, VK_NULL_HANDLE);

		delete buffer;
		buffer = VK_NULL_HANDLE;
	}
}

uint GetBufferSize(Buffer buffer)
{
	return buffer->m_Size;
}
#define CONTANT_BUFFER_SIZE  1024*1024
void SCommandList::Init(Device device)
{

	m_RenderSetups = new RenderSetupVector;
	// Fences
	VkFenceCreateInfo fence_create_info = {};
	fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
	VkResult res = vkCreateFence(device->m_VkDevice, &fence_create_info, VK_NULL_HANDLE, &m_WaitFence);
	ASSERT(res == VK_SUCCESS);

	//commandbuffer
	VkCommandBufferAllocateInfo cmd_buffer_alloc_info = {};
	cmd_buffer_alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	cmd_buffer_alloc_info.commandPool = device->m_VkCommandPool;
	cmd_buffer_alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	cmd_buffer_alloc_info.commandBufferCount = 1;
	res = vkAllocateCommandBuffers(device->m_VkDevice, &cmd_buffer_alloc_info, &m_VkCommandBuffer);
	ASSERT(res == VK_SUCCESS);

	//descriptor pool
	VkDescriptorPoolSize pool_sizes[] =
	{
		{ VK_DESCRIPTOR_TYPE_SAMPLER, 16 },
		{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 256 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 64 },
	};

	VkDescriptorPoolCreateInfo desc_pool_create_info = {};
	desc_pool_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	desc_pool_create_info.poolSizeCount = elementsof(pool_sizes);
	desc_pool_create_info.pPoolSizes = pool_sizes;
	desc_pool_create_info.maxSets = 1024;

	res = vkCreateDescriptorPool(device->m_VkDevice, &desc_pool_create_info, VK_NULL_HANDLE, &m_VkDescriptorPool);
	ASSERT(res == VK_SUCCESS);

	SBufferParams cb(1024 * 1024, HEAP_UPLOAD, CONSTANT_BUFFER, "ConstantBuffer");
	m_Constants.m_Buffer = CreateBuffer(device, cb);
	m_Constants.m_Cursor = 0;
	m_Constants.m_Data = VK_NULL_HANDLE;

	vkMapMemory(device->m_VkDevice, m_Constants.m_Buffer->m_Memory, 0, m_Constants.m_Buffer->m_Size, 0, (void**)&m_Constants.m_Data);
	
	VkQueryPoolCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
	info.queryType = VK_QUERY_TYPE_TIMESTAMP;
	info.queryCount = MAX_QUERY_COUNT;
	res = vkCreateQueryPool(device->m_VkDevice, &info, VK_NULL_HANDLE, &m_QueryPool);
	ASSERT(res == VK_SUCCESS);

	SBufferParams params(MAX_QUERY_COUNT * sizeof(uint64), HEAP_READBACK, NONE, "QueryBuffer");
	m_QueryBuffer = CreateBuffer(device, params);

}

void SCommandList::Destroy(Device device)
{
	delete m_RenderSetups;

	vkDestroyFence(device->m_VkDevice, m_WaitFence, VK_NULL_HANDLE);
	vkDestroyDescriptorPool(device->m_VkDevice, m_VkDescriptorPool, VK_NULL_HANDLE);
	DestroyBuffer(device, m_Constants.m_Buffer);

	vkDestroyQueryPool(device->m_VkDevice, m_QueryPool, VK_NULL_HANDLE);
	DestroyBuffer(device, m_QueryBuffer);
	DestroyBuffer(device, m_Staging.m_Buffer);
}

void SCommandList::Begin(Device device, uint size)
{
	// Use a fence to wait until the command buffer has finished execution before using it again
	VkResult res = vkWaitForFences(device->m_VkDevice, 1, &m_WaitFence, VK_TRUE, UINT64_MAX);
	ASSERT(res == VK_SUCCESS);
	res = vkResetFences(device->m_VkDevice, 1, &m_WaitFence);
	ASSERT(res == VK_SUCCESS);

	vkResetDescriptorPool(device->m_VkDevice, m_VkDescriptorPool, 0);
	m_Constants.m_Cursor = 0;
	for (auto& fb : *m_RenderSetups)
		DestroyRenderSetup(device, fb);
	m_RenderSetups->clear();
		
	uint curr_size = m_Staging.m_Buffer ? m_Staging.m_Buffer->m_Size : 0;
	if (size != curr_size)
	{
		SBufferParams params(size, HEAP_UPLOAD, VERTEX_BUFFER | INDEX_BUFFER | CONSTANT_BUFFER, "UploadBuffer");
		m_Staging.m_Buffer = CreateBuffer(device, params);
		m_Staging.m_Data = nullptr;
		m_Staging.m_Cursor = 0;
	}

	m_QueryOffset = 0;

	VkCommandBufferBeginInfo cmdListInfo = {};
	cmdListInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	cmdListInfo.pNext = VK_NULL_HANDLE;
	cmdListInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	res = vkBeginCommandBuffer(m_VkCommandBuffer, &cmdListInfo);
	ASSERT(res == VK_SUCCESS);
}

void SCommandList::End(Device device)
{
	if (m_Staging.m_Data)
	{
		vkUnmapMemory(device->m_VkDevice, m_Staging.m_Buffer->m_Memory);
		m_Staging.m_Data = nullptr;
		m_Staging.m_Cursor = 0;
	}

	if (m_Constants.m_Data)
	{
		vkUnmapMemory(device->m_VkDevice, m_Constants.m_Buffer->m_Memory);
		m_Constants.m_Data = nullptr;
		m_Constants.m_Cursor = 0;
	}
	if (m_QueryOffset)
	{
		vkCmdCopyQueryPoolResults(m_VkCommandBuffer, m_QueryPool, 0, m_QueryOffset, m_QueryBuffer->m_Buffer, 0, sizeof(uint64), VK_QUERY_RESULT_64_BIT | VK_QUERY_RESULT_WAIT_BIT);
	}

	VkResult res = vkEndCommandBuffer(m_VkCommandBuffer);
	ASSERT(res == VK_SUCCESS);
}


uint32 AllocateBufferSlice(Device device, SlicedBuffer& buffer, uint size, uint alignment)
{
	uint aligned_offset = (buffer.m_Cursor + alignment - 1) & ~(alignment - 1);

	if (aligned_offset + size > buffer.m_Buffer->m_Size)
	{
		// Reallocate
		ASSERT(false);
	}

	if (buffer.m_Data == VK_NULL_HANDLE)
	{
		vkMapMemory(device->m_VkDevice, buffer.m_Buffer->m_Memory, 0, buffer.m_Buffer->m_Size, 0, (void**) &buffer.m_Data);
	}

	buffer.m_Cursor = aligned_offset + size;

	return aligned_offset;
}

void BeginMarker(Context context, const char* name)
{
#ifdef USE_DEBUG_MARKERS
	VkDebugMarkerMarkerInfoEXT info = { VK_STRUCTURE_TYPE_DEBUG_MARKER_MARKER_INFO_EXT, VK_NULL_HANDLE, name, { 0.0f, 0.0f, 0.0f, 0.0f } };
	vkCmdDebugMarkerBegin(context->GetVkCommandBuffer(), &info);
#endif

	if (context->m_IsProfiling)
	{
		SCommandList* cmdList = context->m_CmdList;
		ASSERT(cmdList->m_QueryOffset < MAX_QUERY_COUNT);

		cmdList->m_QueryNames[cmdList->m_QueryOffset] = name;
		vkCmdWriteTimestamp(context->GetVkCommandBuffer(), VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, cmdList->m_QueryPool, cmdList->m_QueryOffset++);
	}
}

void EndMarker(Context context)
{
	if (context->m_IsProfiling)
	{
		SCommandList* cmdList = context->m_CmdList;
		ASSERT(cmdList->m_QueryOffset < MAX_QUERY_COUNT);

		cmdList->m_QueryNames[cmdList->m_QueryOffset] = VK_NULL_HANDLE;
		vkCmdWriteTimestamp(context->GetVkCommandBuffer(), VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, cmdList->m_QueryPool, cmdList->m_QueryOffset++);
	}

#ifdef USE_DEBUG_MARKERS
	vkCmdDebugMarkerEnd(context->GetVkCommandBuffer());
#endif
}

void SetRenderTarget(Context context, SResourceDesc target, uint slot)
{
	Device device = context->m_Device;
	assert(slot < MAX_COLOR_TARGETS);
	context->m_CurrentFrameBufferDesc.m_ColorTargets[slot] = target;
}

void SetDepthTarget(Context context, SResourceDesc depth)
{
	context->m_CurrentFrameBufferDesc.m_DepthTarget = depth;
}

void SetClearColors(Context context, const float clear_color[4], const float clear_depth[2])
{
	memcpy(&context->m_ClearColor[0], &clear_color[0], 4*sizeof(float));
	memcpy(&context->m_ClearDepth[0], &clear_depth[0], 2*sizeof(float));
}

void BeginContext(Context context, uint upload_buffer_size, const char* name, bool profile)
{
	context->m_IsProfiling = profile;

	Device device = context->m_Device;

	context->m_CurrentRenderSetup = nullptr;
	memset(&context->m_CurrentFrameBufferDesc, 0, sizeof(SFrameBuffer));
	
	context->m_CmdList = &device->m_CommandBuffers[device->m_VkSwapChainImageId];

	context->m_CmdList->Begin(device, upload_buffer_size);

	if (profile)
	{
		vkCmdResetQueryPool(context->GetVkCommandBuffer(), context->m_CmdList->m_QueryPool, 0, MAX_QUERY_COUNT);
	}

	BeginMarker(context, name);
}

void EndContext(Context context)
{
	EndMarker(context);

	context->m_CmdList->End(GetDevice(context));
}

void SubmitContexts(Device device, uint count, SContext** context)
{
	ASSERT(count == 1); // TODO: Fix

	VkPipelineStageFlags wait_stage_mask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;//VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

	VkSubmitInfo submit_info = {};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.pWaitDstStageMask = &wait_stage_mask;
	if (device->m_WasAcquired)
	{
		device->m_WasAcquired = false;

		submit_info.pWaitSemaphores = &device->m_PresentSemaphore;
		submit_info.waitSemaphoreCount = 1;
	}
	submit_info.pCommandBuffers = &context[0]->m_CmdList->m_VkCommandBuffer;
	submit_info.commandBufferCount = 1;

	VkResult res = vkQueueSubmit(device->m_VkGraphicsQueue, 1, &submit_info, context[0]->m_CmdList->m_WaitFence);
	ASSERT(res == VK_SUCCESS);
}

void Finish(Device device)
{
	//vkQueueWaitIdle(device->m_Queue);

	VkResult res = vkWaitForFences(device->m_VkDevice, 1, &device->m_MainContext->m_CmdList->m_WaitFence, VK_TRUE, UINT64_MAX);
	ASSERT(res == VK_SUCCESS);
}

uint GetProfileData(Device device, SProfileData (&OutData)[MAX_QUERY_COUNT])
{
	const SCommandList& cmdList = *device->m_MainContext->m_CmdList;

	const uint count = cmdList.m_QueryOffset;
	if (count)
	{
		uint64* data = VK_NULL_HANDLE;
		vkMapMemory(device->m_VkDevice, cmdList.m_QueryBuffer->m_Memory, 0, count * sizeof(uint64), 0, (void**) &data);

		for (uint i = 0; i < count; i++)
		{
			OutData[i].m_TimeStamp = data[i];
			OutData[i].m_Name = cmdList.m_QueryNames[i];
		}

		vkUnmapMemory(device->m_VkDevice, cmdList.m_QueryBuffer->m_Memory);
	}

	return count;
}

float GetTimestampFrequency(Device device)
{
	return device->m_TimestampFrequency;
}

void CopyBuffer(Context context, const Buffer dest, const Buffer src)
{
	VkBufferCopy buffer_copy;
	buffer_copy.srcOffset = 0;
	buffer_copy.dstOffset = 0;
	buffer_copy.size = dest->m_Size;
	vkCmdCopyBuffer(context->GetVkCommandBuffer(), src->m_Buffer, dest->m_Buffer, 1, &buffer_copy);
}

uint8* MapBuffer(const SMapBufferParams& params)
{
	uint8* data = VK_NULL_HANDLE;

	if (params.m_Buffer->m_HeapType == HEAP_DEFAULT)
	{
		SCommandList* cmdList = params.m_Context->m_CmdList;

		uint offset = AllocateBufferSlice(params.m_Device, cmdList->m_Staging, params.m_Size, 16);
		data = cmdList->m_Staging.m_Data + offset;

		params.m_InternalOffset = offset;
	}
	else
	{
		vkMapMemory(params.m_Device->m_VkDevice, params.m_Buffer->m_Memory, 0, params.m_Buffer->m_Size, 0, (void**) &data);
		data += params.m_Offset;
	}

	return data;
}

void UnmapBuffer(const SMapBufferParams& params)
{
	if (params.m_Buffer->m_HeapType == HEAP_DEFAULT)
	{
		Context context = params.m_Context;

		VkBufferCopy buffer_copy;
		buffer_copy.srcOffset = params.m_InternalOffset;
		buffer_copy.dstOffset = params.m_Offset;
		buffer_copy.size = params.m_Size;
		vkCmdCopyBuffer(context->GetVkCommandBuffer(), context->m_CmdList->m_Staging.m_Buffer->m_Buffer, params.m_Buffer->m_Buffer, 1, &buffer_copy);
	}
	else
	{
		vkUnmapMemory(params.m_Device->m_VkDevice, params.m_Buffer->m_Memory);
	}
}
uint GetWidth(Texture texture)
{
	return texture->m_Width;
}
uint GetHeight(Texture texture)
{
	return texture->m_Height;
}
uint GetDepth(Texture texture)
{
	return texture->m_Depth;
}

void SetTextureData(Context context, Texture texture, uint mip, uint slice, const void* data, uint size, uint pitch)
{
	const uint format_size = GetBytesPerBlock(texture->m_Format);
	const bool is_compressed = IsCompressed(texture->m_Format);

	uint w = max(texture->m_Width  >> mip, 1U);
	uint h = max(texture->m_Height >> mip, 1U);
	uint d = max(texture->m_Depth  >> mip, 1U);;

	uint cols = w;
	uint rows = h;
	if (is_compressed)
	{
		cols = (cols + 3) >> 2;
		rows = (rows + 3) >> 2;
	}
	rows *= d;

	ASSERT(pitch * rows == size);

	uint src_pitch = pitch;
	uint dst_pitch = Align(pitch, format_size);

	// Grab upload buffer space
	uint dst_offset = AllocateBufferSlice(context->m_Device, context->m_CmdList->m_Staging, dst_pitch * rows, format_size);

	// Copy data in place
	uint8* src_data = (uint8*) data;
	uint8* dst_data = context->m_CmdList->m_Staging.m_Data + dst_offset;
	for (uint32 r = 0; r < rows; ++r)
	{
		memcpy(dst_data, src_data, src_pitch);
		dst_data += dst_pitch;
		src_data += src_pitch;
	}

	VkImageMemoryBarrier barrier = {};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.image = texture->m_Image;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel = mip;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = slice;
	barrier.subresourceRange.layerCount = 1;
	barrier.srcAccessMask = 0;
	barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	vkCmdPipelineBarrier(context->GetVkCommandBuffer(), VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, VK_NULL_HANDLE, 0, VK_NULL_HANDLE, 1, &barrier);


	// Issue a copy from upload buffer to texture
	VkBufferImageCopy image_copy = {};
	image_copy.bufferOffset = dst_offset;
	image_copy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	image_copy.imageSubresource.mipLevel = mip;
	image_copy.imageSubresource.baseArrayLayer = slice;
	image_copy.imageSubresource.layerCount = 1;
	image_copy.imageExtent.width  = w;
	image_copy.imageExtent.height = h;
	image_copy.imageExtent.depth  = d;
	image_copy.bufferRowLength = /*blocks number*/(dst_pitch / format_size) * /*pixels per block*/(w / cols)/*=pixel number*/;
	image_copy.bufferImageHeight = h;
	vkCmdCopyBufferToImage(context->GetVkCommandBuffer(), context->m_CmdList->m_Staging.m_Buffer->m_Buffer, texture->m_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &image_copy);


	barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	vkCmdPipelineBarrier(context->GetVkCommandBuffer(), VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, VK_NULL_HANDLE, 0, VK_NULL_HANDLE, 1, &barrier);
}

RenderPass GetCurrentRenderPass(Context context)
{
	return context->m_CurrentRenderSetup->m_Pass;
}

void BeginRenderPass(Context context, const char* name, bool clearColor, bool clearDepth)
{
	Device device = context->m_Device;
	SRenderPassDesc passDesc; memset(&passDesc, 0, sizeof(SRenderPassDesc));
	passDesc.msaa_samples = 1;
	int index = 0;
	for (SResourceDesc& color_target : context->m_CurrentFrameBufferDesc.m_ColorTargets)
	{
		if (color_target.m_Resource)
		{
			passDesc.m_ColorFormats[index] = ((Texture)color_target.m_Resource)->m_Format;
		}
		index++;
	}

	SResourceDesc& depth_target = context->m_CurrentFrameBufferDesc.m_DepthTarget;
	if (depth_target.m_Resource)
		passDesc.m_DepthFormat = ((Texture)depth_target.m_Resource)->m_Format;
	
	if (clearColor && context->m_CurrentFrameBufferDesc.m_ColorTargets[0].m_Resource)
		passDesc.m_Flags = passDesc.m_Flags | CLEAR_COLOR;
	if (clearDepth && context->m_CurrentFrameBufferDesc.m_DepthTarget.m_Resource)
		passDesc.m_Flags = passDesc.m_Flags | CLEAR_DEPTH;
	
	RenderPass pass = AcquireRenderPass(device, passDesc);
	context->m_CurrentRenderSetup = CreateRenderSetup(device, pass, context->m_CurrentFrameBufferDesc);
	RenderSetupVector& vec = *context->m_CmdList->m_RenderSetups;
	
	vec.push_back(context->m_CurrentRenderSetup);
	BeginRenderPass(context, name, pass, context->m_CurrentRenderSetup);
}
void EndRenderPass(Context context)
{
	EndRenderPass(context, context->m_CurrentRenderSetup);
}

void BeginRenderPass(Context context, const char* name, const RenderPass render_pass, const RenderSetup setup)
{
	BeginMarker(context, name);

	VkClearValue clear_values[2];
	VkClearValue* clear_value = &clear_values[0];
	if (render_pass->m_Flags & CLEAR_COLOR)
	{
		memcpy(clear_value->color.float32, &context->m_ClearColor[0], 4 * sizeof(float));
		clear_value++;
	}
	if (render_pass->m_Flags & CLEAR_DEPTH)
	{
		clear_value->depthStencil = { context->m_ClearDepth[0], (uint32)context->m_ClearDepth[1] };
		clear_value++;
	}

	VkRenderPassBeginInfo begin_info = {};
	begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	begin_info.pNext = VK_NULL_HANDLE;
	begin_info.renderPass = render_pass->m_RenderPass;
	begin_info.renderArea.offset.x = 0;
	begin_info.renderArea.offset.y = 0;
	begin_info.renderArea.extent.width  = setup->m_Width;
	begin_info.renderArea.extent.height = setup->m_Height;
	begin_info.clearValueCount = uint32(clear_value - clear_values);
	begin_info.pClearValues = begin_info.clearValueCount? clear_values : VK_NULL_HANDLE;
	begin_info.framebuffer = setup->m_FrameBuffer;
	vkCmdBeginRenderPass(context->GetVkCommandBuffer(), &begin_info, VK_SUBPASS_CONTENTS_INLINE);

	VkViewport viewport = {};
	viewport.x = 0;
	viewport.y = (float) setup->m_Height; // Use negative viewport to flip Vulkan's window coordinate system
	viewport.width  = (float) setup->m_Width;
	viewport.height = -(float) setup->m_Height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(context->GetVkCommandBuffer(), 0, 1, &viewport);

	VkRect2D scissor = {};
	scissor.extent.width  = setup->m_Width;
	scissor.extent.height = setup->m_Height;
	scissor.offset.x = 0;
	scissor.offset.y = 0;
	vkCmdSetScissor(context->GetVkCommandBuffer(), 0, 1, &scissor);
}

void EndRenderPass(Context context, const RenderSetup setup)
{
	vkCmdEndRenderPass(context->GetVkCommandBuffer());
	EndMarker(context);
}

void TransitionRenderSetup(Context context, const RenderSetup setup, EResourceState state_before, EResourceState state_after)
{
}

void SetRootSignature(Context context, const RootSignature root)
{
	context->m_CurrentRootSignature = root;
}

void SetPipeline(Context context, const Pipeline pipeline)
{
	vkCmdBindPipeline(context->GetVkCommandBuffer(), pipeline->m_BindPoint, pipeline->m_Pipeline);
}

void SetVertexSetup(Context context, const VertexSetup setup)
{
	if (setup->m_IndexBuffer)
	{
		vkCmdBindIndexBuffer(context->GetVkCommandBuffer(), setup->m_IndexBuffer, 0, setup->m_IndexType);
	}

	if (setup->m_VertexBuffer)
	{
		const VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(context->GetVkCommandBuffer(), 0, 1, &setup->m_VertexBuffer, offsets);
	}
}

uint8* SetVertexBuffer(Context context, uint stream, uint stride, uint count)
{
	ASSERT(false);
	return VK_NULL_HANDLE;
}

uint32 AllocateConstantsSlice(Context context, uint size)
{
	Device device = GetDevice(context);
	uint32 offset = AllocateBufferSlice(device, context->m_CmdList->m_Constants, size, device->m_UniformBufferAlignment);
	return offset;
}
Buffer GetConstantBuffer(Context context)
{
	return  context->m_CmdList->m_Constants.m_Buffer;
}
uint8* GetConstantBufferData(Context context, uint32 offset)
{
	return context->m_CmdList->m_Constants.m_Data + offset;
}

void SetRootConstants(Context context, uint slot, const void* data, uint count)
{
	ASSERT(count == context->m_CurrentRootSignature->m_Slots[slot].m_Size);

	uint32 offset = context->m_CurrentRootSignature->m_Slots[slot].m_Offset;
	vkCmdPushConstants(context->GetVkCommandBuffer(), context->m_CurrentRootSignature->m_PipelineLayout, VK_SHADER_STAGE_ALL, offset, count * sizeof(uint32), data);
}

void SetRootTextureBuffer(Context context, uint slot, const Buffer buffer)
{
	ASSERT(false);
}

void SetGraphicsResourceTable(Context context, uint slot, const ResourceTable table)
{
	uint32 descriptor_set = context->m_CurrentRootSignature->m_Slots[slot].m_Offset;
	vkCmdBindDescriptorSets(context->GetVkCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, context->m_CurrentRootSignature->m_PipelineLayout, descriptor_set, 1, &table->m_DescriptorSet, 0, VK_NULL_HANDLE);
}

void SetComputeResourceTable(Context context, uint slot, const ResourceTable table)
{
	uint32 descriptor_set = context->m_CurrentRootSignature->m_Slots[slot].m_Offset;
	vkCmdBindDescriptorSets(context->GetVkCommandBuffer(), VK_PIPELINE_BIND_POINT_COMPUTE, context->m_CurrentRootSignature->m_PipelineLayout, descriptor_set, 1, &table->m_DescriptorSet, 0, VK_NULL_HANDLE);
}

void SetGraphicsSamplerTable(Context context, uint slot, const SamplerTable table)
{
	uint32 descriptor_set = context->m_CurrentRootSignature->m_Slots[slot].m_Offset;
	vkCmdBindDescriptorSets(context->GetVkCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, context->m_CurrentRootSignature->m_PipelineLayout, descriptor_set, 1, &table->m_DescriptorSet, 0, VK_NULL_HANDLE);
}

void SetComputeSamplerTable(Context context, uint slot, const SamplerTable table)
{
	uint32 descriptor_set = context->m_CurrentRootSignature->m_Slots[slot].m_Offset;
	vkCmdBindDescriptorSets(context->GetVkCommandBuffer(), VK_PIPELINE_BIND_POINT_COMPUTE, context->m_CurrentRootSignature->m_PipelineLayout, descriptor_set, 1, &table->m_DescriptorSet, 0, VK_NULL_HANDLE);
}

void Draw(Context context, uint start, uint count)
{
	vkCmdDraw(context->GetVkCommandBuffer(), count, 1, start, 0);
}

void DrawIndexed(Context context, uint start, uint count)
{
	vkCmdDrawIndexed(context->GetVkCommandBuffer(), count, 1, start, 0, 0);
}

void DrawIndexedInstanced(Context context, uint start, uint count, uint start_instance, uint instance_count)
{
	vkCmdDrawIndexed(context->GetVkCommandBuffer(), count, instance_count, start, 0, start_instance);
}

void DrawIndexedIndirect(Context context, Buffer buffer, uint offset)
{
	vkCmdDrawIndexedIndirect(context->GetVkCommandBuffer(), buffer->m_Buffer, offset, 1, 0);
}

void DrawMeshTask(Context context, uint start, uint count)
{
	// MaxDrawMeshTasksCount is currently set very low in NVIDIA drivers, only 65535, so we may have to issue multiple calls if count is larger than that.
	const uint max_count = context->m_Device->m_MaxDrawMeshTasksCount;
	while (count > max_count)
	{
		vkCmdDrawMeshTasks(context->GetVkCommandBuffer(), max_count, start);
		start += max_count;
		count -= max_count;
	}
	vkCmdDrawMeshTasks(context->GetVkCommandBuffer(), count, start);
}

void Dispatch(Context context, uint group_x, uint group_y, uint group_z)
{
	vkCmdDispatch(context->GetVkCommandBuffer(), group_x, group_y, group_z);
}

void DispatchIndirect(Context context, Buffer buffer, uint offset)
{
	vkCmdDispatchIndirect(context->GetVkCommandBuffer(), buffer->m_Buffer, offset);
}

void ClearBuffer(Context context, Buffer buffer, uint32 clear_value)
{
	vkCmdFillBuffer(context->GetVkCommandBuffer(), buffer->m_Buffer, 0, VK_WHOLE_SIZE, clear_value);
}

void UAVBarrier(Context context, Buffer buffer)
{
	VkMemoryBarrier mem_barrier = {};
	mem_barrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
	mem_barrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
	mem_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	vkCmdPipelineBarrier(context->GetVkCommandBuffer(), VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 1, &mem_barrier, 0, VK_NULL_HANDLE, 0, VK_NULL_HANDLE);


#if 0
	// Full pipeline barrier. Enable for debugging purposes
	VkMemoryBarrier mem_barrier = {};
	mem_barrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
	mem_barrier.srcAccessMask =
		VK_ACCESS_INDIRECT_COMMAND_READ_BIT | VK_ACCESS_INDEX_READ_BIT | VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT | VK_ACCESS_UNIFORM_READ_BIT | VK_ACCESS_INPUT_ATTACHMENT_READ_BIT | VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
		VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_TRANSFER_READ_BIT | VK_ACCESS_TRANSFER_WRITE_BIT | VK_ACCESS_HOST_READ_BIT |VK_ACCESS_HOST_WRITE_BIT;
	mem_barrier.dstAccessMask =
		VK_ACCESS_INDIRECT_COMMAND_READ_BIT | VK_ACCESS_INDEX_READ_BIT | VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT | VK_ACCESS_UNIFORM_READ_BIT | VK_ACCESS_INPUT_ATTACHMENT_READ_BIT | VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
		VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_TRANSFER_READ_BIT | VK_ACCESS_TRANSFER_WRITE_BIT | VK_ACCESS_HOST_READ_BIT |  VK_ACCESS_HOST_WRITE_BIT;
	vkCmdPipelineBarrier(context->GetVkCommandBuffer(), VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 1, &mem_barrier, 0, VK_NULL_HANDLE, 0, VK_NULL_HANDLE);
#endif
}

static VkAccessFlags GetFlags(EResourceState state)
{
	VkAccessFlags flags = 0;
	if (state & RS_VERTEX_BUFFER)
		flags |= VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
	if (state & RS_INDEX_BUFFER)
		flags |= VK_ACCESS_INDEX_READ_BIT;
	if (state & RS_INDIRECT_ARGUMENT)
		flags |= VK_ACCESS_INDIRECT_COMMAND_READ_BIT;
	if (state & RS_UNORDERED_ACCESS)
		flags |= VK_ACCESS_SHADER_WRITE_BIT;
	if (state & RS_SHADER_READ)
		flags |= VK_ACCESS_SHADER_READ_BIT;
	if (state & RS_TRANSFER_DST)
		flags |= VK_ACCESS_TRANSFER_WRITE_BIT;
	if (state & RS_TRANSFER_SRC)
		flags |= VK_ACCESS_TRANSFER_READ_BIT;
	if (state & RS_UNDEFINED)
		return VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
	return flags;
}
static VkImageLayout GetImageLayout(EResourceState state, bool depth)
{
	switch (state)
	{
	case RS_PRESENT:
		return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	case RS_SHADER_READ:
		return depth? VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL: VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	case RS_RENDER_TARGET:
		return depth? VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL: VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	case RS_TRANSFER_DST:
		return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	case RS_TRANSFER_SRC:
		return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
	case RS_UNORDERED_ACCESS:
		return VK_IMAGE_LAYOUT_GENERAL;
	default:
		return VK_IMAGE_LAYOUT_UNDEFINED;
	};
}
EResourceState GetCurrentState(const SResourceDesc& resource)
{
	return resource.m_Type == RESTYPE_TEXTURE?AcquireTextureSubresource(resource)->m_State:((Buffer)resource.m_Resource)->m_State;
}

void Barrier(Context context, const SBarrierDesc* barriers, uint count)
{
    VkImageMemoryBarrier img_barrier = {};
    img_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    img_barrier.srcQueueFamilyIndex = context->m_Device->m_VkGraphicsQueueIndex;
    img_barrier.dstQueueFamilyIndex = context->m_Device->m_VkGraphicsQueueIndex;
    uint image_count = 0;

    VkMemoryBarrier mem_barrier = {};
    mem_barrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;

    for (uint i = 0; i < count; i++)
    {
		EResourceState before = GetCurrentState(barriers[i].m_Desc);
        if (barriers[i].m_Desc.m_Type == RESTYPE_TEXTURE)
		{
			STextureSubresourceDesc range = barriers[i].m_Desc.m_texRange;
            const Texture texture = (Texture)barriers[i].m_Desc.m_Resource;
            bool isCube = texture->m_Type == TEX_CUBE || texture->m_Type == TEX_CUBE_ARRAY;
            uint ifCube6 = isCube ? 6 : 1;
            img_barrier.srcAccessMask = GetFlags(before);
            img_barrier.dstAccessMask = GetFlags(barriers[i].m_Transition);
			bool isDepth = IsDepthFormat(texture->m_Format);
			img_barrier.subresourceRange.aspectMask = isDepth ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
			img_barrier.oldLayout = GetImageLayout(before, isDepth);
			img_barrier.newLayout = GetImageLayout(barriers[i].m_Transition, isDepth);

            img_barrier.subresourceRange.baseMipLevel = range.mip == -1? 0 : range.mip;
            img_barrier.subresourceRange.levelCount = range.mip == -1 ? texture->m_MipLevels : 1;
            img_barrier.subresourceRange.baseArrayLayer = range.slice == -1 ? 0 : ifCube6* range.slice + (range.face == -1?0: range.face);
            img_barrier.subresourceRange.layerCount = range.slice == -1 ? ifCube6 * texture->m_Slices : (range.face == -1? ifCube6 : 1);

            img_barrier.image = texture->m_Image;

            image_count++;
			AcquireTextureSubresource(barriers[i].m_Desc)->m_State = barriers[i].m_Transition;
        }
        else
        {
			const Buffer buffer = (Buffer)barriers[i].m_Desc.m_Resource;

            mem_barrier.srcAccessMask = GetFlags(before);
            mem_barrier.dstAccessMask = GetFlags(barriers[i].m_Transition);
			buffer->m_State = barriers[i].m_Transition;

        }
    }

    vkCmdPipelineBarrier(context->GetVkCommandBuffer(), VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 1, &mem_barrier, 0, VK_NULL_HANDLE, image_count, image_count ? &img_barrier : VK_NULL_HANDLE);
}
#endif