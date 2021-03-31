// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "SharedTexture/D3D11/D3D11SharedTexture.h"
#include "SharedTexture/D3D11/D3D11ImGfxCtx.h"

#include "Uranium.h"

ID3D11Device* UD3D11SharedTexture::GetRhiDevice()
{
    return static_cast<ID3D11Device*>(GDynamicRHI->RHIGetNativeDevice());
}

ID3D11DeviceContext* UD3D11SharedTexture::GetImmediateContext()
{
    ID3D11DeviceContext* res;
    GetRhiDevice()->GetImmediateContext(&res);
    return res;
}

#if (UR_CHROM_COMPAT_USE_NTHANDLE()) || UR_ST_DEVMODE
void UD3D11SharedTexture::Initialize()
{
    ImGfxCtx = NewObject<UD3D11ImGfxCtx>();
}

void UD3D11SharedTexture::OnAcceleratedPaint(void* Handle)
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

void UD3D11SharedTexture::Render()
{
    if(!TargetTexture->IsValidLowLevelFast()) return;
    ENQUEUE_RENDER_COMMAND(void)([this](FRHICommandListImmediate& RHICmdList)
    {
        auto TargetRes = static_cast<FTexture2DResource*>(TargetTexture->Resource);
        if (!TargetRes) return;

        auto RhiRes = TargetRes->GetTexture2DRHI();
        if (!RhiRes) return;

        auto NativeRes = static_cast<ID3D11Texture2D*>(RhiRes->GetNativeResource());
        if (!NativeRes) return;

        GetImmediateContext()->CopyResource(NativeRes, ImGfxCtx->TargetTexture.Get());
    });
}

void UD3D11SharedTexture::InvalidateUeResources(int InWidth, int InHeight, EPixelFormat InFormat)
{
    Width = InWidth; Height = InHeight; Format = InFormat;
    TargetTexture = UTexture2D::CreateTransient(InWidth, InHeight, InFormat, TextureName);
    TargetTexture->UpdateResource();
}

bool UD3D11SharedTexture::IsApplicable()
{
    return (UR_CHROM_COMPAT_USE_NTHANDLE()) && CurrentRHI().Equals("D3D11");
}
#endif