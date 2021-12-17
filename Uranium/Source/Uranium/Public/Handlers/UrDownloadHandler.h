/******************** (C) Copyright 2021 realworld one GmbH & Co. KG All Rights Reserved. ********************/

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UObject/WeakInterfacePtr.h"
#include "UrCefHandler.h"

#include "CefIncludesStart.h"
#include "include/cef_client.h"
#include "CefIncludesEnd.h"

#include "UrDownloadHandler.generated.h"

class IUrDownloadHandler;
class UUrDownloadEvents;

/**
 * Callback interface used to asynchronously cancel a download.
 */
class URANIUM_API ICefDownloadHandler : public CefDownloadHandler 
{
	friend IUrDownloadHandler;
protected:
	UUrDownloadEvents* GetDownloadEvents();

public:
	TWeakInterfacePtr<IUrDownloadHandler> UrDownloadHandler;
};

UINTERFACE(Blueprintable)
class URANIUM_API UUrDownloadHandler : public UInterface
{
	GENERATED_BODY()
};

/**
 * Callback interface used to asynchronously cancel a download.
 */
class URANIUM_API IUrDownloadHandler
{
	GENERATED_BODY()
	friend ICefDownloadHandler;
public:
	CefRefPtr<ICefDownloadHandler> CefDownloadHandler = nullptr;
};

UCLASS(BlueprintType)
class URANIUM_API UUrDownloadEvents : public UObject
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintPure, Category="Uranium")
	static UUrDownloadEvents* GetEvents(TScriptInterface<IUrDownloadHandler> handler);
};