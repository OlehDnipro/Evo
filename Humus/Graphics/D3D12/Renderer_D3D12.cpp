
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
#ifdef GRAPHICS_API_D3D12
#include "../Renderer.h"
#include "../../Util/Array.h"
#include "../../Util/IDAllocator.h"
#include "../RootSignature.h"

#include <dxgi1_4.h>
#include <d3d12.h>
#include <d3dcompiler.h>
#include <map>
#include <unordered_map>

#include <vector>
//#define USE_PIX

#ifdef USE_PIX
#include "pix3.h"
#endif

#pragma comment (lib, "dxgi.lib")
#pragma comment (lib, "d3d12.lib")
#pragma comment (lib, "d3dcompiler.lib")

#include <algorithm>

#define SAFE_RELEASE(b) if (b){ (b)->Release(); b = nullptr; }

// Pick the AMD or Nvidia discrete GPU on laptops with an integrated GPU
extern "C"
{
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
	__declspec(dllexport) DWORD NvOptimusEnablement = 1;
}

struct SDescriptorHeap
{
	ID3D12DescriptorHeap* m_DescHeap;
	D3D12_CPU_DESCRIPTOR_HANDLE m_CPUDescStart;
	D3D12_GPU_DESCRIPTOR_HANDLE m_GPUDescStart;
	uint m_DescSize;

	IDAllocator m_Allocator;
	uint m_Size;
	void Init(ID3D12Device* d3d_device, D3D12_DESCRIPTOR_HEAP_TYPE heap_type, uint size, bool shader_visible)
	{
		m_Allocator.Init(size - 1);

		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.Type = heap_type;
		desc.NumDescriptors = size;
		desc.Flags = shader_visible? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		d3d_device->CreateDescriptorHeap(&desc, __uuidof(ID3D12DescriptorHeap), (void**) &m_DescHeap);

		m_CPUDescStart = m_DescHeap->GetCPUDescriptorHandleForHeapStart();
		m_GPUDescStart = m_DescHeap->GetGPUDescriptorHandleForHeapStart();
		m_DescSize = d3d_device->GetDescriptorHandleIncrementSize(desc.Type);
		m_Size = size;
	}
	void Reset()
	{
		ASSERT(m_Allocator.GetAvailableIDs() <= m_Size);
		m_Allocator.Clear();
	}

	void Destroy()
	{
		m_DescHeap->Release();

		ASSERT(m_Allocator.GetAvailableIDs() <= m_Size);
		m_Allocator.Clear();
	}

	uint Allocate(uint count)
	{
		uint index;
		bool res = m_Allocator.CreateRangeID(index, count);
		ASSERT(res);

		return index;
	}

	void Release(uint offset, uint count)
	{
		bool res = m_Allocator.DestroyRangeID(offset, count);
		ASSERT(res);
	}

	inline D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(uint offset) const
	{
		D3D12_CPU_DESCRIPTOR_HANDLE handle = m_CPUDescStart;
		handle.ptr += offset * m_DescSize;
		return handle;
	}

	inline D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(uint offset) const
	{
		D3D12_GPU_DESCRIPTOR_HANDLE handle = m_GPUDescStart;
		handle.ptr += offset * m_DescSize;
		return handle;
	}

};
struct SlicedBuffer
{
	Buffer	m_Buffer;
	uint8* m_Data;
	uint    m_Cursor;
	uint    m_GPUAddress;
};
typedef  std::map<uint64, RenderPass> RenderPassDictionary;
typedef std::vector<RenderSetup>	RenderSetupVector;

struct SCommandList
{
	RenderSetupVector* m_RenderSetups;
	uint64 m_FenceValue;
	uint64 m_FenceCounter;
	ID3D12Fence* m_Fence;
	HANDLE m_FenceEvent;

	SDescriptorHeap m_ResourceHeap;
	/*SDescriptorHeap m_RTVHeap;
	SDescriptorHeap m_DSVHeap;*/
	SDescriptorHeap m_SamplerHeap;

	SlicedBuffer m_Constants;
	SlicedBuffer m_Staging;

	ID3D12QueryHeap* m_QueryHeap;
	ID3D12Resource* m_QueryBuffer;
	uint             m_QueryOffset;
	const char* m_QueryNames[MAX_QUERY_COUNT];

	ID3D12GraphicsCommandList* m_pD3DList;
	ID3D12CommandAllocator* m_pD3DAllocator;

	void Init(Device device);
	void Destroy(Device device);

	void Begin(Device device, uint size);
	void End(Device device);
};
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
typedef std::unordered_map<SSamplerDesc, D3D12_CPU_DESCRIPTOR_HANDLE> TSamplerDictionary;
struct SDevice
{
	ID3D12Device* m_pD3DDevice;
	ID3D12CommandQueue* m_pD3DQueue;
	IDXGISwapChain3* m_pD3DSwapchain;
	D3D_FEATURE_LEVEL m_FeatureLevel;
	uint64 m_PresentFenceValues[BUFFER_FRAMES];
	uint64 m_PresentFenceCounter;
	ID3D12Fence* m_PresentFence;
	HANDLE m_PresentFenceEvent;

	SDescriptorHeap m_ResourceHeap;
	/*SDescriptorHeap m_RTVHeap;
	SDescriptorHeap m_DSVHeap;*/
	SDescriptorHeap m_SamplerHeap;
	TSamplerDictionary* m_SamplerDictionary;

	SDescriptorHeap m_ResourceDescriptorsCPU;
	SDescriptorHeap m_RTDescriptorsCPU;
	SDescriptorHeap m_DSDescriptorsCPU;
	SDescriptorHeap m_SamplerDescriptorsCPU;
	Context m_MainContext;

	Texture m_BackBuffer[BUFFER_FRAMES];

	HWND m_Window;
	uint32 m_FrameIndex;

	RenderPass m_BackBufferRenderPass;
	SCommandList m_CommandLists[BUFFER_FRAMES];
	RenderPassDictionary* m_RenderpassDictionary;

	float m_TimestampFrequency;
	uint m_MSAASupportMask;

	// Default states
	BlendState m_DefaultBlendState;

	StaticArray<DisplayMode> m_DisplayModes;
};

struct SBufferViewDesc
{
	ItemType m_type;
	SBufferRangeDesc m_range;
	bool operator<(const SBufferViewDesc& other)const
	{
		if (other.m_type != m_type)
		{
			return m_type < other.m_type;
		}
		else
		{
			if (m_range.offset != other.m_range.offset)

				return m_range.offset < other.m_range.offset;
			else
				return m_range.size < other.m_range.size;
		}
	}
};
struct SD3DViewDescriptor
{
	D3D12_CPU_DESCRIPTOR_HANDLE handle;
	SDescriptorHeap* heap;
	uint heapOffset;
};
typedef std::map<SBufferViewDesc, SD3DViewDescriptor> TBufferViewMap;

struct SBuffer
{
	ID3D12Resource* m_Buffer;
	TBufferViewMap* m_Views;
	uint            m_Size;
	HeapType        m_HeapType;
	uint			m_Stride;
	EResourceState m_State;
};


void SCommandList::Init(Device device)
{
	HRESULT hr = device->m_pD3DDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(ID3D12CommandAllocator), (void**)&m_pD3DAllocator);

	ASSERT(SUCCEEDED(hr));
	ID3D12GraphicsCommandList* d3d_cmdlist = nullptr;
	hr = device->m_pD3DDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pD3DAllocator, nullptr, __uuidof(ID3D12GraphicsCommandList*), (void**)&d3d_cmdlist);
	ASSERT(SUCCEEDED(hr));

	// D3D dumbfuckery
	d3d_cmdlist->Close();

	D3D12_QUERY_HEAP_DESC desc = {};
	desc.Type = D3D12_QUERY_HEAP_TYPE_TIMESTAMP;
	desc.Count = MAX_QUERY_COUNT;
	hr = device->m_pD3DDevice->CreateQueryHeap(&desc, __uuidof(ID3D12QueryHeap), (void**)&m_QueryHeap);
	ASSERT(SUCCEEDED(hr));

	D3D12_HEAP_PROPERTIES heap_prop = {};
	heap_prop.Type = D3D12_HEAP_TYPE_READBACK;

	D3D12_RESOURCE_DESC bufDesc = {};
	bufDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	bufDesc.Width = MAX_QUERY_COUNT * sizeof(uint64);
	bufDesc.Height = 1;
	bufDesc.DepthOrArraySize = 1;
	bufDesc.MipLevels = 1;
	bufDesc.SampleDesc.Count = 1;
	bufDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	D3D12_RESOURCE_STATES resource_state = D3D12_RESOURCE_STATE_COPY_DEST;

	ID3D12Resource* buffer = nullptr;
	hr = device->m_pD3DDevice->CreateCommittedResource(&heap_prop, D3D12_HEAP_FLAG_NONE, &bufDesc, resource_state, nullptr, __uuidof(ID3D12Resource), (void**)&m_QueryBuffer);
	ASSERT(SUCCEEDED(hr));

	device->m_pD3DDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), (void**)&m_Fence);
	m_FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	m_FenceCounter = m_FenceValue = 0;

	m_ResourceHeap.Init(device->m_pD3DDevice, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 256, true);
	m_SamplerHeap.Init(device->m_pD3DDevice, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, 64, true);

	SBufferParams cb(1024 * 1024, HEAP_UPLOAD, CONSTANT_BUFFER, "ConstantBuffer");
	m_Constants.m_Buffer = CreateBuffer(device, cb);
	m_Constants.m_Cursor = 0;
	m_Constants.m_Data = nullptr;

}

void SCommandList::Destroy(Device device)
{
	// Wait here?
	m_Fence->Release();
	CloseHandle(m_FenceEvent);
	SAFE_RELEASE(m_QueryBuffer);
	SAFE_RELEASE(m_QueryHeap);
	SAFE_RELEASE(m_pD3DAllocator);
	DestroyBuffer(device, m_Staging.m_Buffer);
	DestroyBuffer(device, m_Constants.m_Buffer);
	SAFE_RELEASE(m_pD3DList);

	// Destroy heaps
	m_SamplerHeap.Destroy();
	m_ResourceHeap.Destroy();
}
void ResetViews(Buffer buffer);
void SCommandList::Begin(Device device, uint size)
{
	
	// If the previous cmdBuf has not finished yet, wait until it's done.
	if (m_Fence->GetCompletedValue() < m_FenceValue)
	{
		m_Fence->SetEventOnCompletion(m_FenceValue, m_FenceEvent);
		WaitForSingleObjectEx(m_FenceEvent, INFINITE, FALSE);
	}

	m_pD3DAllocator->Reset();
	m_pD3DList->Reset(m_pD3DAllocator, nullptr);

	m_SamplerHeap.Reset();
	m_ResourceHeap.Reset();

	ResetViews(m_Constants.m_Buffer);
	m_Constants.m_Cursor = 0;
	for (auto& fb : *m_RenderSetups)
		DestroyRenderSetup(device, fb);
	m_RenderSetups->clear();

	uint curr_size = m_Staging.m_Buffer ? m_Staging.m_Buffer->m_Size : 0;
	if (size != curr_size)
	{
		SBufferParams params(size, HEAP_UPLOAD, VERTEX_BUFFER | INDEX_BUFFER | CONSTANT_BUFFER, "UploadBuffer");
		m_Staging.m_Buffer = CreateBuffer(device, params);
		m_Staging.m_Cursor = 0;
		m_Staging.m_Data = nullptr;
	}



	m_QueryOffset = 0;
}
void SCommandList::End(Device device)
{
	++m_FenceCounter;
	device->m_pD3DQueue->Signal(m_Fence, m_FenceCounter);
	// Save the fence value for this frame.
	m_FenceValue = m_FenceCounter;

	if (m_Staging.m_Data)
	{
		D3D12_RANGE range = { 0, m_Staging.m_Cursor };
		ResetViews(m_Staging.m_Buffer);
		m_Staging.m_Buffer->m_Buffer->Unmap(0, &range);
		m_Staging.m_Data = nullptr;
		m_Staging.m_Cursor = 0;
	}
	if (m_Constants.m_Data)
	{
		D3D12_RANGE range = { 0, m_Constants.m_Cursor };
		ResetViews(m_Constants.m_Buffer);
		m_Constants.m_Buffer->m_Buffer->Unmap(0, &range);
		m_Constants.m_Data = nullptr;
		m_Constants.m_Cursor = 0;
	}
	m_pD3DList->Close();
}
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
};
uint32 AllocateBufferSlice(Device device, SlicedBuffer& buffer, uint size, uint alignment)
{
	uint aligned_offset = (buffer.m_Cursor + alignment - 1) & ~(alignment - 1);

	if (aligned_offset + size > buffer.m_Buffer->m_Size)
	{
		// Reallocate
		ASSERT(false);
	}

	if (buffer.m_Data == nullptr)
	{
		buffer.m_GPUAddress = buffer.m_Buffer->m_Buffer->GetGPUVirtualAddress();

		D3D12_RANGE range = { 0, 0 };
		HRESULT hr = buffer.m_Buffer->m_Buffer->Map(0, &range, (void**)&buffer.m_Data);
		ASSERT(SUCCEEDED(hr));
	}

	buffer.m_Cursor = aligned_offset + size;

	return aligned_offset;
}
uint32 AllocateConstantsSlice(Context context, uint size)
{
	Device device = GetDevice(context);
	uint32 offset = AllocateBufferSlice(device, context->m_CmdList->m_Constants, size, 16/*device->m_UniformBufferAlignment*/);
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


static const DXGI_FORMAT g_Formats[] =
{
	DXGI_FORMAT_UNKNOWN,

	DXGI_FORMAT_R8_UNORM,
	DXGI_FORMAT_R8_UINT,

	DXGI_FORMAT_R8G8_UNORM,
	DXGI_FORMAT_R8G8_UINT,

	DXGI_FORMAT_R8G8B8A8_UNORM,
	DXGI_FORMAT_R8G8B8A8_UINT,

	DXGI_FORMAT_R16_UNORM,
	DXGI_FORMAT_R16_UINT,
	DXGI_FORMAT_R16_FLOAT,

	DXGI_FORMAT_R16G16_UNORM,
	DXGI_FORMAT_R16G16_UINT,
	DXGI_FORMAT_R16G16_FLOAT,

	DXGI_FORMAT_R16G16B16A16_UNORM,
	DXGI_FORMAT_R16G16B16A16_UINT,
	DXGI_FORMAT_R16G16B16A16_FLOAT,

	DXGI_FORMAT_R32_UINT,
	DXGI_FORMAT_R32_FLOAT,

	DXGI_FORMAT_R32G32_UINT,
	DXGI_FORMAT_R32G32_FLOAT,

	DXGI_FORMAT_R32G32B32A32_UINT,
	DXGI_FORMAT_R32G32B32A32_FLOAT,

	DXGI_FORMAT_BC1_UNORM,
	DXGI_FORMAT_BC2_UNORM,
	DXGI_FORMAT_BC3_UNORM,
	DXGI_FORMAT_BC4_UNORM,
	DXGI_FORMAT_BC5_UNORM,
	DXGI_FORMAT_BC7_UNORM,

	DXGI_FORMAT_D16_UNORM,
};
static_assert(elementsof(g_Formats) == IMGFMT_D16 + 1, "g_Formats incorrect length");

static const DXGI_FORMAT g_AttribFormats[] =
{
	DXGI_FORMAT_R32_FLOAT,
	DXGI_FORMAT_R32G32_FLOAT,
	DXGI_FORMAT_R32G32B32_FLOAT,
	DXGI_FORMAT_R32G32B32A32_FLOAT,

	DXGI_FORMAT_R16G16_FLOAT,
	DXGI_FORMAT_R16G16B16A16_FLOAT,

	DXGI_FORMAT_R32_UINT,
	DXGI_FORMAT_R32G32_UINT,
	DXGI_FORMAT_R32G32B32_UINT,
	DXGI_FORMAT_R32G32B32A32_UINT,
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
	TextureType m_dimension;

	TexViewUsage m_viewType;
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

typedef std::map<uint32, SD3DViewDescriptor> TViewMap;
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
	ID3D12Resource* m_Texture;
	STextureSubresource** m_Subresources;
	TextureSubresourceDictionary* m_SubresourceDictionary;// to prevent range duplicates
	uint m_Width;
	uint m_Height;
	uint m_Depth;
	uint m_Slices;
	uint m_MipLevels;
	TextureType m_Type;
	ImageFormat m_Format;
};


struct SResourceTable
{
	SDescriptorHeap* m_Heap;
	uint m_Offset;
	uint m_Size;
};

struct SSamplerTable
{
	SDescriptorHeap* m_Heap;
	uint m_Offset;
	uint m_Count;
};

union SRenderPassDescKey
{
	SRenderPassDesc desc;
	uint64 combo;
};

struct SRenderPass
{
	SRenderPassDesc justDesc;
};

struct SRenderSetup
{
	uint m_Width;
	uint m_Height;
	RenderPass m_Pass;
	uint m_ColorBufferCount;
	D3D12_CPU_DESCRIPTOR_HANDLE m_ColorTargets[MAX_COLOR_TARGETS];
	D3D12_CPU_DESCRIPTOR_HANDLE m_DepthTarget;
};

struct SRootSignature
{
	//helper struct
	struct SRootSlot
	{
		ItemType m_Type;
		uint32   m_Size;
		uint32   m_Offset;
		SResourceTableItem* m_Items;
	};

	uint32 m_SlotCount;
	SRootSlot* m_Slots;

	ID3D12RootSignature* m_Root;
};

struct SPipeline
{
	ID3D12PipelineState* m_Pipeline;
	D3D12_PRIMITIVE_TOPOLOGY m_PrimitiveTopology;
};

struct SBlendState
{
	D3D12_BLEND_DESC m_Desc;
};

struct SVertexSetup
{
	D3D12_INDEX_BUFFER_VIEW  m_IBView;
	D3D12_VERTEX_BUFFER_VIEW m_VBView;
};


static bool CreateBackBufferSetups(Device device, uint width, uint height)
{
	for (uint i = 0; i < BUFFER_FRAMES; i++)
	{
		ID3D12Resource* back_buffer = nullptr;
		if (FAILED(device->m_pD3DSwapchain->GetBuffer(i, __uuidof(ID3D12Resource), (void**)&back_buffer)))
		{
			ErrorMsg("Couldn't create back-buffer");
			return false;
		}
		back_buffer->SetName(L"BackBuffer");

		STexture* texture = new STexture();
		memset(texture, 0, sizeof(STexture));
		texture->m_Texture = back_buffer;
		texture->m_Width     = width;
		texture->m_Height    = height;
		texture->m_Depth     = 1;
		texture->m_Slices    = 1;
		texture->m_MipLevels = 1;
		texture->m_Type      = TEX_2D;
		texture->m_Format    = IMGFMT_RGBA8;

		device->m_BackBuffer[i] = texture;
	}

	return true;
}

static void DestroyBackBufferSetups(Device device)
{
	for (uint i = 0; i < BUFFER_FRAMES; i++)
	{
		DestroyTexture(device, device->m_BackBuffer[i]);
	}
}


// For sorting display modes
bool operator < (const DXGI_MODE_DESC &a, const DXGI_MODE_DESC &b)
{
	int ratio_diff = GetAspectRatio(a.Width, a.Height) - GetAspectRatio(b.Width, b.Height);

	if (ratio_diff < 0)
		return true;
	if (ratio_diff > 0)
		return false;

	return a.Width < b.Width;
}

static void SetD3DName(ID3D12Object* object, const char* name)
{
#ifdef DEBUG
	if (name)
	{
		// Convert to wchar_t
		size_t len = strlen(name);

		// Workaround for Windows 1903 bug with short strings
		size_t new_len = len > 4ULL? len : 4ULL;

		wchar_t* str = (wchar_t*) alloca((new_len + 1) * sizeof(wchar_t));
		size_t i = 0;
		for (; i < len; ++i)
			str[i] = name[i];
		for (; i < new_len; ++i)
			str[i] = ' ';
		str[i] = 0;
		object->SetName(str);
	}
#endif
}

Device CreateDevice(DeviceParams& params)
{
	HINSTANCE inst = GetModuleHandle(nullptr);

	DWORD style = WS_OVERLAPPEDWINDOW;
	int x = 0;
	int y = 0;
	int w = params.m_FullscreenWidth;
	int h = params.m_FullscreenHeight;

	if (!params.m_Fullscreen)
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

		// Center window on screen
		RECT workarea;
		SystemParametersInfo(SPI_GETWORKAREA, 0, &workarea, 0);
		x += (workarea.left + workarea.right  - w) / 2;
		y += (workarea.top  + workarea.bottom - h) / 2;
	}

	wchar_t title[128];
	wsprintf(title, L"%s (%ux%u)", params.m_Title, params.m_Width, params.m_Height);


	HWND hwnd = CreateWindowEx(0, L"Humus", title, style, x, y, w, h, HWND_DESKTOP, nullptr, inst, nullptr);


	UINT flags = 0;
#ifdef DEBUG
	flags |= DXGI_CREATE_FACTORY_DEBUG;

	ID3D12Debug* d3d_debug = nullptr;
	if (SUCCEEDED(D3D12GetDebugInterface(__uuidof(ID3D12Debug), (void**) &d3d_debug)))
	{
		d3d_debug->EnableDebugLayer();
		d3d_debug->Release();
	}
#endif

	IDXGIFactory2* dxgi_factory = nullptr;
	if (FAILED(CreateDXGIFactory2(flags, __uuidof(IDXGIFactory2), (void **) &dxgi_factory)))
	{
		ErrorMsg("Couldn't create DXGIFactory");
		return false;
	}

	IDXGIAdapter1* dxgi_adapter = nullptr;
	int last_device_score = -1;

	{
		// TODO: Investigate if EnumAdapterByGpuPreference() is a better choice
		uint i = 0;
		IDXGIAdapter1* adapter;
		while (dxgi_factory->EnumAdapters1(i, &adapter) != DXGI_ERROR_NOT_FOUND)
		{
			DXGI_ADAPTER_DESC1 desc;
			adapter->GetDesc1(&desc);

			// Primarily prioritize Flags in NONE, REMOTE, SOFTWARE order. Secondly, prioritize AMD or NVIDIA over other vendors (sorry Intel)
			int device_score = ((~desc.Flags & 0x3) << 1) | ((desc.VendorId == 0x1002 || desc.VendorId == 0x10de)? 1 : 0);

			if (device_score > last_device_score)
			{
				if (dxgi_adapter)
					dxgi_adapter->Release();
				dxgi_adapter = adapter;
				last_device_score = device_score;
			}
			else
			{
				adapter->Release();
			}

			++i;
		}
	}

	if (!dxgi_adapter)
	{
		ErrorMsg("No adapters found");
		return nullptr;
	}


/*
	LARGE_INTEGER version;
	dxgi_adapter->CheckInterfaceSupport(__uuidof(ID3D10Device), &version);

	char driver[256];
	sprintf(driver, "%u.%u.%u.%u", version.HighPart >> 16, version.HighPart & 0xFFFF, version.LowPart >> 16, version.LowPart & 0xFFFF);
*/

	ASSERT(dxgi_factory->IsCurrent());

	// Create device
	D3D_FEATURE_LEVEL feature_level = D3D_FEATURE_LEVEL_11_0;

	ID3D12Device* d3d_device = nullptr;
	if (FAILED(D3D12CreateDevice(dxgi_adapter, feature_level, __uuidof(ID3D12Device), (void**) &d3d_device)))
	{
		ErrorMsg("Device creation failed");
		return nullptr;
	}

	D3D12_COMMAND_QUEUE_DESC queue_desc = {};
	queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	ID3D12CommandQueue* d3d_queue = nullptr;
	d3d_device->CreateCommandQueue(&queue_desc, __uuidof(ID3D12CommandQueue), (void**) &d3d_queue);
	SetD3DName(d3d_queue, "Graphics");
	

	DXGI_SWAP_CHAIN_DESC1 sc_desc = {};
	sc_desc.Width  = params.m_Width;
	sc_desc.Height = params.m_Height;
	sc_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sc_desc.SampleDesc.Count = 1;
	sc_desc.BufferUsage = DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT;// | DXGI_USAGE_SHADER_INPUT;
	sc_desc.BufferCount = 2;
	sc_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	sc_desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

	DXGI_SWAP_CHAIN_FULLSCREEN_DESC fs_desc = {};
	fs_desc.Windowed = !params.m_Fullscreen;


	// Create swap-chain
	IDXGISwapChain1* swap_chain1 = nullptr;
	if (FAILED(dxgi_factory->CreateSwapChainForHwnd(d3d_queue, hwnd, &sc_desc, &fs_desc, nullptr, &swap_chain1)))
	{
		ErrorMsg("Couldn't create swapchain");
		return nullptr;
	}

	IDXGISwapChain3* swap_chain = nullptr;
	HRESULT hr = swap_chain1->QueryInterface(__uuidof(IDXGISwapChain3), (void**) &swap_chain);
	swap_chain1->Release();
	if (FAILED(hr))
	{
		ErrorMsg("Couldn't create swapchain");
		return nullptr;
	}

	IDXGIOutput* dxgi_output = nullptr;
	hr = swap_chain->GetContainingOutput(&dxgi_output);

	

	// TODO: Allocate later ...
	Device device = new SDevice;
	memset(device, 0, sizeof(SDevice));
	device->m_Window = hwnd;
	device->m_MSAASupportMask = 1;
	device->m_RenderpassDictionary = new RenderPassDictionary;//for further impl of dz12 renderpass
	device->m_SamplerDictionary = new TSamplerDictionary;
	device->m_ResourceHeap.Init(device->m_pD3DDevice, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 256, true);
	device->m_SamplerHeap.Init(device->m_pD3DDevice, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, 64, true);

	device->m_ResourceDescriptorsCPU.Init(device->m_pD3DDevice, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 256 * 16, false);
	device->m_SamplerDescriptorsCPU.Init(device->m_pD3DDevice, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, 64, false);
	device->m_RTDescriptorsCPU.Init(device->m_pD3DDevice, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 64, false);
	device->m_DSDescriptorsCPU.Init(device->m_pD3DDevice, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 64, false);
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS levels = {};
	levels.Format = sc_desc.Format;
	for (levels.SampleCount = 2; levels.SampleCount <= 8; levels.SampleCount *= 2)
	{
		if (SUCCEEDED(d3d_device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &levels, sizeof(levels))))
		{
			if (levels.NumQualityLevels > 0)
				device->m_MSAASupportMask |= levels.SampleCount;
		}
	}


	uint display_mode_count = 0;
	if (dxgi_output)
	{
		if (SUCCEEDED(dxgi_output->GetDisplayModeList(sc_desc.Format, 0, &display_mode_count, nullptr)))
		{
			DXGI_MODE_DESC* display_modes = new DXGI_MODE_DESC[display_mode_count];

			if (SUCCEEDED(dxgi_output->GetDisplayModeList(sc_desc.Format, 0, &display_mode_count, display_modes)))
			{
				// Sort display modes
				std::sort(display_modes, display_modes + display_mode_count);

				// Remove duplicates (we only care about resolution)
				uint curr = 0;
				for (uint m = 1; m < display_mode_count; m++)
				{
					if (display_modes[m].Width  != display_modes[curr].Width ||
						display_modes[m].Height != display_modes[curr].Height)
					{
						++curr;
						display_modes[curr] = display_modes[m];
					}
				}

				uint count = curr + 1;

				// Store displaymodes
				device->m_DisplayModes.SetCapacity(count);
				for (uint m = 0; m < count; m++)
				{
					device->m_DisplayModes[m].m_Width  = display_modes[m].Width;
					device->m_DisplayModes[m].m_Height = display_modes[m].Height;
				}
			}

			delete[] display_modes;
		}

		dxgi_output->Release();
	}

	// We'll handle Alt-Enter ourselves thank you very much ...
	dxgi_factory->MakeWindowAssociation(device->m_Window, DXGI_MWA_NO_WINDOW_CHANGES | DXGI_MWA_NO_ALT_ENTER);

	dxgi_adapter->Release();
	dxgi_factory->Release();


	device->m_pD3DDevice = d3d_device;
	device->m_pD3DQueue = d3d_queue;
	device->m_pD3DSwapchain = swap_chain;
	device->m_FeatureLevel = feature_level;
	device->m_pD3DDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), (void**)&device->m_PresentFence);
	device->m_PresentFenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	device->m_PresentFenceCounter = 0;

	for (uint i = 0; i < BUFFER_FRAMES; i++)
	{
		device->m_CommandLists[i].Init(device);
		device->m_PresentFenceValues[i] = 0;
	}

	device->m_MainContext = CreateContext(device, false);

	device->m_BackBufferRenderPass = CreateRenderPass(device, IMGFMT_RGBA8, IMGFMT_INVALID, FLAG_NONE);//warning

	if (!CreateBackBufferSetups(device, params.m_Width, params.m_Height))
		return false;


	device->m_DefaultBlendState = CreateBlendState(device, BF_ONE, BF_ZERO, BM_ADD, 0xF, false);



	uint64 freq = 0;
	d3d_queue->GetTimestampFrequency(&freq);
	device->m_TimestampFrequency = (float) (1000.0 / (double) freq);

	return device;
}
void DestroyRenderPass(Device device, RenderPass& render_pass);
void DestroyDevice(Device& device)
{
	if (!device)
		return;

	// Wait here?
	device->m_PresentFence->Release();
	CloseHandle(device->m_PresentFenceEvent);

	DestroyBlendState(device, device->m_DefaultBlendState);

	DestroyBackBufferSetups(device);

	DestroyRenderPass(device, device->m_BackBufferRenderPass);

	DestroyContext(device, device->m_MainContext);

	for (uint i = 0; i < BUFFER_FRAMES; i++)
		device->m_CommandLists[i].Destroy(device);

	// Destroy heaps
	device->m_SamplerHeap.Destroy();
	device->m_ResourceHeap.Destroy();

	device->m_SamplerDescriptorsCPU.Destroy();
	device->m_ResourceDescriptorsCPU.Destroy();
	device->m_RTDescriptorsCPU.Destroy();
	device->m_DSDescriptorsCPU.Destroy();

	// Destroy device and swapchain
	if (device->m_pD3DSwapchain)
	{
		device->m_pD3DSwapchain->SetFullscreenState(FALSE, nullptr);

		device->m_pD3DSwapchain->Release();
		device->m_pD3DSwapchain = nullptr;
	}

	device->m_pD3DQueue->Release();

	delete device->m_RenderpassDictionary;
	delete device->m_SamplerDictionary;
	for (auto& pass : *device->m_RenderpassDictionary)
	{
		DestroyRenderPass(device, pass.second);
	}
	if (device->m_pD3DDevice)
	{
		ULONG ref_count = device->m_pD3DDevice->Release();

#ifdef DEBUG
		if (ref_count)
		{
			DebugString("There are %d unreleased references left on the D3D device!", ref_count);

			ID3D12DebugDevice* debug = nullptr;
			if (SUCCEEDED(device->m_pD3DDevice->QueryInterface<ID3D12DebugDevice>(&debug)))
			{
				debug->ReportLiveDeviceObjects(D3D12_RLDO_DETAIL);
				debug->Release();
			}

		}
#else
		(void) ref_count; // avoid warning
#endif
		device->m_pD3DDevice = nullptr;
	}

	delete device;
	device = nullptr;
}

void Present(Device device, bool vsync)
{
	device->m_pD3DSwapchain->Present(vsync? 1 : 0, 0);


	++device->m_PresentFenceCounter;
	device->m_pD3DQueue->Signal(device->m_PresentFence, device->m_PresentFenceCounter);
	// Save the fence value for this frame.
	device->m_PresentFenceValues[device->m_FrameIndex] = device->m_PresentFenceCounter;

	device->m_FrameIndex = (device->m_FrameIndex + 1) % BUFFER_FRAMES;

	// If the previous frame has not finished yet, wait until it's done.
	if (device->m_PresentFence->GetCompletedValue() < device->m_PresentFenceValues[device->m_FrameIndex])
	{
		device->m_PresentFence->SetEventOnCompletion(device->m_PresentFenceValues[device->m_FrameIndex], device->m_PresentFenceEvent);
		WaitForSingleObjectEx(device->m_PresentFenceEvent, INFINITE, FALSE);
	}

/*
	const UINT64 currentFenceValue = device->m_FenceValues[device->m_FrameIndex];
	device->m_pD3DQueue->Signal(device->m_Fence, currentFenceValue);

	device->m_FrameIndex = (device->m_FrameIndex + 1) % BUFFER_FRAMES;

	// If the next frame is not ready to be rendered yet, wait until it is ready.
	if (device->m_Fence->GetCompletedValue() < device->m_FenceValues[device->m_FrameIndex])
	{
		device->m_Fence->SetEventOnCompletion(device->m_FenceValues[device->m_FrameIndex], device->m_FenceEvent);
		WaitForSingleObjectEx(device->m_FenceEvent, INFINITE, FALSE);
	}

	// Set the fence value for the next frame.
	device->m_FenceValues[device->m_FrameIndex] = currentFenceValue + 1;
*/

}

HWND GetWindow(Device device)
{
	return device->m_Window;
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
void IterateRootSignature(SRootSignature* root, void* _callback, void* receiver)
{
	RootCallback callback = (RootCallback)_callback;
	for (int slot = 0; slot < root->m_SlotCount; slot++)
	{
		for (int i = 0; i < root->m_Slots[slot].m_Size; i++)
		{
			callback(root->m_Slots[slot].m_Items[i].m_Type, slot, i, i == 0 ? root->m_Slots[slot].m_Size : 0, receiver);
		}
	}
}
void SetClearColors(Context context, const float clear_color[4], const float clear_depth[2])
{
	memcpy(&context->m_ClearColor[0], &clear_color[0], 4 * sizeof(float));
	memcpy(&context->m_ClearDepth[0], &clear_depth[0], 2 * sizeof(float));
}
void SetRenderTarget(Context context, SResourceDesc target, uint slot)
{
	Device device = context->m_Device;
	ASSERT(slot < MAX_COLOR_TARGETS);
	context->m_CurrentFrameBufferDesc.m_ColorTargets[slot] = target;
}
void SetDepthTarget(Context context, SResourceDesc depth)
{
	context->m_CurrentFrameBufferDesc.m_DepthTarget = depth;
}

void EndRenderPass(Context context)
{
	EndRenderPass(context, context->m_CurrentRenderSetup);
}

RenderPass GetCurrentRenderPass(Context context)
{
	return context->m_CurrentRenderSetup->m_Pass;
}
Texture GetBackBuffer(Device device, uint buffer)
{
	return device->m_BackBuffer[buffer];
}

ImageFormat GetBackBufferFormat(Device device)
{
	return IMGFMT_RGBA8;
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
	return device->m_pD3DSwapchain->GetCurrentBackBufferIndex();
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

	DestroyBackBufferSetups(device);

	DXGI_MODE_DESC desc;
	desc.RefreshRate.Numerator   = 0;
	desc.RefreshRate.Denominator = 0;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	desc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	if (params.m_Fullscreen)
	{
		DebugString("SetDisplayMode(%d, %d, 1);", params.m_FullscreenWidth, params.m_FullscreenHeight);

		desc.Width  = params.m_FullscreenWidth;
		desc.Height = params.m_FullscreenHeight;

		device->m_pD3DSwapchain->ResizeTarget(&desc);
		device->m_pD3DSwapchain->SetFullscreenState(TRUE, nullptr);
	}
	else
	{
		DebugString("SetDisplayMode(%d, %d, 0);", params.m_WindowedWidth, params.m_WindowedHeight);

		//DXGI_MODE_DESC desc;
		desc.Width  = params.m_WindowedWidth;
		desc.Height = params.m_WindowedHeight;

		/*
		RECT rect;
		rect.left = 0;
		rect.top  = 0;
		rect.right  = params.m_WindowedWidth;
		rect.bottom = params.m_WindowedHeight;
		AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW, FALSE, 0);

		int x = rect.left;
		int y = rect.top;
		int w = rect.right - rect.left;
		int h = rect.bottom - rect.top;

		RECT workarea;
		SystemParametersInfo(SPI_GETWORKAREA, 0, &workarea, 0);

		x += (workarea.left + workarea.right  - w) / 2;
		y += (workarea.top  + workarea.bottom - h) / 2;

		//sprintf(str, "MoveWindow(%d, %d);\n", w, h);
		//OutputDebugStringA(str);

		MoveWindow(device->m_Window, x, y, w, h, TRUE);
		*/

		device->m_pD3DSwapchain->SetFullscreenState(FALSE, nullptr);
		device->m_pD3DSwapchain->ResizeTarget(&desc);
	}

	Resize(device, params, desc.Width, desc.Height);

	CreateBackBufferSetups(device, desc.Width, desc.Height);

	params.m_ModeChangeInProgress = false;
}

// This function is called in response to WM_SIZE messages, including synchronously during mode switch
bool Resize(Device device, DeviceParams& params, int width, int height)
{
	if (!params.m_ModeChangeInProgress)
		DestroyBackBufferSetups(device);

	DebugString("Resize(%d, %d);", width, height);

	device->m_pD3DSwapchain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING);

	UpdateWindow(device, params, width, height);

	if (!params.m_ModeChangeInProgress)
		CreateBackBufferSetups(device, width, height);

	return true;
}

bool SupportsMSAAMode(Device device, uint samples)
{
	return (device->m_MSAASupportMask & samples) != 0;
}


BlendState GetDefaultBlendState(Device device)
{
	return device->m_DefaultBlendState;
}


Context CreateContext(Device device, bool deferred)
{

	// Allocate on aligned address
	Context context = (SContext*) _mm_malloc(sizeof(SContext), 16);
	memset(context, 0, sizeof(SContext));

	context->m_Device = device;

	return context;
}

void DestroyContext(Device device, Context& context)
{
	if (!context)
		return;

	_mm_free(context);

	context = nullptr;
}

Device GetDevice(Context context)
{
	return context->m_Device;
}

Texture CreateTexture(Device device, const STextureParams& params)
{
	D3D12_HEAP_PROPERTIES heap_prop = {};
	heap_prop.Type = D3D12_HEAP_TYPE_DEFAULT;

	D3D12_HEAP_FLAGS flags = D3D12_HEAP_FLAG_NONE;

	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension        = (params.m_Type <= TEX_1D_ARRAY)? D3D12_RESOURCE_DIMENSION_TEXTURE1D : (params.m_Type <= TEX_CUBE_ARRAY)? D3D12_RESOURCE_DIMENSION_TEXTURE2D : D3D12_RESOURCE_DIMENSION_TEXTURE3D;
	desc.Width            = params.m_Width;
	desc.Height           = params.m_Height;
	desc.DepthOrArraySize = (uint16) ((params.m_Type == TEX_3D)? params.m_Depth : params.m_Slices);
	desc.MipLevels        = (uint16) params.m_MipCount;
	desc.Format           = g_Formats[params.m_Format];
	desc.SampleDesc.Count = params.m_MSAASampleCount;
	desc.Layout           = D3D12_TEXTURE_LAYOUT_UNKNOWN;

	if (params.m_RenderTarget)
		desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	if (params.m_DepthTarget)
	{
		desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
		if (!params.m_ShaderResource)
			desc.Flags |= D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE;
	}
	if (params.m_UnorderedAccess)
		desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;


	D3D12_RESOURCE_STATES resource_state = D3D12_RESOURCE_STATE_COPY_DEST;
	D3D12_CLEAR_VALUE clear_value = {};

	if (params.m_DepthTarget)
	{
		resource_state = D3D12_RESOURCE_STATE_DEPTH_WRITE;
		clear_value.Format = desc.Format;
		if (params.m_ClearValue)
			clear_value.DepthStencil.Depth = *params.m_ClearValue;
	}
	else if (params.m_RenderTarget)
	{
		resource_state = D3D12_RESOURCE_STATE_RENDER_TARGET;
		if (params.m_ClearValue)
		{
			clear_value.Format = desc.Format;
			memcpy(clear_value.Color, params.m_ClearValue, sizeof(clear_value.Color));
		}
	}



	ID3D12Resource* d3d_texture = nullptr;
	HRESULT hr = device->m_pD3DDevice->CreateCommittedResource(&heap_prop, flags, &desc, resource_state, clear_value.Format? &clear_value : nullptr, __uuidof(ID3D12Resource), (void**) &d3d_texture);
	if (FAILED(hr))
		return nullptr;

	SetD3DName(d3d_texture, params.m_Name);



	Texture texture = new STexture();
	texture->m_Texture = d3d_texture;
	texture->m_Width     = params.m_Width;
	texture->m_Height    = params.m_Height;
	texture->m_Depth     = params.m_Depth;
	texture->m_Slices    = params.m_Slices;
	texture->m_MipLevels = params.m_MipCount;
	texture->m_Type      = params.m_Type;
	texture->m_Format    = params.m_Format;
	return texture;
}
void DestroyTextureSubresource(Device device, STextureSubresource* sub)
{
	if (sub->m_Views)
	{
		for (auto& view : *sub->m_Views)
		{
			view.second.heap->Release(view.second.heapOffset, 1);
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
		if (texture->m_SubresourceDictionary)
		{
			for (auto& it : *texture->m_SubresourceDictionary)
			{
				DestroyTextureSubresource(device, it.second);
			}
		}
		texture->m_Texture->Release();
		
		delete texture;
		texture = nullptr;
	}
}

STextureSubresource* AcquireTextureSubresource(const SResourceDesc& resourceDesc)
{
	ASSERT(resourceDesc.m_Type == RESTYPE_TEXTURE);
	const STextureSubresourceDesc& desc = resourceDesc.m_texRange;
	Texture texture = (Texture)resourceDesc.m_Resource;
	if (!texture->m_Subresources)
	{
		uint size = texture->m_Slices * (texture->m_MipLevels + 1) + 1;
		texture->m_Subresources = new STextureSubresource * [size];
		memset(texture->m_Subresources, 0, sizeof(STextureSubresource*) * size);
		texture->m_SubresourceDictionary = new TextureSubresourceDictionary;
	}

	ASSERT(desc.slice >= 0 || desc.mip == -1);
	uint index = (desc.slice + 1) + (desc.mip >= 0) * (texture->m_MipLevels * desc.slice + desc.mip);

	bool isCube = texture->m_Type == TEX_CUBE || texture->m_Type == TEX_CUBE_ARRAY;

	STextureSubresource*& subResource = texture->m_Subresources[index];
	if (!subResource)
	{
		uint ifCube6 = isCube ? 6 : 1;

		SRangeKey key;
		key.range.baseMipLevel = desc.mip >= 0 ? desc.mip : 0;
		key.range.levelCount = desc.mip >= 0 ? 1 : texture->m_MipLevels;
		key.range.baseArrayLayer = desc.slice >= 0 ? ifCube6 * desc.slice : 0;
		key.range.layerCount = desc.slice >= 0 ? ifCube6 : ifCube6 * texture->m_Slices;

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
		ASSERT(desc.face < 6);
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
EResourceState GetCurrentState(const SResourceDesc& resource)
{
	return resource.m_Type == RESTYPE_TEXTURE ? AcquireTextureSubresource(resource)->m_State : ((Buffer)resource.m_Resource)->m_State;
}
template<class T>
T FillD3DViewDesc(SRange range, TextureType type);
template<class T>
void CreateD3DView(ID3D12Device* device, Texture texture, T* desc, const D3D12_CPU_DESCRIPTOR_HANDLE& handle);
template<class T>
SD3DViewDescriptor AcquireD3DViewInternal(Device device, SDescriptorHeap* heap, STextureSubresource* sub, TexViewUsage usageType, TextureType type)
{

	SView view;
	view.m_viewType = usageType;
	T desc = FillD3DViewDesc<T>(sub->m_Range, type);
	desc.Format = g_Formats[sub->m_Owner->m_Format];

	view.m_dimension = type;

	SViewKey key; key.view = view;
	TViewMap::iterator viewIt = sub->m_Views->find(key.combo);
	if (viewIt != sub->m_Views->end())
		return viewIt->second;
	else
	{
		sub->m_Views->insert(TViewMap::value_type(key.combo, {}));
		viewIt = sub->m_Views->find(key.combo);
		SD3DViewDescriptor& result = viewIt->second;
		result.heap = heap;
		result.heapOffset = heap->Allocate(1);
		result.handle = heap->m_CPUDescStart;
		result.handle.ptr += result.heapOffset * heap->m_DescSize;
		CreateD3DView<T>(device->m_pD3DDevice, sub->m_Owner, &desc, result.handle);
	}
	return viewIt->second;
}
SD3DViewDescriptor AcquireTextureSubresourceView(Device device, const SResourceDesc& resourceDesc, TexViewUsage usageType, TextureViewFlags flags = TextureViewFlags::ViewDefault)
{
	SD3DViewDescriptor result = {};
	ASSERT(resourceDesc.m_Type == RESTYPE_TEXTURE);
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


	SDescriptorHeap* heap = nullptr;

	switch (usageType)
	{
	case SRV:
	{
		heap = &device->m_ResourceDescriptorsCPU;
		result = AcquireD3DViewInternal<D3D12_SHADER_RESOURCE_VIEW_DESC>(device, heap, sub, usageType, type);
	}
	break;
	case UAV:
	{
		heap = &device->m_ResourceDescriptorsCPU;
		result = AcquireD3DViewInternal<D3D12_UNORDERED_ACCESS_VIEW_DESC>(device, heap, sub, usageType, type);
	}
	break;
	case RTV:
	{
		heap = &device->m_RTDescriptorsCPU;
		result = AcquireD3DViewInternal<D3D12_RENDER_TARGET_VIEW_DESC>(device, heap, sub, usageType, type);
	}
	break;
	case DSV:
	{
		heap = &device->m_DSDescriptorsCPU;
		result = AcquireD3DViewInternal<D3D12_DEPTH_STENCIL_VIEW_DESC>(device, heap, sub, usageType, type);
	}
	break;
	default:
		ASSERT(false);
	}
	return result;
}

template<>
void CreateD3DView<D3D12_SHADER_RESOURCE_VIEW_DESC>(ID3D12Device* device, Texture texture, D3D12_SHADER_RESOURCE_VIEW_DESC* desc, const D3D12_CPU_DESCRIPTOR_HANDLE& handle)
{
	device->CreateShaderResourceView(texture->m_Texture, desc, handle);

}
template<>
void CreateD3DView<D3D12_UNORDERED_ACCESS_VIEW_DESC>(ID3D12Device* device, Texture texture, D3D12_UNORDERED_ACCESS_VIEW_DESC* desc, const D3D12_CPU_DESCRIPTOR_HANDLE& handle)
{
	device->CreateUnorderedAccessView(texture->m_Texture, nullptr, desc, handle);
}
template<>
void CreateD3DView<D3D12_RENDER_TARGET_VIEW_DESC>(ID3D12Device* device, Texture texture, D3D12_RENDER_TARGET_VIEW_DESC* desc, const D3D12_CPU_DESCRIPTOR_HANDLE& handle)
{
	device->CreateRenderTargetView(texture->m_Texture, desc, handle);
}
template<>
void CreateD3DView<D3D12_DEPTH_STENCIL_VIEW_DESC>(ID3D12Device* device, Texture texture, D3D12_DEPTH_STENCIL_VIEW_DESC* desc, const D3D12_CPU_DESCRIPTOR_HANDLE& handle)
{
	device->CreateDepthStencilView(texture->m_Texture, desc, handle);
}
SD3DViewDescriptor AcquireBufferView(Device device, Buffer buffer, const SBufferViewDesc& viewDesc)
{
	if (!buffer->m_Views)
		buffer->m_Views = new TBufferViewMap;
	TBufferViewMap::iterator viewIt = buffer->m_Views->find(viewDesc);
	if (viewIt != buffer->m_Views->end())
		return viewIt->second;
	else
	{
		buffer->m_Views->insert(TBufferViewMap::value_type(viewDesc, {}));
		viewIt = buffer->m_Views->find(viewDesc);
		SDescriptorHeap* heap = &device->m_ResourceDescriptorsCPU;
		SD3DViewDescriptor& result = viewIt->second;
		result.heap = heap;
		result.heapOffset = heap->Allocate(1);
		result.handle = heap->m_CPUDescStart;
		result.handle.ptr += result.heapOffset * heap->m_DescSize;
		switch (viewDesc.m_type)
		{
	
		case STRUCTUREDBUFFER:
		{
			ASSERT(buffer->m_Stride);
			D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
			D3D12_BUFFER_SRV& params = desc.Buffer;
			desc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;

			params.FirstElement = viewDesc.m_range.offset / buffer->m_Stride;
			params.NumElements = viewDesc.m_range.size / buffer->m_Stride;
			params.StructureByteStride = buffer->m_Stride;
			device->m_pD3DDevice->CreateShaderResourceView(buffer->m_Buffer, &desc, result.handle);
			break;
		}
		case RWSTRUCTUREDBUFFER:
		{
			ASSERT(buffer->m_Stride);

			D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {};
			desc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
			D3D12_BUFFER_UAV& params = desc.Buffer;
			params.FirstElement = viewDesc.m_range.offset / buffer->m_Stride;
			params.NumElements = viewDesc.m_range.size / buffer->m_Stride;
			params.StructureByteStride = buffer->m_Stride;
			device->m_pD3DDevice->CreateUnorderedAccessView(buffer->m_Buffer, nullptr,&desc, result.handle);

			break;
		}
		case CBV:
			D3D12_CONSTANT_BUFFER_VIEW_DESC desc;
			desc.BufferLocation = buffer->m_Buffer->GetGPUVirtualAddress() + viewDesc.m_range.offset;
			desc.SizeInBytes = viewDesc.m_range.size;
			device->m_pD3DDevice->CreateConstantBufferView(&desc, result.handle);

		default:
			ASSERT(false);
		}
		return viewIt->second;
	}
}
void ResetViews(Buffer buffer)
{
	for (auto& view : *buffer->m_Views)
	{
		view.second.heap->Release(view.second.heapOffset, 1);
	}
}
void UpdateResourceTable(Device device, RootSignature root, uint32 slot, ResourceTable table, const SResourceDesc* resources, uint offset, uint count, const uint8* reflectionFlags)
{
	D3D12_CPU_DESCRIPTOR_HANDLE* updates = StackAlloc<D3D12_CPU_DESCRIPTOR_HANDLE>(count);
	D3D12_CPU_DESCRIPTOR_HANDLE destination = table->m_Heap->m_CPUDescStart;
	destination.ptr += table->m_Offset * table->m_Heap->m_DescSize;

	const SRootSignature::SRootSlot& root_slot = root->m_Slots[slot];
	uint binding = offset;
	uint element = 0;


	for (uint i = offset; i < offset + count; i++)
	{
		const SResourceTableItem& item = root_slot.m_Items[binding];
		SD3DViewDescriptor descriptor;
		switch (item.m_Type)
		{
		case TEXTURE:
		case RWTEXTURE:
		{
			ASSERT(resources[i].m_Type == RESTYPE_TEXTURE);
			Texture texture = (Texture)resources[i].m_Resource;

			TextureViewFlags viewFlags = ViewDefault;
			if (reflectionFlags && (reflectionFlags[i] & Refl_Flag_Array) && !(reflectionFlags[i] & Refl_Flag_Cube)
				&& (texture->m_Type == TEX_CUBE || texture->m_Type == TEX_CUBE_ARRAY))
				viewFlags = CubeAsArray;

			descriptor = AcquireTextureSubresourceView(device, resources[i], item.m_Type == TEXTURE ? TexViewUsage::SRV : TexViewUsage::UAV, viewFlags);
			updates[i] = descriptor.handle;
			break;
		}

		case STRUCTUREDBUFFER:
		case RWSTRUCTUREDBUFFER:
		case CBV:
		{
			Buffer buffer = (Buffer)resources[i].m_Resource;
			SBufferViewDesc viewDesc = { item.m_Type, resources[i].m_bufRange };
			descriptor = AcquireBufferView(device, buffer, viewDesc);
			updates[i] = descriptor.handle;
		}
		default:
			ASSERT(false);
		}

		++element;
		if (element >= root_slot.m_Items[binding].m_NumElements)
		{
			binding++;
			element = 0;
		}
	}
	device->m_pD3DDevice->CopyDescriptorsSimple(count, table->m_Heap->m_CPUDescStart, updates[0], D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

}
ResourceTable CreateResourceTable(Device device, RootSignature root, uint32 slot, const SResourceDesc* resources, uint count, Context onframe)
{
	SDescriptorHeap* heap = onframe? &onframe->m_CmdList->m_ResourceHeap:&device->m_ResourceHeap;
	
	ResourceTable rt = new SResourceTable();
	rt->m_Offset = heap->Allocate(count);
	rt->m_Size = count;
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
		device->m_ResourceHeap.Release(table->m_Offset, table->m_Size);

		delete table;
		table = nullptr;
	}
}
void UpdateSamplerTable(Device device, RootSignature root, uint32 slot, SamplerTable table, const SSamplerDesc* sampler_descs, uint offset, uint count)
{

	D3D12_CPU_DESCRIPTOR_HANDLE* updates = StackAlloc<D3D12_CPU_DESCRIPTOR_HANDLE>(count);
	D3D12_CPU_DESCRIPTOR_HANDLE destination = table->m_Heap->m_CPUDescStart;
	destination.ptr += table->m_Offset * table->m_Heap->m_DescSize;


	static const D3D12_FILTER filters[] =
	{
		D3D12_FILTER_MIN_MAG_MIP_POINT,
		D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT,
		D3D12_FILTER_MIN_MAG_MIP_POINT,
		D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT,
		D3D12_FILTER_MIN_MAG_MIP_LINEAR,
	};


	for (uint i = 0; i < count; i++)
	{
		const SSamplerDesc& sampler_desc = sampler_descs[i];

		D3D12_SAMPLER_DESC desc;
		desc.Filter = (sampler_desc.Aniso > 1) ? D3D12_FILTER_ANISOTROPIC : filters[sampler_desc.Filter];
		desc.AddressU = (sampler_desc.AddressModeU == AM_WRAP) ? D3D12_TEXTURE_ADDRESS_MODE_WRAP : D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		desc.AddressV = (sampler_desc.AddressModeV == AM_WRAP) ? D3D12_TEXTURE_ADDRESS_MODE_WRAP : D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		desc.AddressW = (sampler_desc.AddressModeW == AM_WRAP) ? D3D12_TEXTURE_ADDRESS_MODE_WRAP : D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		desc.MipLODBias = 0.0f;
		desc.MaxAnisotropy = sampler_desc.Aniso;
		desc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		desc.BorderColor[0] = 0;
		desc.BorderColor[1] = 0;
		desc.BorderColor[2] = 0;
		desc.BorderColor[3] = 0;
		desc.MinLOD = 0.0f;
		desc.MaxLOD = (sampler_desc.Filter > FILTER_LINEAR) ? -1 : 0.0f;
		TSamplerDictionary::iterator itSampler = device->m_SamplerDictionary->find(sampler_desc);
		if (itSampler == device->m_SamplerDictionary->end())
		{
			device->m_SamplerDictionary->insert(TSamplerDictionary::value_type(sampler_desc, {}));
			itSampler = device->m_SamplerDictionary->find(sampler_desc);
			itSampler->second = device->m_SamplerHeap.m_CPUDescStart;
			itSampler->second.ptr += device->m_SamplerHeap.Allocate(1)* device->m_SamplerHeap.m_DescSize;
			device->m_pD3DDevice->CreateSampler(&desc, itSampler->second);

		}
		updates[i] = itSampler->second;
	}
	device->m_pD3DDevice->CopyDescriptorsSimple(count, table->m_Heap->m_CPUDescStart, updates[0], D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);


}
SamplerTable CreateSamplerTable(Device device, RootSignature root, uint32 slot, const SSamplerDesc* sampler_descs, uint count)
{
	SDescriptorHeap* heap = &device->m_SamplerHeap;

	SamplerTable st = new SSamplerTable;
	st->m_Offset = heap->Allocate(count);
	st->m_Count = count;
	if (sampler_descs)
	{
		UpdateSamplerTable(device, root, slot, st, sampler_descs, 0, count);
	}
	

	return st;
}

void DestroySamplerTable(Device device, SamplerTable& table)
{
	if (table)
	{
		device->m_SamplerHeap.Release(table->m_Offset, table->m_Count);

		delete table;
		table = nullptr;
	}
}

RenderPass CreateRenderPass(Device device, ImageFormat color_format, ImageFormat depth_format, RenderPassFlags flags, uint msaa_samples)
{
	SRenderPassDesc desc; memset(&desc, 0, sizeof(SRenderPassDesc));
	desc.m_ColorFormats[0] = color_format;
	desc.m_DepthFormat = depth_format;
	desc.m_Flags = flags;
	desc.msaa_samples = 1;
	return AcquireRenderPass(device, desc);
}

RenderPass AcquireRenderPass(Device device, const SRenderPassDesc& desc)//not need now, todo later
{
	SRenderPassDescKey key;
	key.desc = desc;
	auto it = device->m_RenderpassDictionary->find(key.combo);
	if (it != device->m_RenderpassDictionary->end())
	{
		return it->second;
	}
	RenderPass render_pass = new SRenderPass();
	render_pass->justDesc = desc;
	device->m_RenderpassDictionary->insert(RenderPassDictionary::value_type(key.combo, render_pass));
	return render_pass;
}
void DestroyRenderPass(Device device, RenderPass& render_pass)
{
	if (render_pass)
	{
		delete render_pass;
		render_pass = nullptr;
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
	uint32 width = fb.m_ColorTargets[0].m_Resource ? ((Texture)fb.m_ColorTargets[0].m_Resource)->m_Width : ((Texture)fb.m_DepthTarget.m_Resource)->m_Width;
	uint32 height = fb.m_ColorTargets[0].m_Resource ? ((Texture)fb.m_ColorTargets[0].m_Resource)->m_Height : ((Texture)fb.m_DepthTarget.m_Resource)->m_Height;

	RenderSetup setup = new SRenderSetup();
	memset(setup, 0, sizeof(setup));
	setup->m_Width = width;
	setup->m_Height = height;
	setup->m_Pass = render_pass;

	uint i = 0;
	for (SResourceDesc& color_target : fb.m_ColorTargets)
	{
		if (color_target.m_Resource)
		{
			 SD3DViewDescriptor descr = AcquireTextureSubresourceView(device, color_target, TexViewUsage::RTV);
			 setup->m_ColorTargets[i] = descr.handle;
		}
		i++;
	}
	if (fb.m_DepthTarget.m_Resource)
	{
		SD3DViewDescriptor descr = AcquireTextureSubresourceView(device, fb.m_DepthTarget, TexViewUsage::DSV);
		setup->m_DepthTarget = descr.handle;
	}


	return setup;
}


void DestroyRenderSetup(Device device, RenderSetup& setup)
{
	if (setup)
	{
		delete setup;
		setup = nullptr;
	}
}

RootSignature CreateRootSignature(Device device, const SCodeBlob& blob)
{
	const SRoot* root_header = (const SRoot*)blob.m_Code;
	const SResourceTableItem* resource_table_items = nullptr;
	size_t root_slots_size = 0;
	size_t rootheadersize = 0;
	if (root_header->m_NumSlots)
	{
		size_t root_slots_size = sizeof(SRoot) + (root_header->m_NumSlots - 1) * sizeof(SRootSlot);
		uint table_count = 0;
		resource_table_items = (const SResourceTableItem*)((char*)root_header + root_slots_size);

		const uint32 num_slots = root_header->m_NumSlots;
		for (uint32 i = 0; i < num_slots; i++)
		{
			const SRootSlot& slot = root_header->m_Slots[i];
			if (slot.m_Type == RESOURCE_TABLE || slot.m_Type == SAMPLER_TABLE)
			{
				resource_table_items += slot.m_Size;
			}
		}
		rootheadersize = root_slots_size + table_count * sizeof(SResourceTableItem);
	}

	ID3D12RootSignature* root = nullptr;
	HRESULT hr = device->m_pD3DDevice->CreateRootSignature(0, (char*)blob.m_Code + rootheadersize, blob.m_Size - rootheadersize, __uuidof(ID3D12RootSignature), (void **) &root);
	if (FAILED(hr))
		return nullptr;

	RootSignature root_sig = new SRootSignature();
	root_sig->m_Root = root;

	root_sig->m_SlotCount = root_header->m_NumSlots;
	if (root_sig->m_SlotCount)
		root_sig->m_Slots = new SRootSignature::SRootSlot[root_header->m_NumSlots];

	uint32 push_constant_offset = 0;
	uint32 descriptor_table = 0;
	for (uint32 i = 0; i < root_header->m_NumSlots; i++)
	{
		SRootSignature::SRootSlot& slot = root_sig->m_Slots[i];
		const ItemType type = root_header->m_Slots[i].m_Type;

		slot.m_Type = root_header->m_Slots[i].m_Type;
		slot.m_Size = root_header->m_Slots[i].m_Size;
		if (type == CONSTANT)
		{
			slot.m_Offset = push_constant_offset;
			push_constant_offset += slot.m_Size * sizeof(uint32);
		}
		else
		{
			slot.m_Offset = descriptor_table;
			++descriptor_table;
		}

		if (type == RESOURCE_TABLE || type == SAMPLER_TABLE)
		{
			slot.m_Items = new SResourceTableItem[slot.m_Size];
			memcpy(slot.m_Items, resource_table_items, slot.m_Size * sizeof(SResourceTableItem));
			resource_table_items += slot.m_Size;
		}
	}
	return root_sig;
}

void DestroyRootSignature(Device device, RootSignature& root)
{
	if (root)
	{
		root->m_Root->Release();

		delete root;
		root = nullptr;
	}
}

Pipeline CreatePipeline(Device device, const SPipelineParams& params)
{
	if (params.m_RootSignature == nullptr)
		return nullptr;

	D3D12_INPUT_ELEMENT_DESC ia_desc[16];
	ASSERT(params.m_AttribCount < elementsof(ia_desc));

	uint offsets[4] = {};

	for (uint i = 0; i < params.m_AttribCount; i++)
	{
		uint stream = params.m_Attribs[i].Stream;
		AttribFormat format = params.m_Attribs[i].Format;
		ASSERT(stream < elementsof(offsets));

		ia_desc[i].SemanticName = params.m_Attribs[i].Name;
		ia_desc[i].SemanticIndex = 0;
		ia_desc[i].Format = g_AttribFormats[format];
		ia_desc[i].InputSlot = stream;
		ia_desc[i].AlignedByteOffset = offsets[stream];
		ia_desc[i].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
		ia_desc[i].InstanceDataStepRate = 0;

		offsets[stream] += g_AttribSizes[format];
	}

	D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
	desc.pRootSignature = params.m_RootSignature->m_Root;
	desc.VS.pShaderBytecode = params.m_VS.m_Code;
	desc.VS.BytecodeLength  = params.m_VS.m_Size;
	desc.PS.pShaderBytecode = params.m_PS.m_Code;
	desc.PS.BytecodeLength  = params.m_PS.m_Size;

	desc.PrimitiveTopologyType = (D3D12_PRIMITIVE_TOPOLOGY_TYPE) ((params.m_PrimitiveType + 3) >> 1);

	desc.InputLayout.NumElements = params.m_AttribCount;
	desc.InputLayout.pInputElementDescs = params.m_AttribCount? ia_desc : nullptr;

	desc.BlendState.RenderTarget[0].RenderTargetWriteMask = 0xF;
	if (params.m_BlendState)
	{
		desc.BlendState = params.m_BlendState->m_Desc;
	}

	desc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	desc.RasterizerState.CullMode = (D3D12_CULL_MODE)(params.m_CullMode + 1);
	desc.RasterizerState.DepthClipEnable = TRUE;
	desc.RasterizerState.MultisampleEnable = TRUE;

	desc.DepthStencilState.DepthEnable    = params.m_DepthTest;
	desc.DepthStencilState.DepthWriteMask = params.m_DepthWrite? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;
	desc.DepthStencilState.DepthFunc      = (D3D12_COMPARISON_FUNC) (params.m_DepthFunc + 1);

	const uint32 rt_count = params.m_RenderPass->justDesc.m_ColorTargetCount;

	desc.NumRenderTargets = rt_count;
	memcpy(desc.RTVFormats, params.m_RenderPass->justDesc.m_ColorFormats, rt_count * sizeof(DXGI_FORMAT));
	desc.DSVFormat = g_Formats[params.m_RenderPass->justDesc.m_DepthFormat];

	desc.SampleDesc.Count = params.m_RenderPass->justDesc.msaa_samples;

	desc.SampleMask = 0xFFFFFFFF;

	ID3D12PipelineState* pso = nullptr;
	HRESULT hr = device->m_pD3DDevice->CreateGraphicsPipelineState(&desc, __uuidof(ID3D12PipelineState), (void**) &pso);
	if (FAILED(hr))
		return nullptr;

	SetD3DName(pso, params.m_Name);

	Pipeline pipeline = new SPipeline();
	pipeline->m_Pipeline = pso;
	pipeline->m_PrimitiveTopology = (D3D12_PRIMITIVE_TOPOLOGY) (params.m_PrimitiveType + 1);

	return pipeline;
}
Pipeline CreatePipeline(Device device, const SComputePipelineParams& params)
{
	if (params.m_RootSignature == nullptr)
		return nullptr;

	D3D12_COMPUTE_PIPELINE_STATE_DESC desc = {};
	desc.pRootSignature = params.m_RootSignature->m_Root;
	desc.CS.pShaderBytecode = params.m_CS.m_Code;
	desc.CS.BytecodeLength = params.m_CS.m_Size;

	ID3D12PipelineState* pso = nullptr;
	HRESULT hr = device->m_pD3DDevice->CreateComputePipelineState(&desc, __uuidof(ID3D12PipelineState), (void**)&pso);
	if (FAILED(hr))
		return nullptr;

	SetD3DName(pso, params.m_Name);

	Pipeline pipeline = new SPipeline();
	pipeline->m_Pipeline = pso;

	return pipeline;
}
void DestroyPipeline(Device device, Pipeline& pipeline)
{
	if (pipeline)
	{
		pipeline->m_Pipeline->Release();

		delete pipeline;
		pipeline = nullptr;
	}
}

BlendState CreateBlendState(Device device, BlendFactor src, BlendFactor dst, BlendMode mode, uint32 mask, bool alpha_to_coverage)
{
	BOOL blend_enable = (src != BF_ONE || dst != BF_ZERO);

	BlendState state = new SBlendState();
	D3D12_BLEND_DESC &desc = state->m_Desc;

	desc.AlphaToCoverageEnable = (BOOL) alpha_to_coverage;
	desc.IndependentBlendEnable = FALSE;

	static const D3D12_BLEND blend_factors[] =
	{
		D3D12_BLEND_ZERO,
		D3D12_BLEND_ONE,
		D3D12_BLEND_SRC_COLOR,
		D3D12_BLEND_INV_SRC_COLOR,
		D3D12_BLEND_SRC_ALPHA,
		D3D12_BLEND_INV_SRC_ALPHA,
		D3D12_BLEND_DEST_COLOR,
		D3D12_BLEND_INV_DEST_COLOR,
		D3D12_BLEND_DEST_ALPHA,
		D3D12_BLEND_INV_DEST_ALPHA,
	};

	static const D3D12_BLEND_OP blend_modes[] =
	{
		D3D12_BLEND_OP_ADD,
		D3D12_BLEND_OP_SUBTRACT,
		D3D12_BLEND_OP_REV_SUBTRACT,
		D3D12_BLEND_OP_MIN,
		D3D12_BLEND_OP_MAX,
	};


	for (int i = 0; i < 8; i++)
	{
		desc.RenderTarget[i].BlendEnable = blend_enable;

		desc.RenderTarget[i].BlendOp        = blend_modes[mode];
		desc.RenderTarget[i].BlendOpAlpha   = blend_modes[mode];
		desc.RenderTarget[i].SrcBlend       = blend_factors[src];
		desc.RenderTarget[i].SrcBlendAlpha  = blend_factors[src];
		desc.RenderTarget[i].DestBlend      = blend_factors[dst];
		desc.RenderTarget[i].DestBlendAlpha = blend_factors[dst];

		desc.RenderTarget[i].RenderTargetWriteMask = (UINT8) mask;
	}

	return state;
}

void DestroyBlendState(Device device, BlendState& state)
{
	if (state)
	{
		delete state;
		state = nullptr;
	}
}

VertexSetup CreateVertexSetup(Device device, Buffer vertex_buffer, uint vb_stride, Buffer index_buffer, uint ib_stride)
{
	VertexSetup setup = new SVertexSetup();
	memset(setup, 0, sizeof(SVertexSetup));

	if (index_buffer)
	{
		ASSERT(ib_stride == sizeof(uint16) || ib_stride == sizeof(uint32));

		setup->m_IBView.BufferLocation = index_buffer->m_Buffer->GetGPUVirtualAddress();
		setup->m_IBView.SizeInBytes    = index_buffer->m_Size;
		setup->m_IBView.Format         = ib_stride == sizeof(uint16)? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
	}

	if (vertex_buffer)
	{
		ASSERT(vb_stride > 0);

		setup->m_VBView.BufferLocation = vertex_buffer->m_Buffer->GetGPUVirtualAddress();
		setup->m_VBView.SizeInBytes    = vertex_buffer->m_Size;
		setup->m_VBView.StrideInBytes  = vb_stride;
	}


	return setup;
}

void DestroyVertexSetup(Device device, VertexSetup& state)
{
	if (state)
	{
		delete state;
		state = nullptr;
	}
}

Buffer CreateBuffer(Device device, const SBufferParams& params)
{
	D3D12_HEAP_PROPERTIES heap_prop = {};
	heap_prop.Type = (D3D12_HEAP_TYPE) (params.m_HeapType + 1);

	D3D12_HEAP_FLAGS flags = D3D12_HEAP_FLAG_NONE;

	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension        = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Width            = params.m_Size;
	desc.Height           = 1;
	desc.DepthOrArraySize = 1;
	desc.MipLevels        = 1;
	desc.SampleDesc.Count = 1;
	desc.Layout           = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	D3D12_RESOURCE_STATES resource_state = D3D12_RESOURCE_STATE_GENERIC_READ;

	ID3D12Resource* d3d_buffer = nullptr;
	HRESULT hr = device->m_pD3DDevice->CreateCommittedResource(&heap_prop, flags, &desc, resource_state, nullptr, __uuidof(ID3D12Resource), (void**) &d3d_buffer);
	if (FAILED(hr))
		return nullptr;

	SetD3DName(d3d_buffer, params.m_Name);

	Buffer buffer = new SBuffer();
	buffer->m_Buffer = d3d_buffer;
	buffer->m_Size = params.m_Size;
	buffer->m_HeapType = params.m_HeapType;
	buffer->m_Stride = params.m_Stride;
	buffer->m_Views = new TBufferViewMap;
	return buffer;
}

void DestroyBuffer(Device device, Buffer& buffer)
{
	if (buffer)
	{
		buffer->m_Buffer->Release();
		if (buffer->m_Views)
		{
			for (auto& view : *buffer->m_Views)
			{
				view.second.heap->Release(view.second.heapOffset, 1);
			}
			delete buffer->m_Views;
		}
		delete buffer;
		buffer = nullptr;
	}
}

uint GetBufferSize(Buffer buffer)
{
	return buffer->m_Size;
}

void BeginMarker(Context context, const char* name)
{
#ifdef USE_PIX
	PIXBeginEvent(context->m_CmdList, 0, name);
#endif

	if (context->m_IsProfiling)
	{
		SCommandList* cmdList = context->m_CmdList;
		ASSERT(cmdList->m_QueryOffset < MAX_QUERY_COUNT);

		cmdList->m_QueryNames[cmdList->m_QueryOffset] = name;
		context->m_CmdList->m_pD3DList->EndQuery(cmdList->m_QueryHeap, D3D12_QUERY_TYPE_TIMESTAMP, cmdList->m_QueryOffset++);
	}
}

void EndMarker(Context context)
{
	if (context->m_IsProfiling)
	{
		SCommandList* cmdList = context->m_CmdList;
		ASSERT(cmdList->m_QueryOffset < MAX_QUERY_COUNT);

		cmdList->m_QueryNames[cmdList->m_QueryOffset] = nullptr;
		context->m_CmdList->m_pD3DList->EndQuery(cmdList->m_QueryHeap, D3D12_QUERY_TYPE_TIMESTAMP, cmdList->m_QueryOffset++);
	}

#ifdef USE_PIX
	PIXEndEvent(context->m_CmdList);
#endif
}

void BeginContext(Context context, uint upload_buffer_size, const char* name, bool profile)
{
	Device device = GetDevice(context);
	context->m_IsProfiling = profile;

	context->m_CmdList = &context->m_Device->m_CommandLists[context->m_Device->m_FrameIndex];

	context->m_CmdList->Begin(device, upload_buffer_size);

	ID3D12DescriptorHeap* heaps[] = { context->m_Device->m_ResourceHeap.m_DescHeap, context->m_CmdList->m_SamplerHeap.m_DescHeap,
									device->m_ResourceHeap.m_DescHeap, device->m_SamplerHeap.m_DescHeap, };
	context->m_CmdList->m_pD3DList->SetDescriptorHeaps(4, heaps);

	BeginMarker(context, name);
}

void EndContext(Context context)
{
	EndMarker(context);
	Device device = GetDevice(context);
	if (context->m_CmdList->m_QueryOffset)
	{
		context->m_CmdList->m_pD3DList->ResolveQueryData(context->m_CmdList->m_QueryHeap, D3D12_QUERY_TYPE_TIMESTAMP, 0, context->m_CmdList->m_QueryOffset, context->m_CmdList->m_QueryBuffer, 0);
	}

	context->m_CmdList->End(device);

	context->m_CmdList = nullptr;
}

void SubmitContexts(Device device, uint count, SContext** context)
{
	ID3D12CommandList** cmdlists = StackAlloc<ID3D12CommandList*>(count);

	for (uint i = 0; i < count; i++)
	{
		cmdlists[i] = context[i]->m_CmdList->m_pD3DList;
	}

	device->m_pD3DQueue->ExecuteCommandLists(count, cmdlists);
}

void Finish(Device device)
{
	++device->m_PresentFenceCounter;
	device->m_pD3DQueue->Signal(device->m_PresentFence, device->m_PresentFenceCounter);

	// If the previous frame has not finished yet, wait until it's done.
	if (device->m_PresentFence->GetCompletedValue() < device->m_PresentFenceCounter)
	{
		device->m_PresentFence->SetEventOnCompletion(device->m_PresentFenceCounter, device->m_PresentFenceEvent);
		WaitForSingleObjectEx(device->m_PresentFenceEvent, INFINITE, FALSE);
	}
}

uint GetProfileData(Device device, SProfileData (&OutData)[MAX_QUERY_COUNT])
{
	const SCommandList& cmdList = device->m_CommandLists[device->m_FrameIndex];

	const uint count = cmdList.m_QueryOffset;
	if (count)
	{
		uint64* data = nullptr;
		D3D12_RANGE range = { 0, count * sizeof(uint64) };
		cmdList.m_QueryBuffer->Map(0, &range, (void**) &data);
		for (uint i = 0; i < count; i++)
		{
			OutData[i].m_TimeStamp = data[i];
			OutData[i].m_Name = cmdList.m_QueryNames[i];
		}

		cmdList.m_QueryBuffer->Unmap(0, nullptr);
	}

	return count;
}

float GetTimestampFrequency(Device device)
{
	return device->m_TimestampFrequency;
}

void CopyBuffer(Context context, const Buffer dest, const Buffer src)
{
	context->m_CmdList->m_pD3DList->CopyResource(dest->m_Buffer, src->m_Buffer);
}

uint8* MapBuffer(const SMapBufferParams& params)
{
	uint8* data = nullptr;

	if (params.m_Buffer->m_HeapType == HEAP_DEFAULT)
	{
		SCommandList* cmdList = params.m_Context->m_CmdList;

		uint offset = AllocateBufferSlice(params.m_Device, cmdList->m_Staging, params.m_Size, 16);
		data = cmdList->m_Staging.m_Data + offset;

		params.m_InternalOffset = offset;
	}
	else
	{
		D3D12_RANGE range = { 0, 0 };
		HRESULT hr = params.m_Buffer->m_Buffer->Map(0, &range, (void**)&data);
		ASSERT(SUCCEEDED(hr));

		data += params.m_Offset;
	}

	return data;
}


void UnmapBuffer(const SMapBufferParams& params)
{
	if (params.m_Buffer->m_HeapType == HEAP_DEFAULT)
	{
		Context context = params.m_Context;

		D3D12_RESOURCE_BARRIER desc;
		desc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		desc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		desc.Transition.pResource   = params.m_Buffer->m_Buffer;
		desc.Transition.Subresource = 0;

		desc.Transition.StateBefore = D3D12_RESOURCE_STATE_GENERIC_READ;
		desc.Transition.StateAfter  = D3D12_RESOURCE_STATE_COPY_DEST;
		context->m_CmdList->m_pD3DList->ResourceBarrier(1, &desc);

		context->m_CmdList->m_pD3DList->CopyBufferRegion(params.m_Buffer->m_Buffer, params.m_Offset, context->m_CmdList->m_Staging.m_Buffer->m_Buffer, params.m_InternalOffset, params.m_Size);

		desc.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
		desc.Transition.StateAfter  = D3D12_RESOURCE_STATE_GENERIC_READ;
		context->m_CmdList->m_pD3DList->ResourceBarrier(1, &desc);
	}
	else
	{
		D3D12_RANGE range = { params.m_Offset, params.m_Offset + params.m_Size };
		params.m_Buffer->m_Buffer->Unmap(0, &range);
	}
}


static uint GetFormatSize(DXGI_FORMAT format)
{
	switch (format)
	{
	case DXGI_FORMAT_R8_UNORM:
		return 1;
	case DXGI_FORMAT_R8G8_UNORM:
		return 2;
	case DXGI_FORMAT_R8G8B8A8_UNORM:
		return 4;
	case DXGI_FORMAT_BC1_UNORM:
	case DXGI_FORMAT_BC4_UNORM:
		return 8;
	case DXGI_FORMAT_BC2_UNORM:
	case DXGI_FORMAT_BC3_UNORM:
	case DXGI_FORMAT_BC5_UNORM:
	case DXGI_FORMAT_BC7_UNORM:
		return 16;
	default:
		ASSERT(0);
	}
	return 0;
}

static bool IsCompressedFormat(DXGI_FORMAT format)
{
	return (format >= DXGI_FORMAT_BC1_TYPELESS && format <= DXGI_FORMAT_BC5_SNORM) || (format >= DXGI_FORMAT_BC6H_TYPELESS && format <= DXGI_FORMAT_BC7_UNORM_SRGB);
}

void SetTextureData(Context context, Texture texture, uint mip, uint slice, const void* data, uint size, uint pitch)
{
	Device device = GetDevice(context);
	D3D12_RESOURCE_DESC desc = texture->m_Texture->GetDesc();

	D3D12_TEXTURE_COPY_LOCATION dst;
	dst.pResource = texture->m_Texture;
	dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;

	D3D12_TEXTURE_COPY_LOCATION src;
	src.pResource = context->m_CmdList->m_Staging.m_Buffer->m_Buffer;
	src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
	src.PlacedFootprint.Footprint.Format = desc.Format;


	const uint format_size = GetFormatSize(desc.Format);
	const bool is_compressed = IsCompressedFormat(desc.Format);

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
	uint dst_pitch = Align(pitch, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);

	// Grab upload buffer space
	uint dst_offset = AllocateBufferSlice(device, context->m_CmdList->m_Staging,dst_pitch * rows, D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT);

	// Copy data in place
	uint8* src_data = (uint8*) data;
	uint8* dst_data = context->m_CmdList->m_Staging.m_Data + dst_offset;
	for (uint32 r = 0; r < rows; ++r)
	{
		memcpy(dst_data, src_data, src_pitch);
		dst_data += dst_pitch;
		src_data += src_pitch;
	}

	// Issue a copy from upload buffer to texture
	dst.SubresourceIndex = mip + slice * texture->m_MipLevels;

	src.PlacedFootprint.Offset = dst_offset;
	if (is_compressed)
	{
		src.PlacedFootprint.Footprint.Width  = Align(w, 4);
		src.PlacedFootprint.Footprint.Height = Align(h, 4);
	}
	else
	{
		src.PlacedFootprint.Footprint.Width  = w;
		src.PlacedFootprint.Footprint.Height = h;
	}
	src.PlacedFootprint.Footprint.Depth  = d;
	src.PlacedFootprint.Footprint.RowPitch = dst_pitch;

	context->m_CmdList->m_pD3DList->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);


	D3D12_RESOURCE_BARRIER barrier;
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource   = texture->m_Texture;
	barrier.Transition.Subresource = dst.SubresourceIndex;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
	context->m_CmdList->m_pD3DList->ResourceBarrier(1, &barrier);
}
void BeginRenderPass(Context context, const char* name, bool clearColor, bool clearDepth)
{
	Device device = context->m_Device;
	SRenderPassDesc passDesc; memset(&passDesc, 0, sizeof(SRenderPassDesc));
	passDesc.msaa_samples = 1;
	passDesc.m_ColorTargetCount = context->m_CurrentFrameBufferDesc.m_ColorTargetCount;
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
void BeginRenderPass(Context context, const char* name, const RenderPass render_pass, const RenderSetup setup)
{
	BeginMarker(context, name);

	const uint32 count = setup->m_ColorBufferCount;
	if (count)
	{
		if (render_pass->justDesc.m_Flags & CLEAR_COLOR)
		{
			for (uint i = 0; i < setup->m_ColorBufferCount; i++)
			{
				context->m_CmdList->m_pD3DList->ClearRenderTargetView(setup->m_ColorTargets[i], context->m_ClearColor, 0, nullptr);
			}
		}
	}
	if (setup->m_DepthTarget.ptr)
	{
		if (render_pass->justDesc.m_Flags & CLEAR_DEPTH)
		{
			context->m_CmdList->m_pD3DList->ClearDepthStencilView(setup->m_DepthTarget, D3D12_CLEAR_FLAG_DEPTH, context->m_ClearDepth[0], context->m_ClearDepth[1], 0, nullptr);
		}
	}

	context->m_CmdList->m_pD3DList->OMSetRenderTargets(count, &setup->m_ColorTargets[0], true, &setup->m_DepthTarget);

	D3D12_VIEWPORT vp;
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	vp.Width    = (float) setup->m_Width;
	vp.Height   = (float) setup->m_Height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	context->m_CmdList->m_pD3DList->RSSetViewports(1, &vp);

	D3D12_RECT r;
	r.left   = 0;
	r.top    = 0;
	r.right  = setup->m_Width;
	r.bottom = setup->m_Height;
	context->m_CmdList->m_pD3DList->RSSetScissorRects(1, &r);
}

void EndRenderPass(Context context, const RenderSetup setup)
{
	/*if (setup->m_ResolveTexture)
	{
		D3D12_RESOURCE_BARRIER barriers[2] = {};
		barriers[0].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barriers[0].Transition.pResource = setup->m_ColorTextures[0]->m_Texture;
		barriers[0].Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barriers[0].Transition.StateAfter  = D3D12_RESOURCE_STATE_RESOLVE_SOURCE;

		barriers[1].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barriers[1].Transition.pResource = setup->m_ResolveTexture->m_Texture;
		barriers[1].Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barriers[1].Transition.StateAfter  = D3D12_RESOURCE_STATE_RESOLVE_DEST;

		context->m_CmdList->m_pD3DList->ResourceBarrier(2, barriers);

		context->m_CmdList->m_pD3DList->ResolveSubresource(setup->m_ResolveTexture->m_Texture, 0, setup->m_ColorTextures[0]->m_Texture, 0, g_Formats[setup->m_ResolveTexture->m_Format]);

		barriers[0].Transition.StateBefore = D3D12_RESOURCE_STATE_RESOLVE_SOURCE;
		barriers[0].Transition.StateAfter  = D3D12_RESOURCE_STATE_RENDER_TARGET;

		barriers[1].Transition.StateBefore = D3D12_RESOURCE_STATE_RESOLVE_DEST;
		barriers[1].Transition.StateAfter  = D3D12_RESOURCE_STATE_RENDER_TARGET;

		context->m_CmdList->m_pD3DList->ResourceBarrier(2, barriers);

	}*/// todo

	EndMarker(context);
}

void SetRootSignature(Context context, const RootSignature root)
{
	context->m_CmdList->m_pD3DList->SetGraphicsRootSignature(root->m_Root);
}

void SetPipeline(Context context, const Pipeline pipeline)
{
	context->m_CmdList->m_pD3DList->SetPipelineState(pipeline->m_Pipeline);
	context->m_CmdList->m_pD3DList->IASetPrimitiveTopology(pipeline->m_PrimitiveTopology);
}

void SetVertexSetup(Context context, const VertexSetup setup)
{
	if (setup->m_IBView.BufferLocation)
		context->m_CmdList->m_pD3DList->IASetIndexBuffer(&setup->m_IBView);

	if (setup->m_VBView.BufferLocation)
		context->m_CmdList->m_pD3DList->IASetVertexBuffers(0, 1, &setup->m_VBView);
}

uint8* SetVertexBuffer(Context context, uint stream, uint stride, uint count)
{
	Device device = GetDevice(context);

	uint size = stride * count;
	uint offset = AllocateBufferSlice(device, context->m_CmdList->m_Staging, size, 4);

	D3D12_VERTEX_BUFFER_VIEW view;
	view.BufferLocation = context->m_CmdList->m_Staging.m_GPUAddress + offset;
	view.SizeInBytes = size;
	view.StrideInBytes = stride;
	context->m_CmdList->m_pD3DList->IASetVertexBuffers(stream, 1, &view);

	return context->m_CmdList->m_Staging.m_Data + offset;
}

uint8* SetGraphicsConstantBuffer(Context context, uint slot, uint size)
{
	Device device = GetDevice(context);

	uint offset = AllocateBufferSlice(device, context->m_CmdList->m_Staging, size, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);

	context->m_CmdList->m_pD3DList->SetGraphicsRootConstantBufferView(slot, context->m_CmdList->m_Staging.m_GPUAddress + offset);
	return context->m_CmdList->m_Staging.m_Data + offset;
}

uint8* SetComputeConstantBuffer(Context context, uint slot, uint size)
{
	Device device = GetDevice(context);

	uint offset = AllocateBufferSlice(device, context->m_CmdList->m_Staging, size, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);

	context->m_CmdList->m_pD3DList->SetComputeRootConstantBufferView(slot, context->m_CmdList->m_Staging.m_GPUAddress + offset);
	return context->m_CmdList->m_Staging.m_Data + offset;
}

void SetGraphicsConstantBuffer(Context context, uint slot, const Buffer buffer)
{
	D3D12_GPU_VIRTUAL_ADDRESS addr = buffer->m_Buffer->GetGPUVirtualAddress();
	context->m_CmdList->m_pD3DList->SetGraphicsRootConstantBufferView(slot, addr);
}

void SetComputeConstantBuffer(Context context, uint slot, const Buffer buffer)
{
	D3D12_GPU_VIRTUAL_ADDRESS addr = buffer->m_Buffer->GetGPUVirtualAddress();
	context->m_CmdList->m_pD3DList->SetComputeRootConstantBufferView(slot, addr);
}

void SetRootConstants(Context context, uint slot, const void* data, uint count)
{
	context->m_CmdList->m_pD3DList->SetGraphicsRoot32BitConstants(slot, count, data, 0);
}

void SetRootTextureBuffer(Context context, uint slot, const Buffer buffer)
{
	D3D12_GPU_VIRTUAL_ADDRESS addr = buffer->m_Buffer->GetGPUVirtualAddress();
	context->m_CmdList->m_pD3DList->SetGraphicsRootShaderResourceView(slot, addr);
}

void SetGraphicsResourceTable(Context context, uint slot, const ResourceTable table)
{
	D3D12_GPU_DESCRIPTOR_HANDLE handle = context->m_CmdList->m_ResourceHeap.GetGPUHandle(table->m_Offset);
	context->m_CmdList->m_pD3DList->SetGraphicsRootDescriptorTable(slot, handle);
}

void SetComputeResourceTable(Context context, uint slot, const ResourceTable table)
{
	D3D12_GPU_DESCRIPTOR_HANDLE handle = context->m_CmdList->m_ResourceHeap.GetGPUHandle(table->m_Offset);
	context->m_CmdList->m_pD3DList->SetComputeRootDescriptorTable(slot, handle);
}

void SetGraphicsSamplerTable(Context context, uint slot, const SamplerTable table)
{
	D3D12_GPU_DESCRIPTOR_HANDLE handle = context->m_CmdList->m_SamplerHeap.GetGPUHandle(table->m_Offset);
	context->m_CmdList->m_pD3DList->SetGraphicsRootDescriptorTable(slot, handle);
}

void SetComputeSamplerTable(Context context, uint slot, const SamplerTable table)
{
	D3D12_GPU_DESCRIPTOR_HANDLE handle = context->m_CmdList->m_SamplerHeap.GetGPUHandle(table->m_Offset);
	context->m_CmdList->m_pD3DList->SetComputeRootDescriptorTable(slot, handle);
}

void Draw(Context context, uint start, uint count)
{
	context->m_CmdList->m_pD3DList->DrawInstanced(count, 1, start, 0);
}

void DrawIndexed(Context context, uint start, uint count)
{
	context->m_CmdList->m_pD3DList->DrawIndexedInstanced(count, 1, start, 0, 0);
}

void DrawIndexedInstanced(Context context, uint start, uint count, uint start_instance, uint instance_count)
{
	context->m_CmdList->m_pD3DList->DrawIndexedInstanced(count, instance_count, start, 0, start_instance);
}

void DrawIndexedIndirect(Context context, Buffer buffer, uint offset)
{
	ASSERT(false);
}

void DrawMeshTask(Context context, uint start, uint count)
{
	ASSERT(false);
}

void Dispatch(Context context, uint group_x, uint group_y, uint group_z)
{
	context->m_CmdList->m_pD3DList->Dispatch(group_x, group_y, group_z);
}


void DispatchIndirect(Context context, Buffer buffer, uint offset)
{
	ASSERT(false);
}

void ClearBuffer(Context context, Buffer buffer, uint32 clear_value)
{
	ASSERT(false);
}

void UAVBarrier(Context context, Buffer buffer)
{
	D3D12_RESOURCE_BARRIER desc;
	desc.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	desc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	desc.UAV.pResource = buffer? buffer->m_Buffer : nullptr;
	context->m_CmdList->m_pD3DList->ResourceBarrier(1, &desc);
}

void Barrier(Context context, const SBarrierDesc* barriers, uint count)
{
	uint barrCount = 0;
	auto ProcessBarriers = [](const SResourceDesc& desc, uint* toCount, D3D12_RESOURCE_BARRIER* outBarriers, D3D12_RESOURCE_BARRIER* templ, EResourceState transition)
	{
		STextureSubresource* sub =  AcquireTextureSubresource(desc);
		const Texture texture = (Texture)desc.m_Resource;
		/*STextureSubresourceDesc range = desc.m_texRange;
		bool isCube = texture->m_Type == TEX_CUBE || texture->m_Type == TEX_CUBE_ARRAY;
		uint ifCube6 = isCube ? 6 : 1;
		uint baseMipLevel = range.mip == -1 ? 0 : range.mip;
		uint levelCount = range.mip == -1 ? texture->m_MipLevels : 1;
		uint baseArrayLayer = range.slice == -1 ? 0 : ifCube6 * range.slice + (range.face == -1 ? 0 : range.face);
		uint layerCount = range.slice == -1 ? ifCube6 * texture->m_Slices : (range.face == -1 ? ifCube6 : 1);*/
		if (toCount)
			*toCount += sub->m_Range.layerCount * sub->m_Range.layerCount;
		if (outBarriers)
		{
			for (int curMip = sub->m_Range.baseMipLevel; curMip <= sub->m_Range.baseMipLevel + sub->m_Range.levelCount; curMip++)
			{
				for (int curLayer = sub->m_Range.baseArrayLayer; curLayer <= sub->m_Range.baseArrayLayer + sub->m_Range.layerCount; curLayer++)
				{
					D3D12_RESOURCE_BARRIER& currBarrier = *outBarriers;
					currBarrier = *templ;
					currBarrier.Transition.Subresource = curMip + curLayer * texture->m_MipLevels;
					outBarriers++;
				}
			}
			sub->m_State = transition;
		}

	};
	for (uint i = 0; i < count; i++)
	{
		if (barriers[i].m_Desc.m_Type == RESTYPE_TEXTURE)
		{
			ProcessBarriers(barriers[i].m_Desc, &barrCount, nullptr, nullptr, barriers[i].m_Transition);
		}
		else
			barrCount++;
	}
	D3D12_RESOURCE_BARRIER* outBarriers = StackAlloc<D3D12_RESOURCE_BARRIER>(barrCount);

	uint curr = 0;
	for (uint i = 0; i < count; i++)
	{

		switch (barriers[i].m_Desc.m_Type)
		{
		case RESTYPE_TEXTURE:
			{
				const Texture texture = (Texture)barriers[i].m_Desc.m_Resource;
				EResourceState before = GetCurrentState(barriers[i].m_Desc);

				STextureSubresourceDesc range = barriers[i].m_Desc.m_texRange;

				D3D12_RESOURCE_BARRIER templateDesc;
				templateDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
				templateDesc.Transition.pResource = texture->m_Texture;
				templateDesc.Transition.StateBefore = (D3D12_RESOURCE_STATES)before;
				templateDesc.Transition.StateAfter = (D3D12_RESOURCE_STATES)barriers[i].m_Transition;
				if (range.slice == -1)
				{
					templateDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
					outBarriers[curr] = templateDesc;
					curr++;
				}
				else
				{
					ProcessBarriers(barriers[i].m_Desc, &curr, &outBarriers[curr], &templateDesc, barriers[i].m_Transition);
				}
			}
			break;
		case RESTYPE_BUFFER:
			{
				Buffer buffer = (Buffer)barriers[i].m_Desc.m_Resource;
				EResourceState before = GetCurrentState(barriers[i].m_Desc);
				outBarriers[i].Transition.pResource = buffer->m_Buffer;
				outBarriers[curr].Transition.StateBefore = (D3D12_RESOURCE_STATES)before;
				outBarriers[curr].Transition.StateAfter = (D3D12_RESOURCE_STATES)barriers[i].m_Transition;
				curr++;
				buffer->m_State = barriers[i].m_Transition;

			}
			break;
		default:
			ASSERT(false);
		};

	}
	context->m_CmdList->m_pD3DList->ResourceBarrier(count, outBarriers);
}
template<>
D3D12_SHADER_RESOURCE_VIEW_DESC FillD3DViewDesc<D3D12_SHADER_RESOURCE_VIEW_DESC>(SRange range, TextureType type)
{
	D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
	switch (type)
	{
	case TEX_1D:
		desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1D;
		desc.Texture1D.MostDetailedMip = range.baseMipLevel;
		desc.Texture1D.MipLevels = range.levelCount;
		desc.Texture1D.ResourceMinLODClamp = 0;
		break;
	case TEX_1D_ARRAY:
		desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1DARRAY;
		desc.Texture1DArray.MostDetailedMip = range.baseMipLevel;
		desc.Texture1DArray.MipLevels = range.levelCount;
		desc.Texture1DArray.FirstArraySlice = range.baseArrayLayer;
		desc.Texture1DArray.ArraySize = range.layerCount;
		desc.Texture1DArray.ResourceMinLODClamp = 0;
		break;
	case TEX_2D:
		desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MostDetailedMip = range.baseMipLevel;
		desc.Texture2D.MipLevels = range.levelCount;
		desc.Texture2D.PlaneSlice = 0;
		desc.Texture2D.ResourceMinLODClamp = 0;
		break;
	case TEX_2D_ARRAY:
		desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
		desc.Texture2DArray.MostDetailedMip = range.baseMipLevel;
		desc.Texture2DArray.MipLevels = range.levelCount;
		desc.Texture2DArray.FirstArraySlice = range.baseArrayLayer;
		desc.Texture2DArray.ArraySize = range.layerCount;
		desc.Texture2DArray.PlaneSlice = 0;
		desc.Texture2DArray.ResourceMinLODClamp = 0;
		break;
	case TEX_CUBE:
		desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		desc.TextureCube.MostDetailedMip = range.baseMipLevel;
		desc.TextureCube.MipLevels = range.levelCount;
		desc.TextureCube.ResourceMinLODClamp = 0;
		break;
	case TEX_CUBE_ARRAY:
		desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBEARRAY;
		desc.TextureCubeArray.MostDetailedMip = range.baseMipLevel;
		desc.TextureCubeArray.MipLevels = range.levelCount;
		desc.TextureCubeArray.First2DArrayFace = range.baseArrayLayer;
		desc.TextureCubeArray.NumCubes = range.layerCount / 6;
		desc.TextureCubeArray.ResourceMinLODClamp = 0;
		break;
	case TEX_3D:
		desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE3D;
		desc.Texture3D.MostDetailedMip = range.baseMipLevel;
		desc.Texture3D.MipLevels = range.levelCount;
		desc.Texture3D.ResourceMinLODClamp = 0;
		break;
	default:
		ASSERT(false);
	}
	desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	return desc;
}
template<>
D3D12_UNORDERED_ACCESS_VIEW_DESC FillD3DViewDesc<D3D12_UNORDERED_ACCESS_VIEW_DESC>(SRange range, TextureType type)
{
	D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {};
	switch (type)
	{
	case TEX_1D:
		desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE1D;
		desc.Texture1D.MipSlice = range.baseMipLevel;
		break;
	case TEX_1D_ARRAY:
		desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE1DARRAY;
		desc.Texture1DArray.MipSlice = range.baseMipLevel;
		desc.Texture1DArray.FirstArraySlice = range.baseArrayLayer;
		desc.Texture1DArray.ArraySize = range.layerCount;
		break;
	case TEX_2D:
		desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipSlice = range.baseMipLevel;
		desc.Texture2D.PlaneSlice = 0;
		break;
	case TEX_2D_ARRAY:
		desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
		desc.Texture2DArray.MipSlice = range.baseMipLevel;
		desc.Texture2DArray.FirstArraySlice = range.baseArrayLayer;
		desc.Texture2DArray.ArraySize = range.layerCount;
		desc.Texture2DArray.PlaneSlice = 0;
		break;
	default:
		ASSERT(false);
	}
	return desc;
}
template<>
D3D12_RENDER_TARGET_VIEW_DESC FillD3DViewDesc<D3D12_RENDER_TARGET_VIEW_DESC>(SRange range, TextureType type)
{
	D3D12_RENDER_TARGET_VIEW_DESC desc = {};
	switch (type)
	{
	case TEX_1D:
		desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE1D;
		desc.Texture1D.MipSlice = range.baseMipLevel;
		break;
	case TEX_1D_ARRAY:
		desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE1DARRAY;
		desc.Texture1DArray.MipSlice = range.baseMipLevel;
		desc.Texture1DArray.FirstArraySlice = range.baseArrayLayer;
		desc.Texture1DArray.ArraySize = range.layerCount;
		break;
	case TEX_2D:
		desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipSlice = range.baseMipLevel;
		desc.Texture2D.PlaneSlice = 0;
		break;
	case TEX_2D_ARRAY:
		desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
		desc.Texture2DArray.MipSlice = range.baseMipLevel;
		desc.Texture2DArray.FirstArraySlice = range.baseArrayLayer;
		desc.Texture2DArray.ArraySize = range.layerCount;
		desc.Texture2DArray.PlaneSlice = 0;
		break;
	default:
		ASSERT(false);
	}
	return desc;
}
template<>
D3D12_DEPTH_STENCIL_VIEW_DESC FillD3DViewDesc<D3D12_DEPTH_STENCIL_VIEW_DESC>(SRange range, TextureType type)//todo: support aspects
{
	D3D12_DEPTH_STENCIL_VIEW_DESC desc = {};
	switch (type)
	{
	case TEX_1D:
		desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE1D;
		desc.Texture1D.MipSlice = range.baseMipLevel;
		break;
	case TEX_1D_ARRAY:
		desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE1DARRAY;
		desc.Texture1DArray.MipSlice = range.baseMipLevel;
		desc.Texture1DArray.FirstArraySlice = range.baseArrayLayer;
		desc.Texture1DArray.ArraySize = range.layerCount;
		break;
	case TEX_2D:
		desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipSlice = range.baseMipLevel;
		break;
	case TEX_2D_ARRAY:
		desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
		desc.Texture2DArray.MipSlice = range.baseMipLevel;
		desc.Texture2DArray.FirstArraySlice = range.baseArrayLayer;
		desc.Texture2DArray.ArraySize = range.layerCount;
		break;
	default:
		ASSERT(false);
	}
	return desc;
}
#endif