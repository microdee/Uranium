/******************** (C) Copyright 2021 realworld one GmbH & Co. KG All Rights Reserved. ********************/

#pragma once

#include "CoreMinimal.h"
#include "CefPtr.h"
#include "UObject/Interface.h"

#include "UraniumContext.generated.h"

class CefBrowser;
class FUraniumApp;
class UUrBrowserView;
class UUraniumSubsystem;

class FUrContextDetails;

UINTERFACE()
class URANIUM_API UUraniumContext : public UInterface
{
	GENERATED_BODY()
};

/**
 * Base interface for Uranium contexts.
 * An object implementing this interface is created in |UUraniumSubsystem|
 * which will orchestrate global CEF resources.
 */
class URANIUM_API IUraniumContext
{
	GENERATED_BODY()
private:
	TSharedPtr<FUrContextDetails> Details;

public:
	
	virtual TCefPtr<FUraniumApp> GetApp() = 0;
	virtual CefSettings GetCefSettings() = 0;
	virtual FString GetUserAgent() = 0;
	virtual void SetUserAgent(FString userAgent) = 0;
	virtual FString GetChromiumVersion();
	virtual FString GetCefVersion();

	/** Return true when all is ready to work with CEF */
	virtual bool IsCefRunning() = 0;

	/** Return true while CEF is loading */
	virtual bool IsInitializing() = 0;

	/**
	 * Implementer needs to check if it's safe to call and return early when it isn't.
	 */
	virtual void Initialize(UUraniumSubsystem* owner) = 0;

	/**
	* Implementer needs to check if it's safe to call and return early when it isn't.
	*/
	virtual void Deinitialize() = 0;

	FSimpleMulticastDelegate OnCefInitialized;
	FSimpleMulticastDelegate OnCefShutdown;
};
