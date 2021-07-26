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

#include "D3D11LegacySharedTexture.generated.h"

using namespace Microsoft::WRL;

UCLASS()
class URANIUM_API UD3D11LegacySharedTexture : public UObject, public ISharedTexture
{
	GENERATED_BODY()
private:
	ComPtr<ID3D11Texture2D> SharedTexture;

	UPROPERTY()
	UTexture2D* TargetTexture;

	static ID3D11Device* GetRhiDevice();
	static ID3D11DeviceContext* GetImmediateContext();

public:

#if !(UR_CHROM_COMPAT_USE_NTHANDLE()) || UR_ST_DEVMODE
	virtual void Initialize() override;
	virtual void OnAcceleratedPaint(void* handle) override;
	virtual void Render() override;
	virtual void InvalidateUeResources(int width, int height, EPixelFormat format) override;
	virtual bool IsApplicable() override;
#endif

	virtual UTexture2D* GetTexture() override { return TargetTexture; }
};