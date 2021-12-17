/******************** (C) Copyright 2021 realworld one GmbH & Co. KG All Rights Reserved. ********************/

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UObject/WeakInterfacePtr.h"
#include "UrCefHandler.h"

#include "CefIncludesStart.h"
#include "include/cef_client.h"
#include "CefIncludesEnd.h"

#include "UrFindHandler.generated.h"

class IUrFindHandler;
class UUrFindEvents;

/**
 * Implement this interface to handle events related to find results.
 */
class URANIUM_API ICefFindHandler : public CefFindHandler 
{
	friend IUrFindHandler;
protected:
	UUrFindEvents* GetFindEvents();

public:
	TWeakInterfacePtr<IUrFindHandler> UrFindHandler;
};

UINTERFACE(Blueprintable)
class URANIUM_API UUrFindHandler : public UInterface
{
	GENERATED_BODY()
};

/**
 * Implement this interface to handle events related to find results.
 */
class URANIUM_API IUrFindHandler
{
	GENERATED_BODY()
	friend ICefFindHandler;
public:
	CefRefPtr<ICefFindHandler> CefFindHandler = nullptr;
};

UCLASS(BlueprintType)
class URANIUM_API UUrFindEvents : public UObject
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintPure, Category="Uranium")
	static UUrFindEvents* GetEvents(TScriptInterface<IUrFindHandler> handler);
};