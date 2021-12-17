/******************** (C) Copyright 2021 realworld one GmbH & Co. KG All Rights Reserved. ********************/

#include "SharedTexture/D3D12/D3D12ImGfxCtx.h"

#include "Uranium.h"
#include "HardwareInfo.h"
#include "SharedTexture/D3DErrorHandling.h"
#include "SharedTexture/SharedTextureInterface.h"

ID3D12Device* UD3D12ImGfxCtx::GetRhiDevice()
{
	return static_cast<ID3D12Device*>(GDynamicRHI->RHIGetNativeDevice());
}

bool UD3D12ImGfxCtx::GetRhiAdapter(IDXGIAdapter*& outAdapter)
{
	ComPtr<IDXGIDevice> dxgiDevice;
	HRESULT hr = GetRhiDevice()->QueryInterface(IID_PPV_ARGS(&dxgiDevice));
	if (HrFail(hr, TEXT("Failed to query DXGI device of UE4.")))
	{
		outAdapter = nullptr;
		return false;
	}

	IDXGIAdapter* adapter;
	hr = dxgiDevice->GetAdapter(&adapter);
	if (HrFail(hr, TEXT("Failed to get DXGI device of UE4.")))
	{
		outAdapter = nullptr;
		return false;
	}

	outAdapter = adapter;
	return true;
}

void UD3D12ImGfxCtx::EndFrame()
{
	CommandList->Close();
	//HrFail_R(hr, TEXT("Failed to close command list this frame."));

	ID3D12CommandList* cmdLists[] = { CommandList.Get() };
	CommandQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);
	if(MutexOnlyContext)
	{
		MutexOnlyContext->Flush();
	}
}

void UD3D12ImGfxCtx::Initialize()
{
	ComPtr<IDXGIFactory4> factory;
	HRESULT hr = CreateDXGIFactory1(IID_PPV_ARGS(&factory));
	HrFail_R(hr, TEXT("Failed to create DXGI factory for immediate D3D12 device"));

	ComPtr<IDXGIAdapter1> gfxAdapter1;
	ComPtr<IDXGIAdapter4> gfxAdapter4;

	SIZE_T maxDedicatedVideoMemory = 0;
	for (UINT i = 0; factory->EnumAdapters1(i, &gfxAdapter1) != DXGI_ERROR_NOT_FOUND; ++i)
	{
		DXGI_ADAPTER_DESC1 dxgiAdapterDesc1;
		gfxAdapter1->GetDesc1(&dxgiAdapterDesc1);

		// Check to see if the adapter can create a D3D12 device without actually 
		// creating it. The adapter with the largest dedicated video memory
		// is favored.
		bool adapterValid = SUCCEEDED(D3D12CreateDevice(gfxAdapter1.Get(), D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr));
		if ((dxgiAdapterDesc1.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) == 0
			&& adapterValid
			&& dxgiAdapterDesc1.DedicatedVideoMemory > maxDedicatedVideoMemory
		) {
			maxDedicatedVideoMemory = dxgiAdapterDesc1.DedicatedVideoMemory;
			hr = gfxAdapter1.As(&gfxAdapter4);
		}
	}
	HrFail_R(hr, TEXT("Failed to get the proper adapter to be used."));

	hr = D3D12CreateDevice(gfxAdapter4.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&Device));
	HrFail_R(hr, TEXT("Failed to create immediate D3D12 device"));

	// Creating a D3D11 device just for accessing the keyed mutex chromium provides
	
	D3D_FEATURE_LEVEL featureLevels[4] = {
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0
	};

	//UINT CreateDeviceFlags = 0;

	hr = D3D11CreateDevice(
		nullptr, // using unreal's adapter yields invalid argument error, why?
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0,
		&featureLevels[0], 4,
		D3D11_SDK_VERSION,
		&MutexOnlyDevice,
		nullptr,
		&MutexOnlyContext
	);
	HrFail_R(hr, TEXT("Failed to create mutex only D3D11 device for the D3D12 immediate graphics context."));

	hr = MutexOnlyDevice.As(&MutexOnlyDevice1);
	HrFail_R(hr, TEXT("Failed to query ID3D11Device1 interface of the mutex only D3D11 device"));

	D3D12_COMMAND_QUEUE_DESC cmdQueDesc;
	cmdQueDesc.NodeMask = 0,
	cmdQueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	cmdQueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	cmdQueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	hr = Device->CreateCommandQueue(&cmdQueDesc, IID_PPV_ARGS(&CommandQueue));
	HrFail_R(hr, TEXT("Failed to create immediate D3D12 Command queue"));

	hr = Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&CommandAllocator));
	HrFail_R(hr, TEXT("Failed to create immediate D3D12 Command allocator"));
	
	hr = Device->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		CommandAllocator.Get(),
		nullptr, // we don't need a pipeline state in this particular case
		IID_PPV_ARGS(&CommandList)
	);
	HrFail_R(hr, TEXT("Failed to create immediate D3D12 Command list"));

	hr = CommandList->Close();
	HrFail_R(hr, TEXT("Failed to initially close the new immediate command list"));
}

#define HrFail_R_F(hr, m, ...) if(HrFail(hr, m)) { EndFrame(); __VA_ARGS__ return; }
#define HrFail_R_RM(hrs, m, ...) if(HrFail(hrs, m)) { mutex->ReleaseSync(0); __VA_ARGS__ return; }
#define HrFail_R_F_RM(hrs, m, ...) HrFail_R_F(hrs, m, mutex->ReleaseSync(0); __VA_ARGS__)

void UD3D12ImGfxCtx::OnAcceleratedPaint(void* handle)
{
	if (!bIsInitialized)
	{
		Initialize();
		bIsInitialized = true;
	}
	if (bFailure)
	{
		return;
	}

	HRESULT hr = CommandAllocator->Reset();
	HrFail_R(hr, TEXT("Failed to reset command allocator in frame"));

	ComPtr<IDXGIKeyedMutex> mutex;
	ComPtr<ID3D11Resource> mutexOnlyResource;
	ComPtr<ID3D12Resource> sharedResource;

	hr = MutexOnlyDevice1->OpenSharedResource1(handle, IID_PPV_ARGS(&mutexOnlyResource));
	HrFail_R(hr, TEXT("Failed to open the shared resource for getting the mutex."));

	hr = mutexOnlyResource.As(&mutex);
	HrFail_R_F(hr, TEXT("Failed to get keyed mutex from shared resource."));
	mutex->AcquireSync(1, INFINITE);

	hr = Device->OpenSharedHandle(handle, IID_PPV_ARGS(&sharedResource));
	HrFail_R(hr, TEXT("Failed to open shared resource."));

	D3D12_RESOURCE_DESC sharedDesc = sharedResource->GetDesc();

	if(Width != sharedDesc.Width || Height != sharedDesc.Height || Format != sharedDesc.Format)
	{
		if (TargetTexture)
		{
			TargetTexture.Reset();
		}

		// copy description for setting our own flags
		D3D12_RESOURCE_DESC imDesc = sharedDesc;
		
		Width = sharedDesc.Width;
		Height = sharedDesc.Height;
		Format = sharedDesc.Format;

		CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_DEFAULT);

		hr = Device->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE
			| D3D12_HEAP_FLAG_SHARED
			,
			&imDesc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(&TargetTexture)
		);
		HrFail_R_RM(hr, TEXT("Couldn't create immediate target texture"));
	}

	CommandList->CopyResource(TargetTexture.Get(), sharedResource.Get());

	EndFrame();
	mutex->ReleaseSync(0);
}

#undef HrFail_R_F
#undef HrFail_R_RM
#undef HrFail_R_F_RM