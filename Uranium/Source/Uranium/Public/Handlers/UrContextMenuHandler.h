/******************** (C) Copyright 2021 realworld one GmbH & Co. KG All Rights Reserved. ********************/

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UObject/WeakInterfacePtr.h"
#include "UrCefHandler.h"

#include "CefIncludesStart.h"
#include "include/cef_client.h"
#include "CefIncludesEnd.h"

#include "UrContextMenuHandler.generated.h"

class IUrContextMenuHandler;

/**
 * Callback interface used for continuation of custom context menu display.
 */
class URANIUM_API ICefContextMenuHandler : public CefContextMenuHandler 
{
	friend IUrContextMenuHandler;
public:
	TWeakInterfacePtr<IUrContextMenuHandler> UrContextMenuHandler;
};

UINTERFACE(Blueprintable)
class URANIUM_API UUrContextMenuHandler : public UInterface
{
	GENERATED_BODY()
};

/**
 * Callback interface used for continuation of custom context menu display.
 */
class URANIUM_API IUrContextMenuHandler
{
	GENERATED_BODY()
	friend ICefContextMenuHandler;
public:
	CefRefPtr<ICefContextMenuHandler> CefContextMenuHandler = nullptr;
};

UCLASS(BlueprintType)
class URANIUM_API UUrContextMenuEvents : public UObject
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintPure, Category="Uranium")
	static UUrContextMenuEvents* GetEvents(TScriptInterface<IUrContextMenuHandler> handler);
};