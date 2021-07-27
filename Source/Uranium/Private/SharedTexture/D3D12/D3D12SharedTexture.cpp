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

ID3D12CommandQueue* UD3D12SharedTexture::GetRhiCommandQueue(FRHICommandListImmediate& cmdList)
{
	FD3D12CommandContext& cmdCtx = static_cast<FD3D12CommandContext&>(cmdList.GetContext());
	return cmdCtx.CommandListHandle.GetCommandListManager()->GetD3DCommandQueue();
}

ID3D12GraphicsCommandList* UD3D12SharedTexture::GetRhiGfxCmdList(FRHICommandListImmediate& cmdList)
{
	FD3D12CommandContext& cmdCtx = static_cast<FD3D12CommandContext&>(cmdList.GetContext());
	return cmdCtx.CommandListHandle.GraphicsCommandList();
}

ID3D12Resource* UD3D12SharedTexture::GetTargetTextureResource()
{
	if (!TargetTexture)
	{
		return nullptr;
	}
	
#if UE_VERSION >= MAKE_UE_VERSION(4, 26)
		
	if (!TargetTexture->Resource)
	{
		return nullptr;
	}

	FRHITexture2D* rhiRes = TargetTexture->Resource->GetTexture2DRHI();
	if (!rhiRes)
	{
		return nullptr;
	}
	
#else

	auto targetRes = static_cast<FTexture2DResource*>(TargetTexture->Resource);
	if (!targetRes)
	{
		return nullptr;
	}

	FRHITexture2D* rhiRes = targetRes->GetTexture2DRHI();
	if (!rhiRes)
	{
		return nullptr;
	}
	
#endif

	return static_cast<ID3D12Resource*>(rhiRes->GetNativeResource());
}

bool UD3D12SharedTexture::CreateDD3D11On12TargetTexture()
{
	ID3D12Resource* nativeRes = GetTargetTextureResource();
	if (!nativeRes)
	{
		return true;
	}

	D3D11_RESOURCE_FLAGS d3d11Flags = { D3D11_BIND_SHADER_RESOURCE };

	auto hr = D3D11On12Device->CreateWrappedResource(
		nativeRes,
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

void UD3D12SharedTexture::OnAcceleratedPaint(void* handle)
{
	ImGfxCtx->OnAcceleratedPaint(handle);

	if (PreviousHandle == handle)
	{
		return;
	}
	
	PreviousHandle = handle;

	EPixelFormat ueFormat = FromDXGIFormat(ImGfxCtx->Format);

	if (Width != ImGfxCtx->Width
		|| Height != ImGfxCtx->Height
		|| Format != ueFormat
	) {
		InvalidateUeResources(ImGfxCtx->Width, ImGfxCtx->Height, ueFormat);
	}
}

void UD3D12SharedTexture::Render()
{
	if (!TargetTexture || !TargetTexture->IsValidLowLevelFast())
	{
		return;
	}
	
	ENQUEUE_RENDER_COMMAND(void)([this](FRHICommandListImmediate& cmdList)
	{
		HRESULT hr;
		if (bFailure)
		{
			return;
		}

		if (!bIsInitialized)
		{
			bIsInitialized = true;
			D3D_FEATURE_LEVEL FeatureLevels[4] = {
				D3D_FEATURE_LEVEL_11_1,
				D3D_FEATURE_LEVEL_11_0,
				D3D_FEATURE_LEVEL_10_1,
				D3D_FEATURE_LEVEL_10_0
			};

			ID3D12CommandQueue* cmdQueue = GetRhiCommandQueue(cmdList);

			hr = D3D11On12CreateDevice(
				GetRhiDevice(),
				0,
				&FeatureLevels[0],
				4,
				reinterpret_cast<IUnknown**>(&cmdQueue),
				1,
				0,
				&D3D11Device,
				&D3D11Context,
				nullptr
			);

			HrFail_R(hr, TEXT("Couldn't create D3D11 on 12 device."));

			hr = D3D11Device.As(&D3D11On12Device);
			HrFail_R(hr, TEXT("Couldn't query D3D11on12 device from created D3D11 device."));
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

			ComPtr<ID3D11Resource> imSharedResource;
			hr = D3D11Device->OpenSharedResource(ImGfxCtx->TargetSharedHandle, IID_PPV_ARGS(&imSharedResource));
			HrFail_R(hr, TEXT("Failed to open the immediate shared resource in the RHI render thread."));

			imSharedResource.As(&D3D11SharedTextureFromImGfxCtx);
		}

		if(!D3D11On12TargetTexture)
		{
			return;
		}
		
		ComPtr<ID3D11Resource> d3d11On12TargetResource;
		hr = D3D11On12TargetTexture.As(&d3d11On12TargetResource);
		HrFail_R(hr, TEXT("Couldn't query generic resource of target D3D11 on 12 wrapped texture."));

		D3D11On12Device->AcquireWrappedResources(d3d11On12TargetResource.GetAddressOf(), 1);
		D3D11Context->CopyResource(d3d11On12TargetResource.Get(), D3D11SharedTextureFromImGfxCtx.Get());
		D3D11On12Device->ReleaseWrappedResources(d3d11On12TargetResource.GetAddressOf(), 1);
		D3D11Context->Flush();
		
	});
}

void UD3D12SharedTexture::InvalidateUeResources(int width, int height, EPixelFormat format)
{
	Width = width; Height = height; Format = format;
	TargetTexture = UTexture2D::CreateTransient(width, height, format, TextureName);
	TargetTexture->UpdateResource();
	D3D11On12TargetTexture.Reset();
}

bool UD3D12SharedTexture::IsApplicable()
{
	return (UR_CHROM_COMPAT_USE_NTHANDLE()) && CurrentRHI().Equals("D3D12");
}
#endif