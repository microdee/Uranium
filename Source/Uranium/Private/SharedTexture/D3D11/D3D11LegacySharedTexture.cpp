// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

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

void UD3D11LegacySharedTexture::OnAcceleratedPaint(void* Handle)
{
	if(PreviousHandle == Handle) return;
	PreviousHandle = Handle;

	if(SharedTexture) SharedTexture.Reset();
	ComPtr<ID3D11Resource> SharedResource;

	auto hr = GetRhiDevice()->OpenSharedResource(
		static_cast<HANDLE>(Handle),
		IID_PPV_ARGS(&SharedResource)
	);
	HrFail_R(hr, TEXT("Couldn't open shared resource"));

	hr = SharedResource.As(&SharedTexture);
	HrFail_R(hr, TEXT("Shared resource was not a texture"));

	D3D11_TEXTURE2D_DESC texDesc;
	SharedTexture->GetDesc(&texDesc);

	auto UEFormat = FromDXGIFormat(texDesc.Format);

	if(
		Width != texDesc.Width
		|| Height != texDesc.Height
		|| Format != UEFormat
	) InvalidateUeResources(texDesc.Width, texDesc.Height, UEFormat);
}

void UD3D11LegacySharedTexture::Render()
{
	if (!SharedTexture || !TargetTexture->IsValidLowLevelFast()) return;
	ENQUEUE_RENDER_COMMAND(void)([this](FRHICommandListImmediate& RHICmdList)
	{
		if (!SharedTexture) return;

		auto TargetRes = static_cast<FTexture2DResource*>(TargetTexture->Resource);
		if(!TargetRes) return;

		auto RhiRes = TargetRes->GetTexture2DRHI();
		if(!RhiRes) return;

		auto NativeRes = static_cast<ID3D11Texture2D*>(RhiRes->GetNativeResource());
		if (!NativeRes) return;

		GetImmediateContext()->CopyResource(NativeRes, SharedTexture.Get());
	});
}

void UD3D11LegacySharedTexture::InvalidateUeResources(int InWidth, int InHeight, EPixelFormat InFormat)
{
	Width = InWidth; Height = InHeight; Format = InFormat;
	TargetTexture = UTexture2D::CreateTransient(InWidth, InHeight, InFormat, TextureName);
	TargetTexture->UpdateResource();
}

bool UD3D11LegacySharedTexture::IsApplicable()
{
	return !(UR_CHROM_COMPAT_USE_NTHANDLE()) && CurrentRHI().Equals("D3D11");
}
#endif