/******************** (C) Copyright 2021 realworld one GmbH & Co. KG All Rights Reserved. ********************/

#include "SharedTexture/D3D11/D3D11ImGfxCtx.h"

#include "Uranium.h"
#include "HardwareInfo.h"
#include "SharedTexture/SharedTextureInterface.h"
#include "D3D11Util.h"
#include "SharedTexture/D3DErrorHandling.h"

#define NV_AFTERMATH 0

ID3D11Device* UD3D11ImGfxCtx::GetRhiDevice()
{
	return static_cast<ID3D11Device*>(GDynamicRHI->RHIGetNativeDevice());
}

bool UD3D11ImGfxCtx::GetRhiAdapter(IDXGIAdapter*& outAdapter)
{
	ComPtr<IDXGIDevice> dxgiDevice;
	HRESULT hr = GetRhiDevice()->QueryInterface(IID_PPV_ARGS(&dxgiDevice));
	if(HrFail(hr, TEXT("Failed to query DXGI device of UE4.")))
	{
		outAdapter = nullptr;
		return false;
	}

	IDXGIAdapter* adapter;
	hr = dxgiDevice->GetAdapter(&adapter);
	if(HrFail(hr, TEXT("Failed to get DXGI device of UE4.")))
	{
		outAdapter = nullptr;
		return false;
	}

	outAdapter = adapter;
	return true;
}

void UD3D11ImGfxCtx::Initialize()
{
	D3D_FEATURE_LEVEL featureLevels[4] = {
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0
	};

	//UINT CreateDeviceFlags = 0;

	HRESULT hr = D3D11CreateDevice(
		nullptr, // using unreal's adapter yields invalid argument error, why?
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0
		,
		&featureLevels[0], 4,
		D3D11_SDK_VERSION,
		&Device,
		nullptr,
		&DeviceContext
	);
	HrFail_R(hr, TEXT("Failed to create immediate D3D11 device."));

	hr = Device.As(&Device1);
	HrFail_R(hr, TEXT("Failed to query ID3D11Device1 interface of immediate D3D11 device"));
}

#define HrFail_R_F(hr, m, ...) if(HrFail(hr, m)) { DeviceContext->Flush(); __VA_ARGS__ return; }
#define HrFail_R_F_RM(hrs, m, ...) HrFail_R_F(hrs, m, mutex->ReleaseSync(0); __VA_ARGS__)

void UD3D11ImGfxCtx::OnAcceleratedPaint(void* handle)
{
	if(!bIsInitialized)
	{
		Initialize();
		bIsInitialized = true;
	}
	if(bFailure)
	{
		return;
	}

	ComPtr<IDXGIKeyedMutex> mutex;
	ComPtr<ID3D11Resource> sharedResource;
	ComPtr<ID3D11Texture2D> sharedTexture;
	HRESULT hr = Device1->OpenSharedResource1(static_cast<HANDLE>(handle), IID_PPV_ARGS(&sharedResource));
	HrFail_R_F(hr, TEXT("Failed to open shared resource."));

	hr = sharedResource.As(&mutex);
	HrFail_R_F(hr, TEXT("Failed to get keyed mutex from shared resource."));
	mutex->AcquireSync(1, INFINITE);

	hr = sharedResource.As(&sharedTexture);
	HrFail_R_F_RM(hr, TEXT("Shared resource was not a texture."));

	D3D11_TEXTURE2D_DESC sharedDesc;
	sharedTexture->GetDesc(&sharedDesc);

	// copy description for setting our own flags
	auto imDesc = sharedDesc;
	imDesc.MiscFlags = D3D11_RESOURCE_MISC_SHARED;

	if (Width != sharedDesc.Width || Height != sharedDesc.Height || Format != sharedDesc.Format)
	{
		if (TargetTexture)
		{
			TargetTexture.Reset();
		}

		Width = sharedDesc.Width;
		Height = sharedDesc.Height;
		Format = sharedDesc.Format;

		hr = Device->CreateTexture2D(&imDesc, nullptr, &TargetTexture);
		HrFail_R_F_RM(hr, TEXT("Couldn't create immediate target texture."));

		ComPtr<IDXGIResource> targetResource;
		hr = TargetTexture.As(&targetResource);
		hr = targetResource->GetSharedHandle(&TargetSharedHandle);
		HrFail_R_F_RM(hr, TEXT("Failed to get shared handle of immediate target texture"));
	}

	DeviceContext->CopyResource(TargetTexture.Get(), sharedTexture.Get());
	DeviceContext->Flush();

	mutex->ReleaseSync(0);
}

#undef HrFail_R_F
#undef HrFail_R_F_RM