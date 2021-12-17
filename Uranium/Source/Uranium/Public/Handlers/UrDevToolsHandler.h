/******************** (C) Copyright 2021 realworld one GmbH & Co. KG All Rights Reserved. ********************/

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UObject/WeakInterfacePtr.h"
#include "UrCefHandler.h"

#include "CefIncludesStart.h"
#include "include/cef_client.h"
#include "CefIncludesEnd.h"

#include "UrDevToolsHandler.generated.h"

class IUrDevToolsHandler;
class UUrDevToolsEvents;

UINTERFACE()
class URANIUM_API UUrDevToolsHandler : public UInterface
{
	GENERATED_BODY()
};

/**
 * Determine how the chromium DevTool should be opened in Unreal Engine
 */
class URANIUM_API IUrDevToolsHandler
{
	GENERATED_BODY()
public:
	
	UFUNCTION(BlueprintNativeEvent, Category="Uranium")
	void ShowDevTools(UUrBrowserView* browser, FVector2D inspectElementAt, int width, int height);
};

UCLASS()
class URANIUM_API UUrDevToolEvents : public UObject
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintPure, Category="Uranium")
	static UUrDevToolEvents* GetEvents(TScriptInterface<IUrDevToolsHandler> handler);
};