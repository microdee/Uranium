/******************** (C) Copyright 2021 realworld one GmbH & Co. KG All Rights Reserved. ********************/

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UObject/WeakInterfacePtr.h"
#include "UrCefHandler.h"
#include "NewBrowserMetadata.h"
#include "UrCommonEnums.h"

#include "CefIncludesStart.h"

#include "include/cef_client.h"
#include "CefCompat.h"

#include "CefIncludesEnd.h"

#include "UrLifeSpanHandler.generated.h"

class IUrLifeSpanHandler;
class UUrLifeSpanEvents;

class FCefBrowserClient;

/**
 * Delegate to be executed before the browser is 100% ready. Executed after view is created but
 * before deferred initialization. CEF browser is not yet created at this point
 */
// DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam   (FOnBeforeCreatedDel, UUrBrowserView*, browser);
DECLARE_MULTICAST_DELEGATE_OneParam           (FOnBeforeCreatedStaticDel, UUrBrowserView*);

/**
 * Delegate to be executed after the browser is ready. This is executed before any content is loaded by the browser
 */
// DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam   (FOnAfterCreatedDel, UUrBrowserView*, browser);
DECLARE_MULTICAST_DELEGATE_OneParam           (FOnAfterCreatedStaticDel, UUrBrowserView*);

/**
 * Delegate to be executed before the browser closes, allowing for also destroying associated widgets
 */
// DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam   (FOnBeforeCloseDel, UUrBrowserView*, browser);
DECLARE_MULTICAST_DELEGATE_OneParam           (FOnBeforeCloseStaticDel, UUrBrowserView*);

/**
 * Called when |NewWindowBehavior| is |EUrNewWindowBehavior::OpenInNewBrowser| a new uninitialized
 * UrBrowserView is created on "open in new window" type of actions, with extra metadata
 * about the desired properties of the newly opened browser instance
 * (like the size for a new popup, or the fact that it is a "popup-window").
 * It is the implementer's responsibility to listen to this event, create the appropriate widgets
 * either with or ignoring the metadata, then call |DeferredInit| on the new UrBrowser.
 */
// DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnNewWindowOpenedDel, UUrBrowserView*, parentBrowser, UUrBrowserView*, newBrowser, FNewBrowserMetadata, newBrowserMetadata);
DECLARE_MULTICAST_DELEGATE_ThreeParams        (FOnNewWindowOpenedStaticDel, UUrBrowserView*, UUrBrowserView*, FNewBrowserMetadata);

/**
 * Implement this interface to handle events related to browser life span. The
 * methods of this class will be called on the UI thread unless otherwise
 * indicated.
 */
class URANIUM_API ICefLifeSpanHandler : public CefLifeSpanHandler 
{
	friend IUrLifeSpanHandler;
private:
	bool bIsClosed = false;
protected:
	UUrLifeSpanEvents* GetLifeSpanEvents();

public:
	TWeakInterfacePtr<IUrLifeSpanHandler> UrLifeSpanHandler;
	
	virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) override;
	virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) override;

	virtual bool OnBeforePopup(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		const CefString& targetUrl,
		const CefString& targetFrameName,
		CefLifeSpanHandler::WindowOpenDisposition targetDisposition,
		bool userGesture,
		const CefPopupFeatures& popupFeatures,
		CefWindowInfo& windowInfo,
		CefRefPtr<CefClient>& client,
		CefBrowserSettings& settings,
#if UR_CHROM_COMPAT_EXTRAINFO()
		CefRefPtr<CefDictionaryValue>& extraInfo,
#endif
		bool* noJavascriptAccess
	) override;

	/**
	 * Set browser settings typically when a new window is opened.
	 */
	virtual void SetBrowserSettings(CefWindowInfo& winInfo, CefBrowserSettings& settings);

	bool CreateNewCefBrowser(UUrBrowserView* view, FVector2D initialSize, FString url, TCefPtr<FCefBrowserClient> client);
	bool IsClosed() const { return bIsClosed; }
};

UINTERFACE(Blueprintable)
class URANIUM_API UUrLifeSpanHandler : public UInterface
{
	GENERATED_BODY()
};

/**
 * Implement this interface to handle events related to browser life span. The
 * methods of this class will be called on the UI thread unless otherwise
 * indicated.
 */
class URANIUM_API IUrLifeSpanHandler
{
	GENERATED_BODY()
	friend ICefLifeSpanHandler;
private:
	void AssignBrowserToView(TCefPtr<CefBrowser> browser);
	void InvalidateBrowserAtView();
	
public:
	CefRefPtr<ICefLifeSpanHandler> CefLifeSpanHandler = nullptr;
	
	UFUNCTION(BlueprintNativeEvent, Category="Uranium")
	void OnBeforeCreated(UUrBrowserView* browser);
	
	UFUNCTION(BlueprintNativeEvent, Category="Uranium")
	void OnAfterCreated(UUrBrowserView* browser);
	
	UFUNCTION(BlueprintNativeEvent, Category="Uranium")
	void OnBeforeClose(UUrBrowserView* browser);

	UFUNCTION(BlueprintNativeEvent, Category="Uranium")
	void OnNewWindowOpened(UUrBrowserView* parentBrowser, UUrBrowserView* newBrowser, FNewBrowserMetadata newBrowserMetadata);
	
	UFUNCTION(BlueprintNativeEvent, Category="Uranium")
	EUrNewWindowBehavior GetNewWindowBehavior(UUrBrowserView* browser, FNewBrowserMetadata newWindowArguments);
};

UCLASS(BlueprintType)
class URANIUM_API UUrLifeSpanEvents : public UObject
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintPure, Category="Uranium")
	static UUrLifeSpanEvents* GetEvents(TScriptInterface<IUrLifeSpanHandler> handler);

	// UPROPERTY(BlueprintAssignable, Category = "Uranium")
	// FOnBeforeCreatedDel OnBeforeCreated;
	FOnBeforeCreatedStaticDel OnBeforeCreatedStatic;

	// UPROPERTY(BlueprintAssignable, Category = "Uranium")
	// FOnAfterCreatedDel OnAfterCreated;
	FOnAfterCreatedStaticDel OnAfterCreatedStatic;

	// UPROPERTY(BlueprintAssignable, Category = "Uranium")
	// FOnBeforeCloseDel OnBeforeClose;
	FOnBeforeCloseStaticDel OnBeforeCloseStatic;
	
	// UPROPERTY(BlueprintAssignable, Category = "Uranium")
	// FOnNewWindowOpenedDel OnNewWindowOpened;
	FOnNewWindowOpenedStaticDel OnNewWindowOpenedStatic;
};