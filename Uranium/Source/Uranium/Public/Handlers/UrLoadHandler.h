/******************** (C) Copyright 2021 realworld one GmbH & Co. KG All Rights Reserved. ********************/

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UObject/WeakInterfacePtr.h"
#include "UrCefHandler.h"

#include "CefIncludesStart.h"
#include "include/cef_client.h"
#include "CefIncludesEnd.h"
#include "UrCefEnums.h"

#include "UrLoadHandler.generated.h"

class IUrLoadHandler;
class UUrLoadEvents;

// DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams  (FOnLoadStartDel, UUrBrowserView*, browser, EUrTransitionType, transitionType);
DECLARE_MULTICAST_DELEGATE_TwoParams          (FOnLoadStartStaticDel, UUrBrowserView*, EUrTransitionType);

// DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams  (FOnLoadEndDel, UUrBrowserView*, browser, int, httpStatusCode);
DECLARE_MULTICAST_DELEGATE_TwoParams          (FOnLoadEndStaticDel, UUrBrowserView*, int);

// DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnLoadErrorDel, UUrBrowserView*, browser, FUrCefErrorCode, errorCode, FString, failedUrl);
DECLARE_MULTICAST_DELEGATE_ThreeParams        (FOnLoadErrorStaticDel, UUrBrowserView*, FUrCefErrorCode, FString);

// DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams (FOnLoadingStateChangeDel, UUrBrowserView*, browser, bool, isLoading, bool, canGoBack, bool, canGoForward);
DECLARE_MULTICAST_DELEGATE_FourParams         (FOnLoadingStateChangeStaticDel, UUrBrowserView*, bool, bool, bool);

/**
 * Implement this interface to handle events related to browser load status.
 * The methods of this class will be called on the browser process UI thread or
 * render process main thread (TID_RENDERER).
 */
class URANIUM_API ICefLoadHandler : public CefLoadHandler 
{
	friend IUrLoadHandler;
protected:
	UUrLoadEvents* GetLoadEvents();

public:
	TWeakInterfacePtr<IUrLoadHandler> UrLoadHandler;
	
	virtual void OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, TransitionType transitionType) override;
	virtual void OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode) override;
	
	virtual void OnLoadError(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		ErrorCode errorCode,
		const CefString& errorText,
		const CefString& failedUrl
	) override;

	virtual void OnLoadingStateChange(
		CefRefPtr<CefBrowser> browser,
		bool isLoading,
		bool canGoBack,
		bool canGoForward
	) override;
};

UINTERFACE(Blueprintable)
class URANIUM_API UUrLoadHandler : public UInterface
{
	GENERATED_BODY()
};

/**
 * Implement this interface to handle events related to browser load status.
 * The methods of this class will be called on the browser process UI thread or
 * render process main thread (TID_RENDERER).
 */
class URANIUM_API IUrLoadHandler
{
	GENERATED_BODY()
	friend ICefLoadHandler;
public:
	CefRefPtr<ICefLoadHandler> CefLoadHandler = nullptr;
	
	UFUNCTION(BlueprintNativeEvent, Category="Uranium")
	void OnLoadStart(UUrBrowserView* browser, EUrTransitionType transitionType);

	UFUNCTION(BlueprintNativeEvent, Category="Uranium")
	void OnLoadEnd(UUrBrowserView* browser, int httpStatusCode);

	UFUNCTION(BlueprintNativeEvent, Category="Uranium")
	void OnLoadError(UUrBrowserView* browser, FUrCefErrorCode errorCode, const FString& failedUrl);

	UFUNCTION(BlueprintNativeEvent, Category="Uranium")
	void OnLoadingStateChange(UUrBrowserView* browser, bool isLoading, bool canGoBack, bool canGoForward);

	UFUNCTION(BlueprintNativeEvent, Category="Uranium")
	bool IsLoading();
};

/**
 * BlueprintPure accessors for persistent states
 */
UCLASS(BlueprintType)
class URANIUM_API UUrLoadHandlerStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintPure, Category="Uranium")
	static bool IsLoading(TScriptInterface<IUrLoadHandler> handler)
	{
		return IUrLoadHandler::Execute_IsLoading(handler.GetObject());
	}
};

UCLASS(BlueprintType)
class URANIUM_API UUrLoadEvents : public UObject
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintPure, Category="Uranium")
	static UUrLoadEvents* GetEvents(TScriptInterface<IUrLoadHandler> handler);

	// UPROPERTY(BlueprintAssignable, Category = "Uranium")
	// FOnLoadStartDel OnLoadStart;
	FOnLoadStartStaticDel OnLoadStartStatic;

	// UPROPERTY(BlueprintAssignable, Category = "Uranium")
	// FOnLoadEndDel OnLoadEnd;
	FOnLoadEndStaticDel OnLoadEndStatic;
	
	// UPROPERTY(BlueprintAssignable, Category = "Uranium")
	// FOnLoadErrorDel OnLoadError;
	FOnLoadErrorStaticDel OnLoadErrorStatic;
	
	// UPROPERTY(BlueprintAssignable, Category = "Uranium")
	// FOnLoadingStateChangeDel OnLoadingStateChange;
	FOnLoadingStateChangeStaticDel OnLoadingStateChangeStatic;
};