// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "SharedTexture/D3D12/D3D12LegacySharedTexture.h"
#include "UeVersion.h"

#include "Uranium.h"

#define NV_AFTERMATH 0
#include "D3D12RHI/Private/D3D12RHIPrivate.h"
#include "SharedTexture/D3DErrorHandling.h"

ID3D12Device* UD3D12LegacySharedTexture::GetRhiDevice()
{
	return static_cast<ID3D12Device*>(GDynamicRHI->RHIGetNativeDevice());
}

ID3D12CommandQueue* UD3D12LegacySharedTexture::GetRhiCommandQueue(FRHICommandListImmediate& cmdList)
{
	FD3D12CommandContext& CmdCtx = static_cast<FD3D12CommandContext&>(cmdList.GetContext());
	return CmdCtx.CommandListHandle.GetCommandListManager()->GetD3DCommandQueue();
}

ID3D12GraphicsCommandList* UD3D12LegacySharedTexture::GetRhiGfxCmdList(FRHICommandListImmediate& cmdList)
{
	FD3D12CommandContext& CmdCtx = static_cast<FD3D12CommandContext&>(cmdList.GetContext());
	return CmdCtx.CommandListHandle.GraphicsCommandList();
}

ID3D12Resource* UD3D12LegacySharedTexture::GetTargetTextureResource()
{
#if UE_VERSION >= MAKE_UE_VERSION(4, 26)
	
	if(!TargetTexture)
	{
		return nullptr;
	}
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
	
	if(!TargetTexture)
	{
		return nullptr;
	}
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

bool UD3D12LegacySharedTexture::CreateDD3D11On12TargetTexture()
{
	ID3D12Resource* NativeRes = GetTargetTextureResource();
	if (!NativeRes)
	{
		return true;
	}

	D3D11_RESOURCE_FLAGS d3d11Flags = { D3D11_BIND_SHADER_RESOURCE };

	HRESULT hr = D3D11On12Device->CreateWrappedResource(
		NativeRes,
		&d3d11Flags,
		D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		IID_PPV_ARGS(&D3D11On12TargetTexture)
	);
	HrFail_R(hr, TEXT("Failed to wrap D3D12 resource of target texture into a D3D11 resource")) true;
	return false;
}

#if !(UR_CHROM_COMPAT_USE_NTHANDLE()) || UR_ST_DEVMODE
void UD3D12LegacySharedTexture::Initialize()
{
}

void UD3D12LegacySharedTexture::OnAcceleratedPaint(void* handle)
{
	if (!bIsInitialized)
	{
		InitQueuedHandle = handle;
		return;
	}
	if (PreviousHandle == handle)
	{
		return;
	}
	PreviousHandle = handle;

	if (SharedTexture) SharedTexture.Reset();
	ComPtr<ID3D11Resource> sharedResource;

	auto hr = D3D11Device->OpenSharedResource(
		static_cast<HANDLE>(handle),
		IID_PPV_ARGS(&sharedResource)
	);
	HrFail_R(hr, TEXT("Couldn't open shared resource"));

	hr = sharedResource.As(&SharedTexture);
	HrFail_R(hr, TEXT("Shared resource was not a texture"));

	D3D11_TEXTURE2D_DESC texDesc;
	SharedTexture->GetDesc(&texDesc);

	EPixelFormat ueFormat = FromDXGIFormat(texDesc.Format);

	if (Width != texDesc.Width
		|| Height != texDesc.Height
		|| Format != ueFormat
	) {
		InvalidateUeResources(texDesc.Width, texDesc.Height, ueFormat);
	}
}

void UD3D12LegacySharedTexture::Render()
{
	ENQUEUE_RENDER_COMMAND(void)([this](FRHICommandListImmediate& cmdList)
	{
		HRESULT hr;
		if(bFailure)
		{
			return;
		}

		if(!bIsInitialized)
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
			HrFail_R(hr, TEXT("Couldn't query D3D11 on 12 device from created D3D11 device."));

			if(InitQueuedHandle)
			{
				OnAcceleratedPaint(InitQueuedHandle);
			}
		}

		if(!D3D11On12TargetTexture)
		{
			CreateDD3D11On12TargetTexture();
			return;
		}

		ComPtr<ID3D11Resource> d3d11On12TargetResource;
		hr = D3D11On12TargetTexture.As(&d3d11On12TargetResource);
		HrFail_R(hr, TEXT("Couldn't query generic resource of target D3D11 on 12 wrapped texture."));

		D3D11On12Device->AcquireWrappedResources(d3d11On12TargetResource.GetAddressOf(), 1);
		D3D11Context->CopyResource(d3d11On12TargetResource.Get(), SharedTexture.Get());
		D3D11On12Device->ReleaseWrappedResources(d3d11On12TargetResource.GetAddressOf(), 1);
		D3D11Context->Flush();
	});
}

void UD3D12LegacySharedTexture::InvalidateUeResources(int width, int height, EPixelFormat format)
{
	AsyncTask(ENamedThreads::GameThread, [this, width, height, format]()
	{
		Width = width; Height = height; Format = format;
		TargetTexture = UTexture2D::CreateTransient(width, height, format, TextureName);
		TargetTexture->UpdateResource();
		D3D11On12TargetTexture.Reset();
	});
}

bool UD3D12LegacySharedTexture::IsApplicable()
{
	return !(UR_CHROM_COMPAT_USE_NTHANDLE()) && CurrentRHI().Equals("D3D12");
}
#endif