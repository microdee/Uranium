// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "SharedTexture/D3D12/D3D12SharedTexture.h"
#include "SharedTexture/D3D11/D3D11ImGfxCtx.h"

#include "Uranium.h"

#define NV_AFTERMATH 0
#include "D3D12RHI/Private/D3D12RHIPrivate.h"
#include "SharedTexture/D3DErrorHandling.h"

ID3D12Device* UD3D12SharedTexture::GetRhiDevice()
{
	return static_cast<ID3D12Device*>(GDynamicRHI->RHIGetNativeDevice());
}

ID3D12CommandQueue* UD3D12SharedTexture::GetRhiCommandQueue(FRHICommandListImmediate& RHICmdList)
{
	FD3D12CommandContext& CmdCtx = static_cast<FD3D12CommandContext&>(RHICmdList.GetContext());
	return CmdCtx.CommandListHandle.GetCommandListManager()->GetD3DCommandQueue();
}

ID3D12GraphicsCommandList* UD3D12SharedTexture::GetRhiGfxCmdList(FRHICommandListImmediate& RHICmdList)
{
	FD3D12CommandContext& CmdCtx = static_cast<FD3D12CommandContext&>(RHICmdList.GetContext());
	return CmdCtx.CommandListHandle.GraphicsCommandList();
}

ID3D12Resource* UD3D12SharedTexture::GetTargetTextureResource()
{
	if (!TargetTexture) return nullptr;
	
#if UE_VERSION >= MAKE_UE_VERSION(4, 26)
		
	if (!TargetTexture->Resource) return nullptr;

	auto RhiRes = TargetTexture->Resource->GetTexture2DRHI();
	if (!RhiRes) return nullptr;
	
#else

	auto TargetRes = static_cast<FTexture2DResource*>(TargetTexture->Resource);
	if (!TargetRes) return nullptr;

	auto RhiRes = TargetRes->GetTexture2DRHI();
	if (!RhiRes) return nullptr;
	
#endif

	return static_cast<ID3D12Resource*>(RhiRes->GetNativeResource());
}

bool UD3D12SharedTexture::CreateDD3D11On12TargetTexture()
{
	auto NativeRes = GetTargetTextureResource();
	if (!NativeRes) return true;

	D3D11_RESOURCE_FLAGS d3d11Flags = { D3D11_BIND_SHADER_RESOURCE };

	auto hr = D3D11On12Device->CreateWrappedResource(
		static_cast<IUnknown*>(NativeRes),
		&d3d11Flags,
		D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		IID_PPV_ARGS(&D3D11On12TargetTexture)
	);
	HrFail_R(hr, TEXT("Failed to wrap D3D12 resource of target texture into a D3D11 resource")) true;
	return false;
}

#if (UR_CHROM_COMPAT_USE_NTHANDLE()) || UR_ST_DEVMODE
void UD3D12SharedTexture::Initialize()
{
	ImGfxCtx = NewObject<UD3D11ImGfxCtx>();
}

void UD3D12SharedTexture::OnAcceleratedPaint(void* Handle)
{
	ImGfxCtx->OnAcceleratedPaint(Handle);

	if (PreviousHandle == Handle) return;
	PreviousHandle = Handle;

	auto UEFormat = FromDXGIFormat(ImGfxCtx->Format);

	if (
		Width != ImGfxCtx->Width
		|| Height != ImGfxCtx->Height
		|| Format != UEFormat
	) InvalidateUeResources(ImGfxCtx->Width, ImGfxCtx->Height, UEFormat);
}

void UD3D12SharedTexture::Render()
{
	if (!TargetTexture->IsValidLowLevelFast()) return;
	ENQUEUE_RENDER_COMMAND(void)([this](FRHICommandListImmediate& RHICmdList)
	{
		HRESULT hr;
		if (bFailure) return;

		if (!bIsInitialized)
		{
			bIsInitialized = true;
			D3D_FEATURE_LEVEL FeatureLevels[4] = {
				D3D_FEATURE_LEVEL_11_1,
				D3D_FEATURE_LEVEL_11_0,
				D3D_FEATURE_LEVEL_10_1,
				D3D_FEATURE_LEVEL_10_0
			};

			auto CmdQueue = GetRhiCommandQueue(RHICmdList);

			hr = D3D11On12CreateDevice(
				GetRhiDevice(),
				0,
				&FeatureLevels[0],
				4,
				reinterpret_cast<IUnknown**>(&CmdQueue),
				1,
				0,
				&D3D11Device,
				&D3D11Context,
				nullptr
			);

			HrFail_R(hr, TEXT("Couldn't create D3D11 on 12 device."));

			hr = D3D11Device.As(&D3D11On12Device);
			HrFail_R(hr, TEXT("Couldn't query D3D11 on 12 device from created D3D11 device."));
		}
		if (!D3D11On12TargetTexture)
		{
			CreateDD3D11On12TargetTexture();
			return;
		}
		if (PrevImGfxCtxHandle != ImGfxCtx->TargetSharedHandle)
		{
			PrevImGfxCtxHandle = ImGfxCtx->TargetSharedHandle;
			D3D11SharedTextureFromImGfxCtx.Reset();

			ComPtr<ID3D11Resource> ImSharedResource;
			hr = D3D11Device->OpenSharedResource(ImGfxCtx->TargetSharedHandle, IID_PPV_ARGS(&ImSharedResource));
			HrFail_R(hr, TEXT("Failed to open the immediate shared resource in the RHI render thread."));

			ImSharedResource.As(&D3D11SharedTextureFromImGfxCtx);
		}

		ComPtr<ID3D11Resource> D3D11On12TargetResource;
		if(!D3D11On12TargetTexture) return;
		hr = D3D11On12TargetTexture.As(&D3D11On12TargetResource);
		HrFail_R(hr, TEXT("Couldn't query generic resource of target D3D11 on 12 wrapped texture."));

		D3D11On12Device->AcquireWrappedResources(D3D11On12TargetResource.GetAddressOf(), 1);
		D3D11Context->CopyResource(D3D11On12TargetResource.Get(), D3D11SharedTextureFromImGfxCtx.Get());
		D3D11On12Device->ReleaseWrappedResources(D3D11On12TargetResource.GetAddressOf(), 1);
		D3D11Context->Flush();
		
	});
}

void UD3D12SharedTexture::InvalidateUeResources(int InWidth, int InHeight, EPixelFormat InFormat)
{
	Width = InWidth; Height = InHeight; Format = InFormat;
	TargetTexture = UTexture2D::CreateTransient(InWidth, InHeight, InFormat, TextureName);
	TargetTexture->UpdateResource();
	D3D11On12TargetTexture.Reset();
}

bool UD3D12SharedTexture::IsApplicable()
{
	return (UR_CHROM_COMPAT_USE_NTHANDLE()) && CurrentRHI().Equals("D3D12");
}
#endif