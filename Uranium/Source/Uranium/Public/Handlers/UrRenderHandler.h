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

#include "UrRenderHandler.generated.h"

class IUrRenderResourceHandler;
class IUrRenderEventsHandler;
class UUrRenderEvents;


/**
* Delegate for the native popup visibility.
* In CEF-speak a popup also indicates a native UI element displayed above the HTML content,
* for example the dropdown menu of the <select> tag. To alleviate ambiguity of conflicting
* meanings they're called NativePopups in Uranium. Only one native popup should be ever
* visible above the main HTML content
*/
// DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams  (FOnNativePopupShowDel, UUrBrowserView*, browser, bool, show);
DECLARE_MULTICAST_DELEGATE_TwoParams          (FOnNativePopupShowStaticDel, UUrBrowserView*, bool);

/**
* Delegate for placing the supposedly shown native popup visibility.
* In CEF-speak a popup also indicates a native UI element displayed above the HTML content,
* for example the dropdown menu of the <select> tag. To alleviate ambiguity of conflicting
* meanings they're called NativePopups in Uranium Only one native popup should be ever
* visible above the main HTML content
*/
// DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnNativePopupSizeDel, UUrBrowserView*, browser, FVector2D, location, FVector2D, size);
DECLARE_MULTICAST_DELEGATE_ThreeParams        (FOnNativePopupSizeStaticDel, UUrBrowserView*, FVector2D, FVector2D);

// DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams  (FOnScrollOffsetChangedDel, UUrBrowserView*, browser, FVector2D, offset);
DECLARE_MULTICAST_DELEGATE_TwoParams          (FOnScrollOffsetChangedStaticDel, UUrBrowserView*, FVector2D);

// DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams (FOnTextSelectionChangedDel, UUrBrowserView*, browser, FString, text, int, from, int, to);
DECLARE_MULTICAST_DELEGATE_FourParams         (FOnTextSelectionChangedStaticDel, UUrBrowserView*, FString, int, int);

// DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams  (FOnVirtualKeyboardRequestedDel, UUrBrowserView*, browser, EUrTextInputMode, inputMode);
DECLARE_MULTICAST_DELEGATE_TwoParams          (FOnVirtualKeyboardRequestedStaticDel, UUrBrowserView*, EUrTextInputMode);

/**
 * Implement this interface to handle events when window rendering is disabled.
 * The methods of this class will be called on the UI thread.
 * For Unreal facing interfaces this has been split out to |IUrRenderResourceHandler|
 * and |IUrRenderEventsHandler|.
 */
class URANIUM_API ICefRenderHandler : public CefRenderHandler 
{
	friend IUrRenderResourceHandler;
	friend IUrRenderEventsHandler;
protected:
	UUrRenderEvents* GetRenderEvents();

public:
	TWeakInterfacePtr<IUrRenderResourceHandler> UrRenderResourceHandler;
	TWeakInterfacePtr<IUrRenderEventsHandler> UrRenderHandler;

	virtual bool GetRootScreenRect(CefRefPtr<CefBrowser> browser, CefRect& rect) override;
	virtual bool GetScreenInfo(CefRefPtr<CefBrowser> browser, CefScreenInfo& screenInfo) override;
	virtual void GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) override;

	virtual void OnPaint(
		CefRefPtr<CefBrowser> browser,
		PaintElementType type,
		const RectList& dirtyRects,
		const void* buffer,
		int width, int height
	) override;

	virtual void OnAcceleratedPaint(
		CefRefPtr<CefBrowser> browser,
		PaintElementType type,
		const RectList& dirtyRects,
		void* shared_handle
	) override;

	virtual void OnPopupShow(CefRefPtr<CefBrowser> browser, bool show) override;
	virtual void OnPopupSize(CefRefPtr<CefBrowser> browser, const CefRect& rect) override;
	
	virtual void OnScrollOffsetChanged(CefRefPtr<CefBrowser> browser, double x, double y) override;
	virtual void OnTextSelectionChanged(CefRefPtr<CefBrowser> browser, const CefString& selectedText, const CefRange& selectedRange) override;
	virtual void OnVirtualKeyboardRequested(CefRefPtr<CefBrowser> browser, TextInputMode inputMode) override;
};

UINTERFACE()
class URANIUM_API UUrRenderResourceHandler : public UInterface
{
	GENERATED_BODY()
};

/**
 * Implement this interface to handle events when window rendering is disabled.
 * Handles retrieving browser image and keeping size + scale factor.
 * This handler manages the lifespan of |ICefRenderHandler|
 * This interface can be only overridden by C++.
 */
class URANIUM_API IUrRenderResourceHandler
{
	GENERATED_BODY()
	friend ICefRenderHandler;
public:
	CefRefPtr<ICefRenderHandler> CefRenderHandler = nullptr;

	/**
	 * This should immediately execute when called. Resource availability is not guaranteed
	 * outside of the scope of this function, so an implementer must copy the shared texture
	 * to an Unreal edible resource right in this thread. This is done by default with intermediate
	 * graphics devices which are flushed inside this function. Do not do ENQUEUE_RENDER_COMMAND
	 * in OnAcceleratedPaint and expect that |sharedHandle| will still point to a valid resource.
	 */
	virtual void OnAcceleratedPaint(
		CefRefPtr<CefBrowser> browser,
		void* sharedHandle,
		const CefRenderHandler::RectList& dirtyRects,
		bool isNativePopup
	) {}

	/**
	 * By default nothing should happen here
	 * That might change for platforms not supporting OnAcceleratedPaint
	 */
	virtual void OnPaint(
		CefRefPtr<CefBrowser> browser,
		const void* buffer,
		const CefRenderHandler::RectList& dirtyRects,
		int width, int height,
		bool isNativePopup
	) {}

	virtual UTexture2D* GetMainTexture() const = 0;
	virtual UTexture2D* GetNativePopupTexture() const = 0;
	virtual bool IsTextureVerticallyMirrored() const = 0;
	
	virtual float GetScaleFactor() const = 0;
	virtual void SetScaleFactor(float scale) = 0;
	
	virtual FVector2D GetMainSize() const = 0;
	virtual void SetMainSize(FVector2D size) = 0;

	virtual void TickRenderResource() = 0;
};

/**
 * Implement this interface to handle events when window rendering is disabled.
 * Handles miscellaneous native user interaction~, text selection~ and native-popup events 
 */
UINTERFACE(Blueprintable)
class URANIUM_API UUrRenderEventsHandler : public UInterface
{
	GENERATED_BODY()
};

/**
 * Implement this interface to handle events when window rendering is disabled.
 * The methods of this class will be called on the UI thread.
 * Handles miscellaneous native user interaction~, text selection~ and native-popup events
 * This handler requires an already constructed |ICefRenderHandler| (usually managed by |UUrBrowserView|
 */
class URANIUM_API IUrRenderEventsHandler
{
	GENERATED_BODY()
	friend ICefRenderHandler;
public:
	CefRefPtr<ICefRenderHandler> CefRenderResourceHandler = nullptr;

	/**
	 * Because this interface shares one CEF handler with |IUrRenderResourceHandler|
	 * an already existing CEF handler has to be assigned during the configuration of the
	 * implementing object.
	 */
	void AssignCefRenderHandler(CefRefPtr<ICefRenderHandler> cefHandler);

	/**
	 * Event for the native popup visibility.
	 * In CEF-speak a popup also indicates a native UI element displayed above the HTML content,
	 * for example the dropdown menu of the <select> tag. To alleviate ambiguity of conflicting
	 * meanings they're called NativePopups in Uranium. Only one native popup should be ever
	 * visible above the main HTML content
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Uranium")
	void OnNativePopupShow(UUrBrowserView* browser, bool show);
	
	/**
	 * Event for placing the supposedly shown native popup visibility.
	 * In CEF-speak a popup also indicates a native UI element displayed above the HTML content,
	 * for example the dropdown menu of the <select> tag. To alleviate ambiguity of conflicting
	 * meanings they're called NativePopups in Uranium Only one native popup should be ever
	 * visible above the main HTML content
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Uranium")
	void OnNativePopupSize(UUrBrowserView* browser, FVector2D location, FVector2D size);
	
	UFUNCTION(BlueprintNativeEvent, Category="Uranium")
	void OnScrollOffsetChanged(UUrBrowserView* browser, FVector2D offset);
	
	UFUNCTION(BlueprintNativeEvent, Category="Uranium")
	void OnTextSelectionChanged(UUrBrowserView* browser, const FString& text, int from, int to);
	
	UFUNCTION(BlueprintNativeEvent, Category="Uranium")
	void OnVirtualKeyboardRequested(UUrBrowserView* browser, EUrTextInputMode inputMode);

	UFUNCTION(BlueprintNativeEvent, Category="Uranium")
	bool IsNativePopupVisible();

	UFUNCTION(BlueprintNativeEvent, Category="Uranium")
	FVector2D GetNativePopupLocation();

	UFUNCTION(BlueprintNativeEvent, Category="Uranium")
	FVector2D GetNativePopupSize();

	UFUNCTION(BlueprintNativeEvent, Category="Uranium")
	FVector2D GetScrollOffset();
};

/**
 * BlueprintPure accessors for persistent states
 */
UCLASS(BlueprintType)
class URANIUM_API UUrRenderEventsHandlerStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintPure, Category="Uranium")
	static bool IsNativePopupVisible(TScriptInterface<IUrRenderEventsHandler> handler)
	{
		return IUrRenderEventsHandler::Execute_IsNativePopupVisible(handler.GetObject());
	}

	UFUNCTION(BlueprintPure, Category="Uranium")
	static FVector2D GetNativePopupLocation(TScriptInterface<IUrRenderEventsHandler> handler)
	{
		return IUrRenderEventsHandler::Execute_GetNativePopupLocation(handler.GetObject());
	}

	UFUNCTION(BlueprintPure, Category="Uranium")
	static FVector2D GetNativePopupSize(TScriptInterface<IUrRenderEventsHandler> handler)
	{
		return IUrRenderEventsHandler::Execute_GetNativePopupSize(handler.GetObject());
	}

	UFUNCTION(BlueprintPure, Category="Uranium")
	static FVector2D GetScrollOffset(TScriptInterface<IUrRenderEventsHandler> handler)
	{
		return IUrRenderEventsHandler::Execute_GetScrollOffset(handler.GetObject());
	}
};

UCLASS(BlueprintType)
class URANIUM_API UUrRenderEvents : public UObject
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintPure, Category="Uranium")
	static UUrRenderEvents* GetEvents(TScriptInterface<IUrRenderEventsHandler> handler);

	// UPROPERTY(BlueprintAssignable, Category = "Uranium")
	// FOnNativePopupShowDel OnNativePopupShow;
	FOnNativePopupShowStaticDel OnNativePopupShowStatic;

	// UPROPERTY(BlueprintAssignable, Category = "Uranium")
	// FOnNativePopupSizeDel OnNativePopupSize;
	FOnNativePopupSizeStaticDel OnNativePopupSizeStatic;

	// UPROPERTY(BlueprintAssignable, Category = "Uranium")
	// FOnScrollOffsetChangedDel OnScrollOffsetChanged;
	FOnScrollOffsetChangedStaticDel OnScrollOffsetChangedStatic;

	// UPROPERTY(BlueprintAssignable, Category = "Uranium")
	// FOnTextSelectionChangedDel OnTextSelectionChanged;
	FOnTextSelectionChangedStaticDel OnTextSelectionChangedStatic;

	// UPROPERTY(BlueprintAssignable, Category = "Uranium")
	// FOnVirtualKeyboardRequestedDel OnVirtualKeyboardRequested;
	FOnVirtualKeyboardRequestedStaticDel OnVirtualKeyboardRequestedStatic;
};