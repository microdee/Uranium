/******************** (C) Copyright 2021 realworld one GmbH & Co. KG All Rights Reserved. ********************/

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UObject/WeakInterfacePtr.h"
#include "UrCefHandler.h"

#include "CefIncludesStart.h"
#include "include/cef_client.h"
#include "CefIncludesEnd.h"

#include "UrRequestHandler.generated.h"

class IUrRequestHandler;
class UUrRequestEvents;

// DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams (FOnBeforeBrowseDel, UUrBrowserView*, browser, FString, url, bool, userGesture, bool, isRedirect);
DECLARE_MULTICAST_DELEGATE_FourParams         (FOnBeforeBrowseStaticDel, UUrBrowserView*, FString, bool, bool);

/**
 * Implement this interface to handle events related to browser requests. The
 * methods of this class will be called on the thread indicated.
 */
class URANIUM_API ICefRequestHandler : public CefRequestHandler 
{
	friend IUrRequestHandler;
protected:
	UUrRequestEvents* GetRequestEvents();

public:
	TWeakInterfacePtr<IUrRequestHandler> UrRequestHandler;

	virtual bool OnBeforeBrowse(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefRequest> request,
		bool userGesture,
		bool isRedirect
	) override;

	// TODO: let the implementer handle cookies
	//bool CanGetCookies(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request) override;
	//bool CanSetCookie(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, const CefCookie& cookie) override;
	
	virtual void OnPluginCrashed(CefRefPtr<CefBrowser> browser, const CefString& pluginPath) override;
	virtual void OnRenderProcessTerminated(CefRefPtr<CefBrowser> browser, TerminationStatus status) override;
	virtual void OnRenderViewReady(CefRefPtr<CefBrowser> browser) override;
	
	// Might be used for LoadHtml
	virtual CefRefPtr<CefResourceRequestHandler> GetResourceRequestHandler(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefRequest> request,
		bool isNavigation,
		bool isDownload,
		const CefString& requestInitiator,
		bool& disableDefaultHandling
	) override;
};

UINTERFACE(Blueprintable)
class URANIUM_API UUrRequestHandler : public UInterface
{
	GENERATED_BODY()
};

/**
 * Implement this interface to handle events related to browser requests. The
 * methods of this class will be called on the thread indicated.
 */
class URANIUM_API IUrRequestHandler
{
	GENERATED_BODY()
	friend ICefRequestHandler;
public:
	CefRefPtr<ICefRequestHandler> CefRequestHandler = nullptr;

	UFUNCTION(BlueprintNativeEvent, Category="Uranium")
	bool OnBeforeBrowse(UUrBrowserView* browser, const FString& url, bool userGesture, bool isRedirect);
};

UCLASS(BlueprintType)
class URANIUM_API UUrRequestEvents : public UObject
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintPure, Category="Uranium")
	static UUrRequestEvents* GetEvents(TScriptInterface<IUrRequestHandler> handler);

	// UPROPERTY(BlueprintAssignable, Category = "Uranium")
	// FOnBeforeBrowseDel OnBeforeBrowse;
	FOnBeforeBrowseStaticDel OnBeforeBrowseStatic;
};