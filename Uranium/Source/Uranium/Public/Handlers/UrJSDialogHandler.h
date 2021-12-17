/******************** (C) Copyright 2021 realworld one GmbH & Co. KG All Rights Reserved. ********************/

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UObject/WeakInterfacePtr.h"
#include "UrCefHandler.h"

#include "CefIncludesStart.h"
#include "include/cef_client.h"
#include "CefIncludesEnd.h"

#include "UrJSDialogHandler.generated.h"

class IUrJSDialogHandler;
class UUrJSDialogEvents;

/**
 * Callback interface used for asynchronous continuation of JavaScript dialog requests.
 */
class URANIUM_API ICefJSDialogHandler : public CefJSDialogHandler 
{
	friend IUrJSDialogHandler;
protected:
	UUrJSDialogEvents* GetJSDialogEvents();

public:
	TWeakInterfacePtr<IUrJSDialogHandler> UrJSDialogHandler;
};

UINTERFACE(Blueprintable)
class URANIUM_API UUrJSDialogHandler : public UInterface
{
	GENERATED_BODY()
};

/**
 * Callback interface used for asynchronous continuation of JavaScript dialog requests.
 */
class URANIUM_API IUrJSDialogHandler
{
	GENERATED_BODY()
	friend ICefJSDialogHandler;
public:
	CefRefPtr<ICefJSDialogHandler> CefJSDialogHandler = nullptr;
};

UCLASS(BlueprintType)
class URANIUM_API UUrJSDialogEvents : public UObject
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintPure, Category="Uranium")
	static UUrJSDialogEvents* GetEvents(TScriptInterface<IUrJSDialogHandler> handler);
};