/******************** (C) Copyright 2021 realworld one GmbH & Co. KG All Rights Reserved. ********************/

#pragma once

#include "CoreMinimal.h"

#include "UObject/Object.h"
#include "UrBrowserComposition.generated.h"

class UUrBrowserView;

/**
 * Inherit from this class and set the default values for its properties in its constructor
 * to override handler classes
 */
UCLASS(BlueprintType, Blueprintable)
class URANIUM_API UUrBrowserComposition : public UObject
{
	GENERATED_BODY()
public:

	UUrBrowserComposition(const FObjectInitializer& oi);

	/** The type of new browsers created within current browser composition scope */
	UPROPERTY(EditAnywhere, Category="Uranium")
	TSubclassOf<UUrBrowserView> BrowserView;

	/** Implement this interface to handle audio events. */
	UPROPERTY(EditAnywhere, Category="Uranium", meta = (MustImplement="UrAudioHandler", ExactClass=false))
	UClass* AudioHandler;

	/** Callback interface used for continuation of custom context menu display. */
	UPROPERTY(EditAnywhere, Category="Uranium", meta = (MustImplement="UrContextMenuHandler", ExactClass=false))
	UClass* ContextMenuHandler;

	/** Implement this interface to handle dialog events. */
	UPROPERTY(EditAnywhere, Category="Uranium", meta = (MustImplement="UrDialogHandler", ExactClass=false))
	UClass* DialogHandler;

	/** Implement this interface to handle events related to browser display state. */
	UPROPERTY(EditAnywhere, Category="Uranium", meta = (MustImplement="UrDisplayHandler", ExactClass=false))
	UClass* DisplayHandler;

	/** Callback interface used to asynchronously cancel a download. */
	UPROPERTY(EditAnywhere, Category="Uranium", meta = (MustImplement="UrDownloadHandler", ExactClass=false))
	UClass* DownloadHandler;

	/** Implement this interface to handle events related to dragging. */
	UPROPERTY(EditAnywhere, Category="Uranium", meta = (MustImplement="UrDragHandler", ExactClass=false))
	UClass* DragHandler;

	/** Implement this interface to handle events related to find results. */
	UPROPERTY(EditAnywhere, Category="Uranium", meta = (MustImplement="UrFindHandler", ExactClass=false))
	UClass* FindHandler;

	/** Implement this interface to handle events related to focus. */
	UPROPERTY(EditAnywhere, Category="Uranium", meta = (MustImplement="UrFocusHandler", ExactClass=false))
	UClass* FocusHandler;

	/** Callback interface used for asynchronous continuation of JavaScript dialog requests. */
	UPROPERTY(EditAnywhere, Category="Uranium", meta = (MustImplement="UrJSDialogHandler", ExactClass=false))
	UClass* JSDialogHandler;

	/** Implement this interface to handle events related to keyboard input. */
	UPROPERTY(EditAnywhere, Category="Uranium", meta = (MustImplement="UrKeyboardHandler", ExactClass=false))
	UClass* KeyboardHandler;

	/** Implement this interface to handle events related to browser life span. */
	UPROPERTY(EditAnywhere, Category="Uranium", meta = (MustImplement="UrLifeSpanHandler", ExactClass=false))
	UClass* LifeSpanHandler;

	/** Implement this interface to handle events related to browser load status. */
	UPROPERTY(EditAnywhere, Category="Uranium", meta = (MustImplement="UrLoadHandler", ExactClass=false))
	UClass* LoadHandler;

	/**
	 * Implement this interface to handle events when window rendering is disabled.
	 * Handles retrieving browser image and keeping size + scale factor.
	 * This interface can be only overridden by C++.
	 */
	UPROPERTY(EditAnywhere, Category="Uranium", meta = (MustImplement="UrRenderResourceHandler", ExactClass=false))
	UClass* RenderResourceHandler;

	/**
	 * Implement this interface to handle events when window rendering is disabled.
	 * Handles miscellaneous native user interaction~, text selection~ and native-popup events 
	 */
	UPROPERTY(EditAnywhere, Category="Uranium", meta = (MustImplement="UrRenderEventsHandler", ExactClass=false))
	UClass* RenderEventsHandler;

	/** Implement this interface to handle events related to browser requests. */
	UPROPERTY(EditAnywhere, Category="Uranium", meta = (MustImplement="UrRequestHandler", ExactClass=false))
	UClass* RequestHandler;

	/** Determine how the chromium DevTool should be opened in Unreal Engine. */
	UPROPERTY(EditAnywhere, Category="Uranium", meta = (MustImplement="UrDevToolsHandler", ExactClass=false))
	UClass* DevToolsHandler;
};
