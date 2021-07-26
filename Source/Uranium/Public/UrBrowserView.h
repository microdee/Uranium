// MESO Digital Interiors GmbH. (C) 2020-
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Engine/Texture2D.h"
#include "UrCefEnums.h"

#include "CefIncludesStart.h"

#include "include/cef_app.h"
#include "include/cef_browser.h"

#include "CefIncludesEnd.h"

#include "UrBrowserView.generated.h"

class FCefBrowserView;
class UCefBrowserManagedRef;
class ISharedTexture;
class FEvent;
class UUrBrowserView;

/**
 * Delegate to be executed just before the browser is ready, making place for some configuration.
 */
DECLARE_DYNAMIC_DELEGATE_OneParam             (FOnBeforeCreatedDel, UUrBrowserView*, browser);

/**
 * Delegate to be executed after the browser is ready. This is executed before any content is loaded by the browser
 */
DECLARE_DYNAMIC_DELEGATE_OneParam             (FOnAfterCreatedFuncDel, UUrBrowserView*, browser);

/**
 * Delegate to be executed after the browser is ready. This is executed before any content is loaded by the browser
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam   (FOnAfterCreatedDel, UUrBrowserView*, browser);
DECLARE_MULTICAST_DELEGATE_OneParam           (FOnAfterCreatedStaticDel, UUrBrowserView*);

/**
 * Delegate to be executed before the browser closes, allowing for also destroying associated widgets
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam   (FOnBeforeCloseDel, UUrBrowserView*, browser);
DECLARE_MULTICAST_DELEGATE_OneParam           (FOnBeforeCloseStaticDel, UUrBrowserView*);

/**
 * Delegate for the native popup visibility.
 * In CEF-speak a popup also indicates a native UI element displayed above the HTML content,
 * for example the dropdown menu of the <select> tag. To alleviate ambiguity of conflicting
 * meanings they're called NativePopups in Uranium. Only one native popup should be ever
 * visible above the main HTML content
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam   (FOnNativePopupShowDel, bool, show);
DECLARE_MULTICAST_DELEGATE_OneParam           (FOnNativePopupShowStaticDel, bool);

/**
 * Delegate for placing the supposedly shown native popup visibility.
 * In CEF-speak a popup also indicates a native UI element displayed above the HTML content,
 * for example the dropdown menu of the <select> tag. To alleviate ambiguity of conflicting
 * meanings they're called NativePopups in Uranium Only one native popup should be ever
 * visible above the main HTML content
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams  (FOnNativePopupSizeDel, FVector2D, location, FVector2D, size);
DECLARE_MULTICAST_DELEGATE_TwoParams          (FOnNativePopupSizeStaticDel, FVector2D, FVector2D);

/**
 * Called when |NewWindowBehavior| is |EUrNewWindowBehavior::OpenInNewBrowser| a new uninitialized
 * UrBrowserView is created on "open in new window" type of actions, with extra metadata
 * about the desired properties of the newly opened browser instance
 * (like the size for a new popup, or the fact that it is a "popup-window").
 * It is the implementer's responsibility to listen to this event, create the appropriate widgets
 * either with or ignoring the metadata, then call |DeferredInit| on the new UrBrowser.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams  (FOnNewWindowOpenedDel, UUrBrowserView*, browser, FNewBrowserMetadata, newBrowserMetadata);
DECLARE_MULTICAST_DELEGATE_TwoParams          (FOnNewWindowOpenedStaticDel, UUrBrowserView*, FNewBrowserMetadata);

/**
 * Called when an embedded Devtool is opened. Embedded in this context means that the Devtool
 * doesn't open in a separate window, but rather as a new UrBrowser, which the implementer has
 * full freedom over how to render its output inside UE4. The result of this event is already
 * initialized with the size provided when calling |OpenDevToolsEmbedded|,
 * there's no need to call |DeferredInit|.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam   (FOnEmbeddedDevToolOpenedDel, UUrBrowserView*, browser);
DECLARE_MULTICAST_DELEGATE_OneParam           (FOnEmbeddedDevToolOpenedStaticDel, UUrBrowserView*);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam   (FOnVirtualKeyboardRequestedDel, EUrTextInputMode, inputMode);
DECLARE_MULTICAST_DELEGATE_OneParam           (FOnVirtualKeyboardRequestedStaticDel, EUrTextInputMode);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam   (FOnLoadEndDel, int, httpStatusCode);
DECLARE_MULTICAST_DELEGATE_OneParam           (FOnLoadEndStaticDel, int);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams  (FOnLoadErrorDel, FUrCefErrorCode, errorCode, FString, failedUrl);
DECLARE_MULTICAST_DELEGATE_TwoParams          (FOnLoadErrorStaticDel, FUrCefErrorCode, FString);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam   (FOnAddressChangeDel, FString, url);
DECLARE_MULTICAST_DELEGATE_OneParam           (FOnAddressChangeStaticDel, FString);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam   (FOnAutoResizeDel, FVector2D, newSize);
DECLARE_MULTICAST_DELEGATE_OneParam           (FOnAutoResizeStaticDel, FVector2D);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams (FOnConsoleMessageDel, EUrLogSeverity, level, FString, message, FString, source, int, line);
DECLARE_MULTICAST_DELEGATE_FourParams         (FOnConsoleMessageStaticDel, EUrLogSeverity, FString, FString, int);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam   (FOnFullscreenModeChangeDel, bool, fullscreen);
DECLARE_MULTICAST_DELEGATE_OneParam           (FOnFullscreenModeChangeStaticDel, bool);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam   (FOnLoadingProgressChangeDel, float, progress);
DECLARE_MULTICAST_DELEGATE_OneParam           (FOnLoadingProgressChangeStaticDel, float);

UENUM(BlueprintType)
enum class EUrNewWindowBehavior : uint8
{
	OpenInNewBrowser,
	OpenInSameBrowser,
	Suppress,
};

UENUM(BlueprintType)
enum class EUrPointerType : uint8
{
	Touch = 0,
	Mouse,
	Pen,
	Eraser,
	Unknown
};

UENUM(BlueprintType)
enum class EUrPointerEventType : uint8
{
	Released = 0,
	Pressed,
	Moved,
	Cancelled
};

namespace UraniumInput
{
	inline CefBrowserHost::MouseButtonType GetUraniumMouseButton(const FKey& button)
	{
		if (button == EKeys::LeftMouseButton) return MBT_LEFT;
		if (button == EKeys::RightMouseButton) return MBT_RIGHT;
		if (button == EKeys::MiddleMouseButton) return MBT_MIDDLE;
		return MBT_LEFT;
	}

	inline bool IsValidMouseButton(const FKey& button)
	{
		return button == EKeys::LeftMouseButton
			|| button == EKeys::RightMouseButton
			|| button == EKeys::MiddleMouseButton;
	}

	inline cef_event_flags_t GetCefModifierFlagForKey(const FKey& button)
	{
		if (button == EKeys::LeftShift || button == EKeys::RightShift)
			return EVENTFLAG_SHIFT_DOWN;
		if (button == EKeys::LeftControl || button == EKeys::RightControl)
			return EVENTFLAG_CONTROL_DOWN;
		if (button == EKeys::LeftAlt) return EVENTFLAG_ALT_DOWN;
		if (button == EKeys::LeftMouseButton) return EVENTFLAG_LEFT_MOUSE_BUTTON;
		if (button == EKeys::MiddleMouseButton) return EVENTFLAG_MIDDLE_MOUSE_BUTTON;
		if (button == EKeys::RightMouseButton) return EVENTFLAG_RIGHT_MOUSE_BUTTON;
		if (button == EKeys::LeftCommand || button == EKeys::RightCommand)
			return EVENTFLAG_COMMAND_DOWN;
		if (button == EKeys::RightAlt) return EVENTFLAG_ALTGR_DOWN;
		return EVENTFLAG_NONE;
	}
}

USTRUCT(BlueprintType)
struct FHtmlContent
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Uranium")
	FString HtmlContent;

	UPROPERTY(BlueprintReadWrite, Category = "Uranium")
	FString AssociatedUrl;
};

USTRUCT(BlueprintType)
struct FUraniumPointerEvent
{
	GENERATED_BODY()

	FUraniumPointerEvent(): Id(), IsUVSpace(false), RotationDegrees(0), Pressure(0)
	{
	}

	FUraniumPointerEvent(
		const FGeometry& geometry,
		const FPointerEvent& inEvent,
		EUrPointerEventType eventType,
		EUrPointerType pointerType = EUrPointerType::Touch
	)   : Id(static_cast<ETouchIndex::Type>(inEvent.GetPointerIndex()))
		, Location(geometry.AbsoluteToLocal(inEvent.GetScreenSpacePosition()) / geometry.GetLocalSize())
		, Size(FVector2D::ZeroVector)
		, IsUVSpace(true)
		, RotationDegrees(0.0f)
		, Pressure(inEvent.GetTouchForce())
		, EventType(eventType)
		, PointerType(pointerType)
	{
	}

	FUraniumPointerEvent(
		uint32 id,
		const FVector2D& location,
		const FVector2D& size,
		bool isUVSpace,
		float rotationDegrees,
		float pressure,
		EUrPointerEventType eventType,
		EUrPointerType pointerType
	)   : Id(static_cast<ETouchIndex::Type>(id))
		, Location(location)
		, Size(size)
		, IsUVSpace(isUVSpace)
		, RotationDegrees(rotationDegrees)
		, Pressure(pressure)
		, EventType(eventType)
		, PointerType(pointerType)
	{
	}

	UPROPERTY(BlueprintReadWrite, Category = "Uranium")
	TEnumAsByte<ETouchIndex::Type> Id;

	UPROPERTY(BlueprintReadWrite, Category = "Uranium")
	FVector2D Location;

	UPROPERTY(BlueprintReadWrite, Category = "Uranium")
	FVector2D Size = FVector2D::ZeroVector;

	UPROPERTY(BlueprintReadWrite, Category = "Uranium")
	bool IsUVSpace;

	UPROPERTY(BlueprintReadWrite, Category = "Uranium")
	float RotationDegrees;

	UPROPERTY(BlueprintReadWrite, Category = "Uranium")
	float Pressure;

	UPROPERTY(BlueprintReadWrite, Category = "Uranium")
	EUrPointerEventType EventType = EUrPointerEventType::Pressed;

	UPROPERTY(BlueprintReadWrite, Category = "Uranium")
	EUrPointerType PointerType = EUrPointerType::Touch;
};

UCLASS(BlueprintType)
class URANIUM_API UUrBrowserView : public UObject
{
	GENERATED_BODY()

	friend FCefBrowserView;
private:

	bool bStartSync = false;
	bool bClosing = false;
	bool bBrowserValid = false;

	bool bPopupShown = false;
	bool bLoading = false;

	void OnBeforeCloseInternal();

	template<typename TCefEvent>
	static void GetModifierKeys(const FInputEvent& ueKey, TCefEvent& cefKey, const FCefKeyEventFlags& extraFlags)
	{
		// as of cef_event_flags_t
		cefKey.modifiers = ueKey.AreCapsLocked() << 0
			| ueKey.IsShiftDown() << 1
			| ueKey.IsControlDown() << 2
			| ueKey.IsLeftAltDown() << 3
			| extraFlags.LeftMouseButton << 4
			| extraFlags.MiddleMouseButton << 5
			| extraFlags.RightMouseButton << 6
			| ueKey.IsCommandDown() << 7
			| extraFlags.NumLocked << 8
			| extraFlags.IsKeypad << 9
			| extraFlags.Left << 10
			| extraFlags.Right << 11
			| ueKey.IsRightAltDown() << 12
			| extraFlags.OrFlag;
	}

	static FCefKeyEventFlags EnforceFlagsFromEvent(const FInputEvent& ueKey, FCefKeyEventFlags extraFlags)
	{
		extraFlags.CapsLocked = ueKey.AreCapsLocked();
		extraFlags.ShiftDown = ueKey.IsShiftDown();
		extraFlags.ControlDown = ueKey.IsControlDown();
		extraFlags.AltDown = ueKey.IsLeftAltDown();
		extraFlags.CommandDown = ueKey.IsCommandDown();
		extraFlags.AltGrDown = ueKey.IsRightAltDown();
		return extraFlags;
	}


	CefMouseEvent GetCefMouseEvent(FVector2D location, bool uvSpace, const FCefKeyEventFlags& modifiers) const;

	UPROPERTY()
	TScriptInterface<ISharedTexture> MainTexture;

	UPROPERTY()
	TScriptInterface<ISharedTexture> NativePopupTexture;

	FVector2D MainSize;
	FVector2D MainPosition;
	FVector2D NativePopupSize;
	FVector2D NativePopupPosition;

public:

	UUrBrowserView(const FObjectInitializer& oi);

	/**
	 * Browsers created as "new windows" from this browser.
	 */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Uranium")
	TSet<UUrBrowserView*> Children;

	/**
	 * If this browser were created as a "new window" from another browser
	 * this is a reference to that browser, otherwise null.
	 */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Uranium")
	TSoftObjectPtr<UUrBrowserView> Parent;

	/**
	 * The preferred behavior when traditionally a new window would be opened in a web-browser.
	 * By default it is set to |EUrNewWindowBehavior::OpenInSameBrowser| which just navigates
	 * the current browser to the target URL, without creating a new browser instance.
	 */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Uranium")
	EUrNewWindowBehavior NewWindowBehavior = EUrNewWindowBehavior::OpenInSameBrowser;

	/**
	 * The DPI scale factor for chromium. So far it's just a simple read-write property.
	 */
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Uranium")
	float ScaleFactor = 1.0;

	UPROPERTY(Export, Instanced)
	UCefBrowserManagedRef* Instance;

	void RemoveChild(UUrBrowserView* child);
	void AddChild(UUrBrowserView* child);

	/**
	 * Creates a new wrapper object but doesn't yet create an instance of the wrapping CEF browser view
	 * It's the responsibility of the caller to later call |DeferredInit| after some configuration of this wrapper.
	 * This is decided by design in order to allow overseeing the lifespan of the resulting
	 * managed browser view object, by another managed UObject, while non-managed classes has
	 * a chance to configure it before committing to initialization. This allows to have a
	 * completely detached Slate widget using the managed browser wrapper.
	 */
	static UUrBrowserView* CreateNew(UObject* worldContext);

	/**
	 * Creates a new wrapper object but doesn't yet create an instance of the wrapping CEF browser view
	 * It's the responsibility of the caller to later call |DeferredInit| after some configuration of this wrapper.
	 * This is decided by design in order to allow overseeing the lifespan of the resulting
	 * managed browser view object, by another managed UObject, while non-managed classes has
	 * a chance to configure it before committing to initialization. This allows to have a
	 * completely detached Slate widget using the managed browser wrapper.
	 *
	 * @param worldContext      The object owning this browser.
	 *
	 * @param onBeforeCreated   Feed a delegate to be executed just before the browser is ready,
	 *                          making place for some configuration.
	 *
	 * @param onAfterCreated    Feed a delegate to be executed after the browser is ready.
	 *                          This is executed before any content is loaded by the browser
	 *
	 */
	UFUNCTION(BlueprintCallable, Category = "Uranium")
	static UUrBrowserView* CreateNewUraniumBrowser(
		UObject* worldContext,
		FOnBeforeCreatedDel onBeforeCreated,
		FOnAfterCreatedFuncDel onAfterCreated
	);

	/**
	 * This should be called after all preliminary configuration is set (like
	 * the size of the rendering widget is already known). Without calling this function
	 * no CEF browsers are actually created.
	 */
	UFUNCTION(BlueprintCallable, Category = "Uranium")
	void DeferredInit(FVector2D initialSize, FString url);
	void DeferredInit(UCefBrowserManagedRef* fromInstance);

	/**
	 * Explicitly close this browser and release resources. Automatically called on BeginDestroy too.
	 */
	UFUNCTION(BlueprintCallable, Category="Uranium")
	void Close();

	virtual void BeginDestroy() override;

	/**
	 * Event to be executed after the browser is ready. This is executed before any content is loaded by the browser
	 */
	UPROPERTY(BlueprintAssignable, Category = "Uranium")
	FOnAfterCreatedDel OnAfterCreated;
	FOnAfterCreatedStaticDel OnAfterCreatedStatic;

	/**
	 * Event to be executed before the browser closes, allowing for also destroying associated widgets
	 */
	UPROPERTY(BlueprintAssignable, Category = "Uranium")
	FOnBeforeCloseDel OnBeforeClose;
	FOnBeforeCloseStaticDel OnBeforeCloseStatic;
	
	/**
	 * Event for native popups visibility.
	 * In CEF-speak a popup also indicates a native UI element displayed above the HTML content,
	 * for example the dropdown menu of the <select> tag. To alleviate ambiguity of conflicting
	 * meanings they're called NativePopups in Uranium Only one native popup should be ever
	 * visible above the main HTML content
	 */
	UPROPERTY(BlueprintAssignable, Category = "Uranium")
	FOnNativePopupShowDel OnNativePopupShow;
	FOnNativePopupShowStaticDel OnNativePopupShowStatic;

	/**
	 * Event for placing the supposedly shown native popup visibility.
	 * In CEF-speak a popup also indicates a native UI element displayed above the HTML content,
	 * for example the dropdown menu of the <select> tag. To alleviate ambiguity of conflicting
	 * meanings they're called NativePopups in Uranium Only one native popup should be ever
	 * visible above the main HTML content
	 */
	UPROPERTY(BlueprintAssignable, Category = "Uranium")
	FOnNativePopupSizeDel OnNativePopupSize;
	FOnNativePopupSizeStaticDel OnNativePopupSizeStatic;

	/**
	 * Called when |NewWindowBehavior| is |EUrNewWindowBehavior::OpenInNewBrowser| a new uninitialized
	 * UrBrowserView is created on "open in new window" type of actions, with extra metadata
	 * about the desired properties of the newly opened browser instance
	 * (like the size for a new popup, or the fact that it is a "popup-window").
	 * It is the implementer's responsibility to listen to this event, create the appropriate widgets
	 * either with or ignoring the metadata, then call |DeferredInit| on the new UrBrowser.
	 */
	UPROPERTY(BlueprintAssignable, Category = "Uranium")
	FOnNewWindowOpenedDel OnNewWindowOpened;
	FOnNewWindowOpenedStaticDel OnNewWindowOpenedStatic;

	/**
	 * Called when an embedded Devtool is opened. Embedded in this context means that the Devtool
	 * doesn't open in a separate window, but rather as a new UrBrowserView, which the
	 * implementer has full freedom over how to render its output inside UE4.
	 * The result of this event is already initialized with the size provided when calling
	 * |OpenDevToolsEmbedded|, there's no need to call |DeferredInit|.
	 */
	UPROPERTY(BlueprintAssignable, Category = "Uranium")
	FOnEmbeddedDevToolOpenedDel OnEmbeddedDevToolOpened;
	FOnEmbeddedDevToolOpenedStaticDel OnEmbeddedDevToolOpenedStatic;
	
	UPROPERTY(BlueprintAssignable, Category = "Uranium")
	FOnVirtualKeyboardRequestedDel OnVirtualKeyboardRequested;
	FOnVirtualKeyboardRequestedStaticDel OnVirtualKeyboardRequestedStatic;

	UPROPERTY(BlueprintAssignable, Category = "Uranium")
	FOnLoadEndDel OnLoadEnd;
	FOnLoadEndStaticDel OnLoadEndStatic;
	
	UPROPERTY(BlueprintAssignable, Category = "Uranium")
	FOnLoadErrorDel OnLoadError;
	FOnLoadErrorStaticDel OnLoadErrorStatic;
	
	UPROPERTY(BlueprintAssignable, Category = "Uranium")
	FOnAddressChangeDel OnAddressChange;
	FOnAddressChangeStaticDel OnAddressChangeStatic;
	
	UPROPERTY(BlueprintAssignable, Category = "Uranium")
	FOnAutoResizeDel OnAutoResize;
	FOnAutoResizeStaticDel OnAutoResizeStatic;
	
	UPROPERTY(BlueprintAssignable, Category = "Uranium")
	FOnConsoleMessageDel OnConsoleMessage;
	FOnConsoleMessageStaticDel OnConsoleMessageStatic;

	UPROPERTY(BlueprintAssignable, Category = "Uranium")
	FOnFullscreenModeChangeDel OnFullscreenModeChange;
	FOnFullscreenModeChangeStaticDel OnFullscreenModeChangeStatic;
	
	UPROPERTY(BlueprintAssignable, Category = "Uranium")
	FOnLoadingProgressChangeDel OnLoadingProgressChange;
	FOnLoadingProgressChangeStaticDel OnLoadingProgressChangeStatic;
	
	
	UFUNCTION(BlueprintPure, Category = "Uranium")
	void GetMainSize(FVector2D& desiredMainSize, FVector2D& actualMainSize) const;
	
	UFUNCTION(BlueprintPure, Category = "Uranium")
	FVector2D GetNativePopupSize() const { return NativePopupSize; }
	
	UFUNCTION(BlueprintPure, Category = "Uranium")
	FVector2D GetNativePopupPosition() const { return NativePopupPosition; }

	UFUNCTION(BlueprintPure, Category = "Uranium")
	bool GetNativePopupShown() const { return bPopupShown; }

	UFUNCTION(BlueprintPure, Category = "Uranium")
	UTexture2D* GetMainTexture() const;

	UFUNCTION(BlueprintPure, Category = "Uranium")
	UTexture2D* GetNativePopupTexture() const;

	UFUNCTION(BlueprintPure, Category = "Uranium")
	bool IsBrowserValid() const { return bBrowserValid; }
		
	UFUNCTION(BlueprintCallable, Category = "Uranium")
	void SetMainSize(FVector2D size, bool force = false);

	/**
	 * Uranium managed browsers has to be ticked by its parent UObject managing its lifespan.
	 * All widgets created by Uranium (especially SUrBrowser Slate widget) or derivatives
	 * already ticking their associated browsers so the implementer doesn't have to call this
	 * tick function explicitly.
	 * @param deltaSeconds
	 * @param recursive     Should this browser tick its opened children recursively?
	 *                      Shouldn't be enabled when individual child browsers are
	 *                      managed by their own widgets.
	 */
	UFUNCTION(BlueprintCallable, Category = "Uranium")
	void Tick(float deltaSeconds, bool recursive = false);
	
	UFUNCTION(BlueprintPure, Category = "Uranium")
	int GetIdentifier() const;

	UFUNCTION(BlueprintPure, Category = "Uranium")
	bool IsSame(UUrBrowserView* other) const;

	/**
	 * Is this browser represents content traditionally placed in a popup window?
	 * Not to be mistaken by the NativePopup which is a wildly different thing.
	 */
	UFUNCTION(BlueprintPure, Category = "Uranium")
	bool IsPopup() const;

	/**
	 * Does the browser contain or will load a valid document?
	 */
	UFUNCTION(BlueprintPure, Category = "Uranium")
	bool HasDocument() const;


	UFUNCTION(BlueprintCallable, Category = "Uranium")
	void LoadUrl(FString url);

	// TODO: see implementation
	UFUNCTION(BlueprintCallable, Category = "Uranium")
	void LoadHtml(const FHtmlContent& htmlContent);

	UFUNCTION(BlueprintCallable, Category = "Uranium")
	void ExecuteJavaScript(FString code, FString scriptUrl, int startLine = 0);

	UFUNCTION(BlueprintPure, Category = "Uranium")
	FString GetURL() const;

	// TODO: document / implement the formula between CEF zoom and percentage
	UFUNCTION(BlueprintCallable, Category = "Uranium")
	void SetZoomLevel(float zoom);

	UFUNCTION(BlueprintCallable, Category = "Uranium")
	void GoBack();

	UFUNCTION(BlueprintCallable, Category = "Uranium")
	void GoForward();

	UFUNCTION(BlueprintCallable, Category = "Uranium")
	void Reload();

	UFUNCTION(BlueprintCallable, Category = "Uranium")
	void ReloadIgnoreCache();

	UFUNCTION(BlueprintPure, Category = "Uranium")
	bool IsLoading() const;

	/**
	 * Show the DevTools of Chromium in a separate window.
	 * @param inspectElementAt  Optional pixel coordinates from top-left for picking an HTML
	 *                          Element for inspection in the DOM viewer.
	 */
	UFUNCTION(BlueprintCallable, Category = "Uranium")
	void ShowDevToolsWindow(FVector2D inspectElementAt);

	/**
	 * Show the DevTools of Chromium in a new UrBrowserView instance. Listen on
	 * |OnEmbeddedDevToolOpened| event to manage its rendering in UE4
	 * @param inspectElementAt  Optional pixel coordinates from top-left for picking an HTML
	 *                          Element for inspection in the DOM viewer.
	 * @param size              The desired size of the containing Uranium browser
	 */
	UFUNCTION(BlueprintCallable, Category = "Uranium")
	void ShowDevToolsEmbedded(FVector2D inspectElementAt, FVector2D size);

	UFUNCTION(BlueprintCallable, Category = "Uranium")
	void CloseDevTools();

	UFUNCTION(BlueprintPure, Category = "Uranium")
	bool HasDevTools() const;

	UFUNCTION(BlueprintCallable, Category = "Uranium")
	void SetAudioMuted(bool mute);

	UFUNCTION(BlueprintPure, Category = "Uranium")
	bool IsAudioMuted() const;
	
	UFUNCTION(BlueprintCallable, Category = "Uranium")
	void SendKeyEvent(FKeyEvent event, bool isUp, FCefKeyEventFlags modifiers);

	UFUNCTION(BlueprintCallable, Category = "Uranium")
	void SendKey(int windowsKeyCode, FString character, bool isUp, FCefKeyEventFlags modifiers);
	
	void SendKeyEventKc(uint32 keyCode, TCHAR character, bool isSysKey, bool isUp, FCefKeyEventFlags modifiers);

	UFUNCTION(BlueprintCallable, Category = "Uranium")
	void SendCharacterKey(FCharacterEvent event, FCefKeyEventFlags modifiers);

	UFUNCTION(BlueprintCallable, Category = "Uranium")
	void SendKeyString(FString input, FCefKeyEventFlags modifiers);

	void SendCharKc(TCHAR character, bool isSysKey, FCefKeyEventFlags modifiers);


	UFUNCTION(BlueprintCallable, Category = "Uranium")
	void SendMouseMove(FVector2D location, bool uvSpace, bool leaving, FCefKeyEventFlags modifiers);

	UFUNCTION(BlueprintCallable, Category = "Uranium")
	void SendMouseButton(FVector2D location, bool uvSpace, const FKey& button, bool isUp, FCefKeyEventFlags modifiers);
	
	UFUNCTION(BlueprintCallable, Category = "Uranium")
	void SendMouseWheel(FVector2D location, bool uvSpace, FVector2D wheelDelta, FCefKeyEventFlags modifiers);


	UFUNCTION(BlueprintCallable, Category = "Uranium")
	void SendTouchEvent(FUraniumPointerEvent event, FCefKeyEventFlags modifiers);
};