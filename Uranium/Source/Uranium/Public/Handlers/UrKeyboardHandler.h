/******************** (C) Copyright 2021 realworld one GmbH & Co. KG All Rights Reserved. ********************/

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UObject/WeakInterfacePtr.h"
#include "UrCefHandler.h"

#include "CefIncludesStart.h"
#include "include/cef_client.h"
#include "CefIncludesEnd.h"

#include "UrKeyboardHandler.generated.h"

class IUrKeyboardHandler;
class UUrKeyboardEvents;

/**
 * Implement this interface to handle events related to keyboard input.
 */
class URANIUM_API ICefKeyboardHandler : public CefKeyboardHandler 
{
	friend IUrKeyboardHandler;
protected:
	UUrKeyboardEvents* GetKeyboardEvents();

public:
	TWeakInterfacePtr<IUrKeyboardHandler> UrKeyboardHandler;
};

UINTERFACE(Blueprintable)
class URANIUM_API UUrKeyboardHandler : public UInterface
{
	GENERATED_BODY()
};

/**
 * Implement this interface to handle events related to keyboard input.
 */
class URANIUM_API IUrKeyboardHandler
{
	GENERATED_BODY()
	friend ICefKeyboardHandler;
public:
	CefRefPtr<ICefKeyboardHandler> CefKeyboardHandler = nullptr;
};

UCLASS(BlueprintType)
class URANIUM_API UUrKeyboardEvents : public UObject
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintPure, Category="Uranium")
	static UUrKeyboardEvents* GetEvents(TScriptInterface<IUrKeyboardHandler> handler);
};