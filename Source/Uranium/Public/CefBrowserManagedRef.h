// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include "CefBrowserView.h"

#include "CefBrowserManagedRef.generated.h"

/**
 * Just a simple wrapper for CefRefPtr<FCefBrowserView> so it is compatible with forward declaration
 */
UCLASS()
class UCefBrowserManagedRef : public UObject
{
    GENERATED_BODY()
public:

    CefRefPtr<FCefBrowserView> Obj;

    bool IsAvailable(bool bCheckBrowserReady = true);
};
