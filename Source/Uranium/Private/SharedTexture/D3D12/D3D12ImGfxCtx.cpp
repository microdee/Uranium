// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "SharedTexture/D3D12/D3D12ImGfxCtx.h"

#include "Uranium.h"
#include "HardwareInfo.h"
#include "SharedTexture/D3DErrorHandling.h"
#include "SharedTexture/SharedTextureInterface.h"

ID3D12Device* UD3D12ImGfxCtx::GetRhiDevice()
{
	return static_cast<ID3D12Device*>(GDynamicRHI->RHIGetNativeDevice());
}

bool UD3D12ImGfxCtx::GetRhiAdapter(IDXGIAdapter*& OutAdapter)
{
	ComPtr<IDXGIDevice> DxgiDevice;
	auto hr = GetRhiDevice()->QueryInterface(IID_PPV_ARGS(&DxgiDevice));
	if (HrFail(hr, TEXT("Failed to query DXGI device of UE4.")))
	{
		OutAdapter = nullptr;
		return false;
	}

	IDXGIAdapter* Adapter;
	hr = DxgiDevice->GetAdapter(&Adapter);
	if (HrFail(hr, TEXT("Failed to get DXGI device of UE4.")))
	{
		OutAdapter = nullptr;
		return false;
	}

	OutAdapter = Adapter;
	return true;
}

void UD3D12ImGfxCtx::EndFrame()
{
	CommandList->Close();
	//HrFail_R(hr, TEXT("Failed to close command list this frame."));

	ID3D12CommandList* CommandLists[] = { CommandList.Get() };
	CommandQueue->ExecuteCommandLists(_countof(CommandLists), CommandLists);
	if(MutexOnlyContext) MutexOnlyContext->Flush();
}

void UD3D12ImGfxCtx::Initialize()
{
	ComPtr<IDXGIFactory4> Factory;
	auto hr = CreateDXGIFactory1(IID_PPV_ARGS(&Factory));
	HrFail_R(hr, TEXT("Failed to create DXGI factory for immediate D3D12 device"));

	ComPtr<IDXGIAdapter1> GfxAdapter1;
	ComPtr<IDXGIAdapter4> GfxAdapter4;

	SIZE_T maxDedicatedVideoMemory = 0;
	for (UINT i = 0; Factory->EnumAdapters1(i, &GfxAdapter1) != DXGI_ERROR_NOT_FOUND; ++i)
	{
		DXGI_ADAPTER_DESC1 dxgiAdapterDesc1;
		GfxAdapter1->GetDesc1(&dxgiAdapterDesc1);

		// Check to see if the adapter can create a D3D12 device without actually 
		// creating it. The adapter with the largest dedicated video memory
		// is favored.
		if ((dxgiAdapterDesc1.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) == 0 &&
			SUCCEEDED(D3D12CreateDevice(GfxAdapter1.Get(),
				D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr)) &&
			dxgiAdapterDesc1.DedicatedVideoMemory > maxDedicatedVideoMemory)
		{
			maxDedicatedVideoMemory = dxgiAdapterDesc1.DedicatedVideoMemory;
			hr = GfxAdapter1.As(&GfxAdapter4);
		}
	}
	HrFail_R(hr, TEXT("Failed to get the proper adapter to be used."));

	hr = D3D12CreateDevice(
		GfxAdapter4.Get(),
		D3D_FEATURE_LEVEL_11_0,
		IID_PPV_ARGS(&Device)
	);
	HrFail_R(hr, TEXT("Failed to create immediate D3D12 device"));

	// Creating a D3D11 device just for accessing the keyed mutex chromium provides
	
	D3D_FEATURE_LEVEL FeatureLevels[4] = {
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
		0
		,
		&FeatureLevels[0], 4,
		D3D11_SDK_VERSION,
		&MutexOnlyDevice,
		nullptr,
		&MutexOnlyContext
	);
	HrFail_R(hr, TEXT("Failed to create mutex only D3D11 device for the D3D12 immediate graphics context."));

	hr = MutexOnlyDevice.As(&MutexOnlyDevice1);
	HrFail_R(hr, TEXT("Failed to query ID3D11Device1 interface of the mutex only D3D11 device"));

	D3D12_COMMAND_QUEUE_DESC CmdQueDesc;
	CmdQueDesc.NodeMask = 0,
	CmdQueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	CmdQueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	CmdQueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	hr = Device->CreateCommandQueue(&CmdQueDesc, IID_PPV_ARGS(&CommandQueue));
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
#define HrFail_R_RM(hrs, m, ...) if(HrFail(hrs, m)) { Mutex->ReleaseSync(0); __VA_ARGS__ return; }
#define HrFail_R_F_RM(hrs, m, ...) HrFail_R_F(hrs, m, Mutex->ReleaseSync(0); __VA_ARGS__)

void UD3D12ImGfxCtx::OnAcceleratedPaint(void* Handle)
{
	if (!bIsInitialized)
	{
		Initialize();
		bIsInitialized = true;
	}
	if (bFailure) return;

	auto hr = CommandAllocator->Reset();
	HrFail_R(hr, TEXT("Failed to reset command allocator in frame"));

	ComPtr<IDXGIKeyedMutex> Mutex;
	ComPtr<ID3D11Resource> MutexOnlyResource;
	ComPtr<ID3D12Resource> SharedResource;

	hr = MutexOnlyDevice1->OpenSharedResource1(Handle, IID_PPV_ARGS(&MutexOnlyResource));
	HrFail_R(hr, TEXT("Failed to open the shared resource for getting the mutex."));

	hr = MutexOnlyResource.As(&Mutex);
	HrFail_R_F(hr, TEXT("Failed to get keyed mutex from shared resource."));
	Mutex->AcquireSync(1, INFINITE);

	hr = Device->OpenSharedHandle(Handle, IID_PPV_ARGS(&SharedResource));
	HrFail_R(hr, TEXT("Failed to open shared resource."));

	auto SharedDesc = SharedResource->GetDesc();

	if(Width != SharedDesc.Width || Height != SharedDesc.Height || Format != SharedDesc.Format)
	{
		if (TargetTexture)
		{
			TargetTexture.Reset();
		}

		// copy description for setting our own flags
		auto ImDesc = SharedDesc;
		
		Width = SharedDesc.Width;
		Height = SharedDesc.Height;
		Format = SharedDesc.Format;

		auto HeapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

		hr = Device->CreateCommittedResource(
			&HeapProps,
			D3D12_HEAP_FLAG_NONE
			| D3D12_HEAP_FLAG_SHARED
			,
			&ImDesc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(&TargetTexture)
		);
		HrFail_R_RM(hr, TEXT("Couldn't create immediate target texture"));
	}

	CommandList->CopyResource(TargetTexture.Get(), SharedResource.Get());

	EndFrame();
	Mutex->ReleaseSync(0);
}