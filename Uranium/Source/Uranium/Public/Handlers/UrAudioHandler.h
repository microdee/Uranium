/******************** (C) Copyright 2021 realworld one GmbH & Co. KG All Rights Reserved. ********************/

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UObject/WeakInterfacePtr.h"
#include "UrCefHandler.h"

#include "CefIncludesStart.h"
#include "include/cef_client.h"
#include "CefIncludesEnd.h"

#include "UrAudioHandler.generated.h"

class IUrAudioHandler;
class UUrAudioEvents;

/**
 * Implement this interface to handle audio events.
 */
class URANIUM_API ICefAudioHandler : public CefAudioHandler 
{
	friend IUrAudioHandler;
protected:
	UUrAudioEvents* GetAudioEvents();

public:
	TWeakInterfacePtr<IUrAudioHandler> UrAudioHandler;
};

UINTERFACE()
class URANIUM_API UUrAudioHandler : public UInterface
{
	GENERATED_BODY()
};

/**
 * Implement this interface to handle audio events.
 */
class URANIUM_API IUrAudioHandler
{
	GENERATED_BODY()
	friend ICefAudioHandler;
public:
	CefRefPtr<ICefAudioHandler> CefAudioHandler = nullptr;
};

UCLASS()
class URANIUM_API UUrAudioEvents : public UObject
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintPure, Category="Uranium")
	static UUrAudioEvents* GetEvents(TScriptInterface<IUrAudioHandler> handler);
};