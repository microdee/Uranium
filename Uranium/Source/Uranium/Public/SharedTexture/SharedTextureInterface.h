/******************** (C) Copyright 2021 realworld one GmbH & Co. KG All Rights Reserved. ********************/

#pragma once

#include "CoreMinimal.h"

THIRD_PARTY_INCLUDES_START
#include "Windows/AllowWindowsPlatformTypes.h"
#include "Windows/AllowWindowsPlatformAtomics.h"
#include "Windows/PreWindowsApi.h"

#include <dxgiformat.h>

#include "Windows/PostWindowsApi.h"
#include "Windows/HideWindowsPlatformAtomics.h"
#include "Windows/HideWindowsPlatformTypes.h"
THIRD_PARTY_INCLUDES_END

#include "CefCompat.h"

#include "SharedTextureInterface.generated.h"

#define UR_CHROM_COMPAT_USE_NTHANDLE() UR_CHROM_VERSION_MIN(79, 0, 0)

// disable before build
#define UR_ST_DEVMODE 0

class IImGfxCtx;
class UTexture2D;

UINTERFACE()
class URANIUM_API USharedTexture : public UInterface
{
	GENERATED_BODY()
};

class URANIUM_API ISharedTexture
{
	GENERATED_BODY()

protected:
	void* PreviousHandle = nullptr;
public:
	bool bFailure = false;
	int Width = -1;
	int Height = -1;
	EPixelFormat Format = PF_Unknown;

	FName TextureName = NAME_None;

	virtual void Initialize() { }
	virtual void OnAcceleratedPaint(void* handle) { }
	virtual void Render() { }
	virtual void InvalidateUeResources(int width, int height, EPixelFormat format) { }
	virtual bool IsApplicable() { return false; }
	virtual bool NeedsImmediateGfxContext() { return false; }
	virtual IImGfxCtx* GetImmediateGfxContext() { return nullptr; }
	virtual UTexture2D* GetTexture() { return nullptr; }

	static FString CurrentRHI();
	static EPixelFormat FromDXGIFormat(DXGI_FORMAT format);
	static DXGI_FORMAT FromUEFormat(EPixelFormat format);

	static TScriptInterface<ISharedTexture> CreateSharedTexture();
};