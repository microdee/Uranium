// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Widget.h"
#include "Widgets/SCompoundWidget.h"
#include "UrCefEnums.h"

class FDeferredCleanupSlateBrush;
class UUrBrowserView;
class SConstraintCanvas;
class UWidget;

/**
 * Simple bare-bones slate widget providing a view on an Uranium browser object
 * It can also initialize and tick the browser object (default behavior) when requested.
 */
class URANIUM_API SUrBrowser : public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SUrBrowser)
		: _UseOwnPopupWidget(true)
		, _DoDeferredInit(true)
		, _DoTick(true)
		, _AutoRemovePopup(true)
		, _BrowserSizeMode(EUrGetBrowserSize::FromWidgetLocalSize)
		, _ManualSize(FVector2D(800.0f, 800.0f))
		, _ColorAndOpacity(FSlateColor(FLinearColor::White))
		{ }

		/**
		 * The browser object this widget should operate on.
		 * This can be changed during runtime without the need for recreating this widget.
		 */
		SLATE_ATTRIBUTE(TWeakObjectPtr<UUrBrowserView>, AssociatedBrowser)

		/** Use the default simple widget for native popups */
		SLATE_ATTRIBUTE(bool, UseOwnPopupWidget)

		/** Initialize browser object if it haven't been yet. */
		SLATE_ATTRIBUTE(bool, DoDeferredInit)
	
		/** This widget will do the ticking of the browser object */
		SLATE_ATTRIBUTE(bool, DoTick)
	
		/**
		 * If UseOwnPopupWidget is false and this is true, remove the custom native-popup widget
		 * automatically when it's not displayed anymore. When this is false the native-popup
		 * widget is responsible of its own removal or hiding.
		 * This allows to have a fade-out animation for custom native-popup widgets.
		 */
		SLATE_ATTRIBUTE(bool, AutoRemovePopup)

		/** Use thumb mouse buttons for navigating the history of associated browser */
		SLATE_ATTRIBUTE(bool, UseThumbMouseButtons)

		/** The way to set the viewport resolution for the Browser */
		SLATE_ATTRIBUTE(EUrGetBrowserSize, BrowserSizeMode)

		/** Used when BrowserSizeMode is set "Manually" */
		SLATE_ATTRIBUTE(FVector2D, ManualSize)

		/** Used for auto deferred initialization */
		SLATE_ATTRIBUTE(FNewBrowserMetadata, InitMetadata)

		SLATE_ATTRIBUTE(FSlateColor, ColorAndOpacity)
		SLATE_ATTRIBUTE(float, ScaleFactor)

		/**
		 * If valid and UseOwnPopupWidget is false then the returned widget will be used as native-popup
		 * It's recommended to bind a function to this attribute and create a new widget in there.
		 */
		SLATE_ATTRIBUTE(TSharedPtr<SWidget>, PopupWidget)

	SLATE_END_ARGS()

	SUrBrowser()
	{
		SetCanTick(true);
		bCanSupportFocus = true;
	}

	virtual ~SUrBrowser() override;

	FArguments PersistentArgs;

	TSharedPtr<SImage> BaseImage;
	TSharedPtr<SWidget> PopupWidgetPersistent;
	TSharedPtr<SConstraintCanvas> Layout;

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& args);

	void RemovePopupWidget();

	virtual void Tick(const FGeometry& allottedGeometry, const double currentTime, const float deltaTime) override;

	FCefKeyEventFlags GetModifiers(const FInputEvent& event);
	FCefKeyEventFlags GetPointerModifiers(const FPointerEvent& event);

	virtual void OnMouseLeave(const FPointerEvent& mouseEvent) override;
	virtual FReply OnMouseMove(const FGeometry& geometry, const FPointerEvent& mouseEvent) override;
	virtual FReply OnMouseWheel(const FGeometry& geometry, const FPointerEvent& mouseEvent) override;
	virtual FReply OnMouseButtonDown(const FGeometry& geometry, const FPointerEvent& mouseEvent) override;
	virtual FReply OnMouseButtonUp(const FGeometry& geometry, const FPointerEvent& mouseEvent) override;

	virtual FReply OnTouchStarted(const FGeometry& geometry, const FPointerEvent& touchEvent) override;
	virtual FReply OnTouchMoved(const FGeometry& geometry, const FPointerEvent& touchEvent) override;
	virtual FReply OnTouchForceChanged(const FGeometry& geometry, const FPointerEvent& touchEvent) override;
	virtual FReply OnTouchEnded(const FGeometry& geometry, const FPointerEvent& touchEvent) override;

	virtual bool SupportsKeyboardFocus() const override;

	virtual FReply OnKeyDown(const FGeometry& geometry, const FKeyEvent& keyEvent) override;
	virtual FReply OnKeyChar(const FGeometry& geometry, const FCharacterEvent& characterEvent) override;
	virtual FReply OnKeyUp(const FGeometry& geometry, const FKeyEvent& keyEvent) override;

protected:

	TSharedPtr<FDeferredCleanupSlateBrush> PersistentBrush;
	TSharedPtr<SWidget> CreatePopupWidget();
	static FVector2D GetUvFromScreenSpace(const FGeometry& geometry, FVector2D position);
	FVector2D GetUvFromBrowser(FVector2D position) const;

	static FVector2D GetSizeForBrowserContent(const FGeometry& geometry);

private:
	void HandleOnPopupShow(bool show);
	void HandleOnPopupSize(FVector2D location, FVector2D size);
	
	void CheckBrowserChange(TWeakObjectPtr<UUrBrowserView> assocBrowser);

	FVector2D PrevMouseLoc = FVector2D::ZeroVector;

	FCefKeyEventFlags PersistentModifiers;

	TWeakObjectPtr<UUrBrowserView> PrevBrowser {};
	void* PrevTextureID = nullptr;

	TSet<uint32> ActiveTouches;
};