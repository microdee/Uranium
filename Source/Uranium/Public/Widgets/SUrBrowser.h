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
 * Simple bare-bones slate widget providing a view on a Uranium browser object
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

        SLATE_ATTRIBUTE(TWeakObjectPtr<UUrBrowserView>, AssociatedBrowser)
        SLATE_ATTRIBUTE(bool, UseOwnPopupWidget)
        SLATE_ATTRIBUTE(bool, DoDeferredInit)
        SLATE_ATTRIBUTE(bool, DoTick)
        SLATE_ATTRIBUTE(bool, AutoRemovePopup)

        /** Use thumb mouse buttons for navigating the history of associated browser */
        SLATE_ATTRIBUTE(bool, UseThumbMouseButtons)
        SLATE_ATTRIBUTE(EUrGetBrowserSize, BrowserSizeMode)
        SLATE_ATTRIBUTE(FVector2D, ManualSize)
        SLATE_ATTRIBUTE(FNewBrowserMetadata, InitMetadata)
        SLATE_ATTRIBUTE(FSlateColor, ColorAndOpacity)
        SLATE_ATTRIBUTE(float, ScaleFactor)
        SLATE_ATTRIBUTE(TSharedPtr<SWidget>, PopupWidget)

    SLATE_END_ARGS()

    SUrBrowser()
    {
        SetCanTick(true);
        bCanSupportFocus = true;
    }

    FArguments PersistentArgs;

    TSharedPtr<SImage> BaseImage;
    TSharedPtr<SWidget> PopupWidgetPersistent;
    TSharedPtr<SConstraintCanvas> Layout;

    /** Constructs this widget with InArgs */
    void Construct(const FArguments& InArgs);

    void RemovePopupWidget();
    
    virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

    FCefKeyEventFlags GetModifiers(const FInputEvent& Event);
    FCefKeyEventFlags GetPointerModifiers(const FPointerEvent& Event);

    virtual void OnMouseLeave(const FPointerEvent& MouseEvent) override;
    virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    virtual FReply OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

    virtual FReply OnTouchStarted(const FGeometry& MyGeometry, const FPointerEvent& InTouchEvent) override;
    virtual FReply OnTouchMoved(const FGeometry& MyGeometry, const FPointerEvent& InTouchEvent) override;
    virtual FReply OnTouchForceChanged(const FGeometry& MyGeometry, const FPointerEvent& TouchEvent) override;
    virtual FReply OnTouchEnded(const FGeometry& MyGeometry, const FPointerEvent& InTouchEvent) override;

    virtual bool SupportsKeyboardFocus() const override;

    virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;
    virtual FReply OnKeyChar(const FGeometry& MyGeometry, const FCharacterEvent& InCharacterEvent) override;
    virtual FReply OnKeyUp(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;

protected:

    TSharedPtr<FDeferredCleanupSlateBrush> PersistentBrush;
    TSharedPtr<SWidget> CreatePopupWidget();
    static FVector2D GetUvFromScreenSpace(const FGeometry& Geom, FVector2D InPos);
    FVector2D GetUvFromBrowser(FVector2D InPos) const;

    static FVector2D GetSizeForBrowserContent(const FGeometry& Geom);

private:
    void HandleOnPopupShow(bool Show);
    void HandleOnPopupSize(FVector2D Location, FVector2D Size);

    FVector2D PrevMouseLoc = FVector2D::ZeroVector;

    FCefKeyEventFlags PersistentModifiers;

    void* PrevBrowser = nullptr;
    void* PrevTextureID = nullptr;

    TSet<uint32> ActiveTouches;
};
