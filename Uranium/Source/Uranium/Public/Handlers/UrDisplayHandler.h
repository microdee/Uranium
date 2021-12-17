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

#include "UrDisplayHandler.generated.h"

class IUrDisplayHandler;
class UUrDisplayEvents;

// DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams  (FOnAddressChangeDel, UUrBrowserView*, browser, FString, url);
DECLARE_MULTICAST_DELEGATE_TwoParams          (FOnAddressChangeStaticDel, UUrBrowserView*, FString);

// DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams  (FOnAutoResizeDel, UUrBrowserView*, browser, FVector2D, newSize);
DECLARE_MULTICAST_DELEGATE_TwoParams          (FOnAutoResizeStaticDel, UUrBrowserView*, FVector2D);

// DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams (FOnConsoleMessageDel, UUrBrowserView*, browser, EUrLogSeverity, level, FString, message, FString, source, int, line);
DECLARE_MULTICAST_DELEGATE_FiveParams         (FOnConsoleMessageStaticDel, UUrBrowserView*, EUrLogSeverity, FString, FString, int);

// DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams  (FOnFullscreenModeChangeDel, UUrBrowserView*, browser, bool, fullscreen);
DECLARE_MULTICAST_DELEGATE_TwoParams          (FOnFullscreenModeChangeStaticDel, UUrBrowserView*, bool);

// DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams  (FOnLoadingProgressChangeDel, UUrBrowserView*, browser, float, progress);
DECLARE_MULTICAST_DELEGATE_TwoParams          (FOnLoadingProgressChangeStaticDel, UUrBrowserView*, float);

// DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams  (FOnTitleChangeDel, UUrBrowserView*, browser, FString, title);
DECLARE_MULTICAST_DELEGATE_TwoParams          (FOnTitleChangeStaticDel, UUrBrowserView*, FString);

// DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams  (FOnStatusMessageDel, UUrBrowserView*, browser, FString, status);
DECLARE_MULTICAST_DELEGATE_TwoParams          (FOnStatusMessageStaticDel, UUrBrowserView*, FString);

// DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams  (FOnTooltipDel, UUrBrowserView*, browser, FString, text);
DECLARE_MULTICAST_DELEGATE_TwoParams          (FOnTooltipStaticDel, UUrBrowserView*, FString);

/**
 * Implement this interface to handle events related to browser display state.
 * The methods of this class will be called on the UI thread.
 */
class URANIUM_API ICefDisplayHandler : public CefDisplayHandler 
{
	friend IUrDisplayHandler;
protected:
	UUrDisplayEvents* GetDisplayEvents();
	
public:
	TWeakInterfacePtr<IUrDisplayHandler> UrDisplayHandler;
	
	virtual void OnAddressChange(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		const CefString& url
	) override;
	
	virtual bool OnAutoResize(CefRefPtr<CefBrowser> browser, const CefSize& newSize) override;

	virtual bool OnConsoleMessage(
		CefRefPtr<CefBrowser> browser,
		cef_log_severity_t level,
		const CefString& message,
		const CefString& source,
		int line
	) override;

	virtual void OnFullscreenModeChange(CefRefPtr<CefBrowser> browser, bool fullscreen) override;
	virtual void OnLoadingProgressChange(CefRefPtr<CefBrowser> browser, double progress) override;
	virtual void OnStatusMessage(CefRefPtr<CefBrowser> browser, const CefString& value) override;
	virtual void OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title) override;
	virtual bool OnTooltip(CefRefPtr<CefBrowser> browser, CefString& text) override;

	// TODO: OnFaviconURLChange
};

UINTERFACE(Blueprintable)
class URANIUM_API UUrDisplayHandler : public UInterface
{
	GENERATED_BODY()
};

/**
 * Implement this interface to handle events related to browser display state.
 * The methods of this class will be called on the UI thread.
 */
class URANIUM_API IUrDisplayHandler
{
	GENERATED_BODY()
	friend ICefDisplayHandler;
public:
	CefRefPtr<ICefDisplayHandler> CefDisplayHandler = nullptr;

	UFUNCTION(BlueprintNativeEvent, Category="Uranium")
	void OnAddressChange(UUrBrowserView* browser, const FString& url);

	UFUNCTION(BlueprintNativeEvent, Category="Uranium")
	bool OnAutoResize(UUrBrowserView* browser, FVector2D newSize);

	UFUNCTION(BlueprintNativeEvent, Category="Uranium")
	void OnConsoleMessage(UUrBrowserView* browser, EUrLogSeverity level, const FString& message, const FString& source, int line);

	UFUNCTION(BlueprintNativeEvent, Category="Uranium")
	void OnFullscreenModeChange(UUrBrowserView* browser, bool fullscreen);

	UFUNCTION(BlueprintNativeEvent, Category="Uranium")
	void OnLoadingProgressChange(UUrBrowserView* browser, float progress);
	
	UFUNCTION(BlueprintNativeEvent, Category="Uranium")
	void OnTitleChange(UUrBrowserView* browser, const FString& title);
	
	UFUNCTION(BlueprintNativeEvent, Category="Uranium")
	void OnTooltip(UUrBrowserView* browser, const FString& text);
	
	UFUNCTION(BlueprintNativeEvent, Category="Uranium")
	void OnStatusMessage(UUrBrowserView* browser, const FString& status);

	UFUNCTION(BlueprintNativeEvent, Category="Uranium")
	float GetCachedLoadingProgress();

	UFUNCTION(BlueprintNativeEvent, Category="Uranium")
	FString GetCachedTitle();

	UFUNCTION(BlueprintNativeEvent, Category="Uranium")
	FString GetCachedAddress();
};

UCLASS()
class URANIUM_API UUrDisplayHandlerStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure, Category="Uranium")
	static float GetCachedLoadingProgress(TScriptInterface<IUrDisplayHandler> handler)
	{
		return IUrDisplayHandler::Execute_GetCachedLoadingProgress(handler.GetObject());
	}
	
	UFUNCTION(BlueprintPure, Category="Uranium")
	static FString GetCachedTitle(TScriptInterface<IUrDisplayHandler> handler)
	{
		return IUrDisplayHandler::Execute_GetCachedTitle(handler.GetObject());
	}

	UFUNCTION(BlueprintPure, Category="Uranium")
	static FString GetCachedAddress(TScriptInterface<IUrDisplayHandler> handler)
	{
		return IUrDisplayHandler::Execute_GetCachedAddress(handler.GetObject());
	}
};

UCLASS(BlueprintType)
class URANIUM_API UUrDisplayEvents : public UObject
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintPure, Category="Uranium")
	static UUrDisplayEvents* GetEvents(TScriptInterface<IUrDisplayHandler> handler);
	
	// UPROPERTY(BlueprintAssignable, Category = "Uranium")
	// FOnAddressChangeDel OnAddressChange;
	FOnAddressChangeStaticDel OnAddressChangeStatic;
	
	// UPROPERTY(BlueprintAssignable, Category = "Uranium")
	// FOnAutoResizeDel OnAutoResize;
	FOnAutoResizeStaticDel OnAutoResizeStatic;
	
	// UPROPERTY(BlueprintAssignable, Category = "Uranium")
	// FOnConsoleMessageDel OnConsoleMessage;
	FOnConsoleMessageStaticDel OnConsoleMessageStatic;

	// UPROPERTY(BlueprintAssignable, Category = "Uranium")
	// FOnFullscreenModeChangeDel OnFullscreenModeChange;
	FOnFullscreenModeChangeStaticDel OnFullscreenModeChangeStatic;
	
	// UPROPERTY(BlueprintAssignable, Category = "Uranium")
	// FOnLoadingProgressChangeDel OnLoadingProgressChange;
	FOnLoadingProgressChangeStaticDel OnLoadingProgressChangeStatic;
	
	// UPROPERTY(BlueprintAssignable, Category = "Uranium")
	// FOnTitleChangeDel OnTitleChange;
	FOnTitleChangeStaticDel OnTitleChangeStatic;
	
	// UPROPERTY(BlueprintAssignable, Category = "Uranium")
	// FOnStatusMessageDel OnStatusMessage;
	FOnStatusMessageStaticDel OnStatusMessageStatic;
	
	// UPROPERTY(BlueprintAssignable, Category = "Uranium")
	// FOnTooltipDel OnTooltip;
	FOnTooltipStaticDel OnTooltipStatic;
};