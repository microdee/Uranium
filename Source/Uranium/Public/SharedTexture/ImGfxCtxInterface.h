// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include <dxgiformat.h>

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
    
    virtual void OnAcceleratedPaint(void* Handle) { }
};