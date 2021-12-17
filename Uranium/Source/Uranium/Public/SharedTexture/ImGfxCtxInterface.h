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


#include "ImGfxCtxInterface.generated.h"

UINTERFACE()
class URANIUM_API UImGfxCtx : public UInterface
{
	GENERATED_BODY()
};

class URANIUM_API IImGfxCtx
{
	GENERATED_BODY()
public:
	bool bFailure = false;
	int Width = -1;
	int Height = -1;
	DXGI_FORMAT Format = DXGI_FORMAT_UNKNOWN;
	
	virtual void OnAcceleratedPaint(void* handle) { }
};