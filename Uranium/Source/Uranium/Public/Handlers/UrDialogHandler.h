/******************** (C) Copyright 2021 realworld one GmbH & Co. KG All Rights Reserved. ********************/

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UObject/WeakInterfacePtr.h"
#include "UrCefHandler.h"

#include "CefIncludesStart.h"
#include "include/cef_client.h"
#include "CefIncludesEnd.h"

#include "UrDialogHandler.generated.h"

class IUrDialogHandler;

/**
 * Implement this interface to handle dialog events.
 * The methods of this class will be called on the browser process UI thread.
 */
class URANIUM_API ICefDialogHandler : public CefDialogHandler 
{
	friend IUrDialogHandler;
public:
	TWeakInterfacePtr<IUrDialogHandler> UrDialogHandler;
};

UINTERFACE(Blueprintable)
class URANIUM_API UUrDialogHandler : public UInterface
{
	GENERATED_BODY()
};

/**
 * Implement this interface to handle dialog events.
 * The methods of this class will be called on the browser process UI thread.
 */
class URANIUM_API IUrDialogHandler
{
	GENERATED_BODY()
	friend ICefDialogHandler;
public:
	CefRefPtr<ICefDialogHandler> CefDialogHandler = nullptr;
};

UCLASS(BlueprintType)
class URANIUM_API UUrDialogEvents : public UObject
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintPure, Category="Uranium")
	static UUrDialogEvents* GetEvents(TScriptInterface<IUrDialogHandler> handler);
};