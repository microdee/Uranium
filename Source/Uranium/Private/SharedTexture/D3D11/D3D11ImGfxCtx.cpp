// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

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

bool UD3D11ImGfxCtx::GetRhiAdapter(IDXGIAdapter*& OutAdapter)
{
    ComPtr<IDXGIDevice> DxgiDevice;
    auto hr = GetRhiDevice()->QueryInterface(IID_PPV_ARGS(&DxgiDevice));
    if(HrFail(hr, TEXT("Failed to query DXGI device of UE4.")))
    {
        OutAdapter = nullptr;
        return false;
    }

    IDXGIAdapter* Adapter;
    hr = DxgiDevice->GetAdapter(&Adapter);
    if(HrFail(hr, TEXT("Failed to get DXGI device of UE4.")))
    {
        OutAdapter = nullptr;
        return false;
    }

    OutAdapter = Adapter;
    return true;
}

void UD3D11ImGfxCtx::Initialize()
{
    D3D_FEATURE_LEVEL FeatureLevels[4] = {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0
    };

    //UINT CreateDeviceFlags = 0;

    auto hr = D3D11CreateDevice(
        nullptr, // using unreal's adapter yields invalid argument error, why?
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        0
        ,
        &FeatureLevels[0], 4,
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
#define HrFail_R_F_RM(hrs, m, ...) HrFail_R_F(hrs, m, Mutex->ReleaseSync(0); __VA_ARGS__)

void UD3D11ImGfxCtx::OnAcceleratedPaint(void* Handle)
{
    if(!bIsInitialized)
    {
        Initialize();
        bIsInitialized = true;
    }
    if(bFailure) return;

    ComPtr<IDXGIKeyedMutex> Mutex;
    ComPtr<ID3D11Resource> SharedResource;
    ComPtr<ID3D11Texture2D> SharedTexture;
    auto hr = Device1->OpenSharedResource1(static_cast<HANDLE>(Handle), IID_PPV_ARGS(&SharedResource));
    HrFail_R_F(hr, TEXT("Failed to open shared resource."));

    hr = SharedResource.As(&Mutex);
    HrFail_R_F(hr, TEXT("Failed to get keyed mutex from shared resource."));
    Mutex->AcquireSync(1, INFINITE);

    hr = SharedResource.As(&SharedTexture);
    HrFail_R_F_RM(hr, TEXT("Shared resource was not a texture."));

    D3D11_TEXTURE2D_DESC SharedDesc;
    SharedTexture->GetDesc(&SharedDesc);

    // copy description for setting our own flags
    auto ImDesc = SharedDesc;
    ImDesc.MiscFlags = D3D11_RESOURCE_MISC_SHARED;

    if (Width != SharedDesc.Width || Height != SharedDesc.Height || Format != SharedDesc.Format)
    {
        if (TargetTexture) TargetTexture.Reset();

        Width = SharedDesc.Width;
        Height = SharedDesc.Height;
        Format = SharedDesc.Format;

        hr = Device->CreateTexture2D(&ImDesc, nullptr, &TargetTexture);
        HrFail_R_F_RM(hr, TEXT("Couldn't create immediate target texture."));

        ComPtr<IDXGIResource> TargetResource;
        hr = TargetTexture.As(&TargetResource);
        hr = TargetResource->GetSharedHandle(&TargetSharedHandle);
        HrFail_R_F_RM(hr, TEXT("Failed to get shared handle of immediate target texture"));
    }

    DeviceContext->CopyResource(TargetTexture.Get(), SharedTexture.Get());
    DeviceContext->Flush();

    Mutex->ReleaseSync(0);
}
