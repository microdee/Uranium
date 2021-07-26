// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"


#include "SharedTexture/SharedTextureInterface.h"

THIRD_PARTY_INCLUDES_START
#include "Windows/AllowWindowsPlatformTypes.h"

#include <wrl/client.h>
#include <d3d11.h>
#include <d3d11_1.h>
#include <d3d12.h>

#include "Windows/HideWindowsPlatformTypes.h"
THIRD_PARTY_INCLUDES_END

#include "D3D11SharedTexture.generated.h"

using namespace Microsoft::WRL;

class UD3D11ImGfxCtx;

UCLASS()
class URANIUM_API UD3D11SharedTexture : public UObject, public ISharedTexture
{
	GENERATED_BODY()
private:

	UPROPERTY()
	UTexture2D* TargetTexture;

	UPROPERTY()
	UD3D11ImGfxCtx* ImGfxCtx;

	static ID3D11Device* GetRhiDevice();
	static ID3D11DeviceContext* GetImmediateContext();

public:

#if (UR_CHROM_COMPAT_USE_NTHANDLE()) || UR_ST_DEVMODE
	virtual void Initialize() override;
	virtual void OnAcceleratedPaint(void* Handle) override;
	virtual void Render() override;
	virtual void InvalidateUeResources(int InWidth, int InHeight, EPixelFormat InFormat) override;
	virtual bool IsApplicable() override;
#endif

	virtual UTexture2D* GetTexture() override { return TargetTexture; }
};