/******************** (C) Copyright 2021 realworld one GmbH & Co. KG All Rights Reserved. ********************/

#include "SharedTexture/D3D11/D3D11LegacySharedTexture.h"

#include "Uranium.h"
#include "SharedTexture/D3DErrorHandling.h"

ID3D11Device* UD3D11LegacySharedTexture::GetRhiDevice()
{
	return static_cast<ID3D11Device*>(GDynamicRHI->RHIGetNativeDevice());
}

ID3D11DeviceContext* UD3D11LegacySharedTexture::GetImmediateContext()
{
	ID3D11DeviceContext* res;
	GetRhiDevice()->GetImmediateContext(&res);
	return res;
}

#if !(UR_CHROM_COMPAT_USE_NTHANDLE()) || UR_ST_DEVMODE
void UD3D11LegacySharedTexture::Initialize()
{
}

void UD3D11LegacySharedTexture::OnAcceleratedPaint(void* handle)
{
	if(PreviousHandle == handle)
	{
		return;
	}
	PreviousHandle = handle;

	if(SharedTexture)
	{
		SharedTexture.Reset();
	}
	ComPtr<ID3D11Resource> sharedResource;

	HRESULT hr = GetRhiDevice()->OpenSharedResource(handle, IID_PPV_ARGS(&sharedResource));
	HrFail_R(hr, TEXT("Couldn't open shared resource"));

	hr = sharedResource.As(&SharedTexture);
	HrFail_R(hr, TEXT("Shared resource was not a texture"));

	D3D11_TEXTURE2D_DESC texDesc;
	SharedTexture->GetDesc(&texDesc);

	EPixelFormat ueFormat = FromDXGIFormat(texDesc.Format);

	if(
		Width != texDesc.Width
		|| Height != texDesc.Height
		|| Format != ueFormat
	) {
		InvalidateUeResources(texDesc.Width, texDesc.Height, ueFormat);
	}
}

void UD3D11LegacySharedTexture::Render()
{
	if (!SharedTexture || !TargetTexture->IsValidLowLevelFast())
	{
		return;
	}
	ENQUEUE_RENDER_COMMAND(void)([this](FRHICommandListImmediate& cmdList)
	{
		if (!SharedTexture)
		{
			return;
		}
		
#if UE_VERSION >= MAKE_UE_VERSION(4, 26)
		
		if (!TargetTexture->Resource)
		{
			return;
		}

		FRHITexture2D* rhiRes = TargetTexture->Resource->GetTexture2DRHI();
		if (!rhiRes)
		{
			return;
		}
		
#else
		
		auto targetRes = static_cast<FTexture2DResource*>(TargetTexture->Resource);
		if (!targetRes)
		{
			return;
		}

		FRHITexture2D* rhiRes = targetRes->GetTexture2DRHI();
		if (!rhiRes)
		{
			return;
		}
		
#endif

		ID3D11Texture2D* nativeRes = static_cast<ID3D11Texture2D*>(rhiRes->GetNativeResource());
		if (!nativeRes)
		{
			return;
		}

		GetImmediateContext()->CopyResource(nativeRes, SharedTexture.Get());
	});
}

void UD3D11LegacySharedTexture::InvalidateUeResources(int width, int height, EPixelFormat format)
{
	Width = width; Height = height; Format = format;
	TargetTexture = UTexture2D::CreateTransient(width, height, format, TextureName);
	TargetTexture->UpdateResource();
}

bool UD3D11LegacySharedTexture::IsApplicable()
{
	return !(UR_CHROM_COMPAT_USE_NTHANDLE()) && CurrentRHI().Equals("D3D11");
}
#endif