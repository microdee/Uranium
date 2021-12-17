/******************** (C) Copyright 2021 realworld one GmbH & Co. KG All Rights Reserved. ********************/

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UObject/WeakInterfacePtr.h"
#include "UrCefHandler.h"

#include "CefIncludesStart.h"
#include "include/cef_client.h"
#include "CefIncludesEnd.h"

#include "UrFocusHandler.generated.h"

class IUrFocusHandler;
class UUrFocusEvents;

/**
 * Implement this interface to handle events related to focus.
 */
class URANIUM_API ICefFocusHandler : public CefFocusHandler 
{
	friend IUrFocusHandler;
protected:
	UUrFocusEvents* GetFocusEvents();

public:
	TWeakInterfacePtr<IUrFocusHandler> UrFocusHandler;
};

UINTERFACE(Blueprintable)
class URANIUM_API UUrFocusHandler : public UInterface
{
	GENERATED_BODY()
};

/**
 * Implement this interface to handle events related to focus.
 */
class URANIUM_API IUrFocusHandler
{
	GENERATED_BODY()
	friend ICefFocusHandler;
public:
	CefRefPtr<ICefFocusHandler> CefFocusHandler = nullptr;
};

UCLASS(BlueprintType)
class URANIUM_API UUrFocusEvents : public UObject
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintPure, Category="Uranium")
	static UUrFocusEvents* GetEvents(TScriptInterface<IUrFocusHandler> handler);
};