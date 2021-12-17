/******************** (C) Copyright 2021 realworld one GmbH & Co. KG All Rights Reserved. ********************/
#pragma once

#include "CoreMinimal.h"
#include "UrCefEnums.h"
#include "UrCommonEnums.h"
#include "UrBrowserCompositor.h"
#include "CefPtr.h"
#include "NewBrowserMetadata.h"
#include "UrInput.h"
#include "UObject/WeakInterfacePtr.h"

#include "UrBrowserView.generated.h"

class IUrDevToolsHandler;
class IUrRequestHandler;
class IUrRenderEventsHandler;
class IUrRenderResourceHandler;
class IUrLoadHandler;
class IUrKeyboardHandler;
class IUrJSDialogHandler;
class IUrFocusHandler;
class IUrFindHandler;
class IUrDragHandler;
class IUrDownloadHandler;
class IUrDisplayHandler;
class IUrDialogHandler;
class IUrContextMenuHandler;
class IUrAudioHandler;
class IUrLifeSpanHandler;
class FCefBrowserClient;
class FEvent;
class UUrBrowserView;
class CefBrowser;

/**
* Delegate to be executed just before the browser is ready, making place for some configuration.
*/
DECLARE_DYNAMIC_DELEGATE_OneParam             (FOnBeforeCreatedFunc, UUrBrowserView*, browser);

/**
 * Delegate to be executed after the browser is ready. This is executed before any content is loaded by the browser
 */
DECLARE_DYNAMIC_DELEGATE_OneParam             (FOnAfterCreatedFunc, UUrBrowserView*, browser);

USTRUCT(BlueprintType)
struct FHtmlContent
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Uranium")
	FString HtmlContent;

	UPROPERTY(BlueprintReadWrite, Category = "Uranium")
	FString AssociatedUrl;
};

UCLASS(BlueprintType, Blueprintable)
class URANIUM_API UUrBrowserView : public UObject, public IUrBrowserCompositor
{
	GENERATED_BODY()

	friend struct FUrBrowserViewDetails;
	
private:

	virtual void OnCreatedInternal(UObject* parent = nullptr, UUrBrowserComposition* composition = nullptr);

	std::atomic_bool bIsAlreadyTicked;

	UFUNCTION()
	void HandleEndFrame();
	
protected:

	bool bClosing = false;
	bool bBrowserValid = false;
	TCefPtr<FCefBrowserClient> CefClient;
	TCefPtr<CefBrowser> Browser;

	CefMouseEvent GetCefMouseEvent(FVector2D location, bool uvSpace, const FCefKeyEventFlags& modifiers) const;

	UObject* CreateHandler(UClass* handlerClass, UClass* ofInterface);

	UPROPERTY()
	TMap<UClass* /* UniqueHandlerClass */, UObject*> UniqueHandlers;

	UPROPERTY()
	TMap<UClass* /* IUr__Handler */, UObject*> Handlers;

	UFUNCTION(BlueprintNativeEvent, Category="Uranium")
	void OnCreated();

	UFUNCTION(BlueprintNativeEvent, Category="Uranium")
	void OnHandlerCreated(UObject* handler, UClass* ofInterface);

	UFUNCTION(BlueprintNativeEvent, Category="Uranium")
	void OnAfterDeferredInitialized();

public:

	template<class T>
	class TBadge
	{
		friend T;
	private: TBadge() {}
	};

	UUrBrowserView(const FObjectInitializer& oi);

	/**
	 * Soft reference to the parent object which created this browser.
	 */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Uranium")
	TSoftObjectPtr<UObject> Parent;

	/**
	 * Browsers created as "new windows" from this browser.
	 */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Uranium")
	TSet<UUrBrowserView*> Children;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Uranium")
	UUrBrowserComposition* Composition;
	
	/** Implement this interface to handle audio events. */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Transient, Category="Uranium")
	UObject* AudioHandler;

	/** Callback interface used for continuation of custom context menu display. */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Transient, Category="Uranium")
	UObject* ContextMenuHandler;

	/** Implement this interface to handle dialog events. */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Transient, Category="Uranium")
	UObject* DialogHandler;

	/** Implement this interface to handle events related to browser display state. */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Transient, Category="Uranium")
	UObject* DisplayHandler;

	/** Callback interface used to asynchronously cancel a download. */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Transient, Category="Uranium")
	UObject* DownloadHandler;

	/** Implement this interface to handle events related to dragging. */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Transient, Category="Uranium")
	UObject* DragHandler;

	/** Implement this interface to handle events related to find results. */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Transient, Category="Uranium")
	UObject* FindHandler;

	/** Implement this interface to handle events related to focus. */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Transient, Category="Uranium")
	UObject* FocusHandler;

	/** Callback interface used for asynchronous continuation of JavaScript dialog requests. */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Transient, Category="Uranium")
	UObject* JSDialogHandler;

	/** Implement this interface to handle events related to keyboard input. */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Transient, Category="Uranium")
	UObject* KeyboardHandler;

	/** Implement this interface to handle events related to browser life span. */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Transient, Category="Uranium")
	UObject* LifeSpanHandler;

	/** Implement this interface to handle events related to browser load status. */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Transient, Category="Uranium")
	UObject* LoadHandler;

	/**
	 * Implement this interface to handle events when window rendering is disabled.
	 * Handles retrieving browser image and keeping size + scale factor.
	 * This interface can be only overridden by C++.
	 */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Transient, Category="Uranium")
	UObject* RenderResourceHandler;

	/**
	 * Implement this interface to handle events when window rendering is disabled.
	 * Handles miscellaneous native user interaction~, text selection~ and native-popup events 
	 */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Transient, Category="Uranium")
	UObject* RenderEventsHandler;

	/** Implement this interface to handle events related to browser requests. */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Transient, Category="Uranium")
	UObject* RequestHandler;

	/** Determine how the chromium DevTool should be opened in Unreal Engine. */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Transient, Category="Uranium")
	UObject* DevToolsHandler;
	
	/** Implement this interface to handle audio events. */
	UFUNCTION(BlueprintPure, Category="Uranium")
	TScriptInterface<IUrAudioHandler> GetAudioInterface() const;

	/** Callback interface used for continuation of custom context menu display. */
	UFUNCTION(BlueprintPure, Category="Uranium")
	TScriptInterface<IUrContextMenuHandler> GetContextMenuInterface() const;

	/** Implement this interface to handle dialog events. */
	UFUNCTION(BlueprintPure, Category="Uranium")
	TScriptInterface<IUrDialogHandler> GetDialogInterface() const;

	/** Implement this interface to handle events related to browser display state. */
	UFUNCTION(BlueprintPure, Category="Uranium")
	TScriptInterface<IUrDisplayHandler> GetDisplayInterface() const;

	/** Callback interface used to asynchronously cancel a download. */
	UFUNCTION(BlueprintPure, Category="Uranium")
	TScriptInterface<IUrDownloadHandler> GetDownloadInterface() const;

	/** Implement this interface to handle events related to dragging. */
	UFUNCTION(BlueprintPure, Category="Uranium")
	TScriptInterface<IUrDragHandler> GetDragInterface() const;

	/** Implement this interface to handle events related to find results. */
	UFUNCTION(BlueprintPure, Category="Uranium")
	TScriptInterface<IUrFindHandler> GetFindInterface() const;

	/** Implement this interface to handle events related to focus. */
	UFUNCTION(BlueprintPure, Category="Uranium")
	TScriptInterface<IUrFocusHandler> GetFocusInterface() const;

	/** Callback interface used for asynchronous continuation of JavaScript dialog requests. */
	UFUNCTION(BlueprintPure, Category="Uranium")
	TScriptInterface<IUrJSDialogHandler> GetJSDialogInterface() const;

	/** Implement this interface to handle events related to keyboard input. */
	UFUNCTION(BlueprintPure, Category="Uranium")
	TScriptInterface<IUrKeyboardHandler> GetKeyboardInterface() const;

	/** Implement this interface to handle events related to browser life span. */
	UFUNCTION(BlueprintPure, Category="Uranium")
	TScriptInterface<IUrLifeSpanHandler> GetLifeSpanInterface() const;

	/** Implement this interface to handle events related to browser load status. */
	UFUNCTION(BlueprintPure, Category="Uranium")
	TScriptInterface<IUrLoadHandler> GetLoadInterface() const;

	/**
	 * Implement this interface to handle events when window rendering is disabled.
	 * Handles retrieving browser image and keeping size + scale factor.
	 * This interface can be only overridden by C++.
	 */
	UFUNCTION(BlueprintPure, Category="Uranium")
	TScriptInterface<IUrRenderResourceHandler> GetRenderResourceInterface() const;

	/**
	 * Implement this interface to handle events when window rendering is disabled.
	 * Handles miscellaneous native user interaction~, text selection~ and native-popup events 
	 */
	UFUNCTION(BlueprintPure, Category="Uranium")
	TScriptInterface<IUrRenderEventsHandler> GetRenderEventsInterface() const;

	/** Implement this interface to handle events related to browser requests. */
	UFUNCTION(BlueprintPure, Category="Uranium")
	TScriptInterface<IUrRequestHandler> GetRequestInterface() const;

	/** Determine how the chromium DevTool should be opened in Unreal Engine. */
	UFUNCTION(BlueprintPure, Category="Uranium")
	TScriptInterface<IUrDevToolsHandler> GetDevToolsInterface() const;

	void SetCefBrowser(TBadge<IUrLifeSpanHandler> badge, TCefPtr<CefBrowser> browser);
	void InvalidateBrowser(TBadge<IUrLifeSpanHandler> badge);

	/**
	 * The DPI scale factor for chromium. So far it's just a simple read-write property.
	 */
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Uranium")
	float ScaleFactor = 1.0;

	void RemoveChild(UUrBrowserView* child);
	void AddChild(UUrBrowserView* child);

	/**
	 * Creates a new browser view but doesn't yet create an actual instance of the CEF browser
	 * It's the responsibility of the caller to later call |DeferredInit| after some configuration of this view.
	 * This is decided by design in order to allow overseeing the lifespan of the resulting
	 * managed browser view object, by another managed UObject, while non-managed classes has
	 * a chance to configure it before committing to initialization. This allows to have a
	 * completely detached Slate widget using the managed browser view.
	 *
	 * @param parent            Optional parent UObject (other browser view, widget, world)
	 *                          that's creating this view
	 * 
	 * @param compositionClass  Optional class composition of the new browser
	 * 
	 * @param explicitViewClass Optionally override the browser view class which is normally set by
	 *                          either the compositionClass parameter, or the default
	 *                          browser composition set in |UUraniumSubsystem|
	 * 
	 * @param templateObject    Optional template object for the newly create browser view
	 */
	static UUrBrowserView* CreateNew(
		UObject* parent = nullptr,
		TSubclassOf<UUrBrowserComposition> compositionClass = nullptr,
		TSubclassOf<UUrBrowserView> explicitViewClass = nullptr,
		UUrBrowserView* templateObject = nullptr
	);

	/**
	 * Creates a new browser view but doesn't yet create an actual instance of the CEF browser
	 * It's the responsibility of the caller to later call |DeferredInit| after some configuration of this view.
	 * This is decided by design in order to allow overseeing the lifespan of the resulting
	 * managed browser view object, by another managed UObject, while non-managed classes has
	 * a chance to configure it before committing to initialization. This allows to have a
	 * completely detached Slate widget using the managed browser view.
	 *
	 * @param onBeforeCreated   Feed a delegate to be executed just before the browser is ready,
	 *                          making place for some configuration.
	 *
	 * @param onAfterCreated    Feed a delegate to be executed after the browser is ready.
	 *                          This is executed before any content is loaded by the browser
	 *
	 * @param parentIn          Optional parent UObject (other browser view, widget, world)
	 *                          that's creating this view
	 * 
	 * @param compositionClass  Optional class composition of the new browser
	 * 
	 * @param explicitViewClass Optionally override the browser view class which is normally set by
	 *                          either the compositionClass parameter, or the default
	 *                          browser composition set in |UUraniumSubsystem|
	 * 
	 * @param templateObject    Optional template object for the newly create browser view
	 */
	UFUNCTION(BlueprintCallable, Category = "Uranium", meta = (AdvancedDisplay = 3))
	static UUrBrowserView* CreateNewUraniumBrowser(
		FOnBeforeCreatedFunc onBeforeCreated,
		FOnAfterCreatedFunc onAfterCreated,
		UObject* parentIn = nullptr,
		TSubclassOf<UUrBrowserComposition> compositionClass = nullptr,
		TSubclassOf<UUrBrowserView> explicitViewClass = nullptr,
		UUrBrowserView* templateObject = nullptr
	);

	/**
	 * This should be called after all preliminary configuration is set (like
	 * the size of the rendering widget is already known). Without calling this function
	 * no CEF browsers are actually created.
	 * This will create a regular CEF browser.
	 * @param initialSize Initial desired size for the new browser
	 * @param url         Initial URL to load when the new browser is ready
	 */
	UFUNCTION(BlueprintCallable, Category = "Uranium")
	void DeferredInit(FVector2D initialSize, FString url);
	
	/**
	 * This should be called after all preliminary configuration is set (like
	 * the size of the rendering widget is already known). Without calling this function
	 * no CEF browsers are actually created.
	 * This should be only used in special cases, like displaying the Devtools inside UE rendering.
	 * @param initialSize     Initial desired size for the new browser
	 * @param url             Initial URL to load when the new browser is ready
	 * @param browserCreation The functor which will create the desired CEF browser with the
	 *                        initial size, URL, and the prepared CEF client
	 */
	void DeferredInit(
		FVector2D initialSize, FString url,
		TFunction<void(FVector2D, FString, TCefPtr<FCefBrowserClient>)> browserCreation
	);

	/**
	 * Explicitly close this browser and release resources. Automatically called on BeginDestroy too.
	 */
	UFUNCTION(BlueprintCallable, Category="Uranium")
	void Close(bool force = false);

	virtual UUrBrowserComposition* GetComposition_Implementation() override;
	virtual void BeginDestroy() override;
	virtual bool IsReadyForFinishDestroy() override;
	virtual bool IsDestructionThreadSafe() const override;
	virtual bool IsSupportedForNetworking() const override;
	
	UFUNCTION(BlueprintPure, Category = "Uranium")
	void GetMainSize(FVector2D& desiredMainSize, FVector2D& actualMainSize) const;
	
	UFUNCTION(BlueprintPure, Category = "Uranium")
	FVector2D GetNativePopupSize() const;
	
	UFUNCTION(BlueprintPure, Category = "Uranium")
	FVector2D GetNativePopupLocation() const;

	UFUNCTION(BlueprintPure, Category = "Uranium")
	bool GetNativePopupShown() const;

	UFUNCTION(BlueprintPure, Category = "Uranium")
	UTexture2D* GetMainTexture() const;

	UFUNCTION(BlueprintPure, Category = "Uranium")
	UTexture2D* GetNativePopupTexture() const;

	UFUNCTION(BlueprintPure, Category = "Uranium")
	bool IsBrowserValid() const;

	UFUNCTION(BlueprintPure, Category = "Uranium")
	bool IsTextureVerticallyMirrored() const;
		
	UFUNCTION(BlueprintCallable, Category = "Uranium")
	void SetMainSize(FVector2D size);

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
	 * Show the DevTools of Chromium. Specify a |DevToolsHandler| to determine how it will be shown
	 * By default it is shown in a separate window, outside of Unreal Engine
	 * (this default behavior is specified by |UrWindowedDevToolsHandler|)
	 * @param inspectElementAt  Optional pixel coordinates from top-left for picking an HTML
	 *                          Element for inspection in the DOM viewer.
	 * @param size              The desired size of the containing browser
	 */
	UFUNCTION(BlueprintCallable, Category = "Uranium")
	void ShowDevTools(FVector2D inspectElementAt, FVector2D size);

	/**
	 * Show the DevTools of Chromium in a separate window regardless of |DevToolsHandler|.
	 * This function will not do anything when not run in editor.
	 * @param inspectElementAt  Optional pixel coordinates from top-left for picking an HTML
	 *                          Element for inspection in the DOM viewer.
	 */
	UFUNCTION(BlueprintCallable, Category = "Uranium")
	void ShowDevToolsWindow(FVector2D inspectElementAt);

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
	void SendTouchEvent(FUrPointerEvent event, FCefKeyEventFlags modifiers);
};
