/******************** (C) Copyright 2021 realworld one GmbH & Co. KG All Rights Reserved. ********************/

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UObject/WeakInterfacePtr.h"
#include "UrCefHandler.h"

#include "CefIncludesStart.h"
#include "include/cef_client.h"
#include "CefIncludesEnd.h"

#include "UrDragHandler.generated.h"

class IUrDragHandler;
class UUrDragEvents;

/**
 * Implement this interface to handle events related to dragging.
 */
class URANIUM_API ICefDragHandler : public CefDragHandler 
{
	friend IUrDragHandler;
protected:
	UUrDragEvents* GetDragEvents();

public:
	TWeakInterfacePtr<IUrDragHandler> UrDragHandler;
};

UINTERFACE(Blueprintable)
class URANIUM_API UUrDragHandler : public UInterface
{
	GENERATED_BODY()
};

/**
 * Implement this interface to handle events related to dragging.
 */
class URANIUM_API IUrDragHandler
{
	GENERATED_BODY()
	friend ICefDragHandler;
public:
	CefRefPtr<ICefDragHandler> CefDragHandler = nullptr;
};

UCLASS(BlueprintType)
class URANIUM_API UUrDragEvents : public UObject
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintPure, Category="Uranium")
	static UUrDragEvents* GetEvents(TScriptInterface<IUrDragHandler> handler);
};