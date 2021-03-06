/******************** (C) Copyright 2021 realworld one GmbH & Co. KG All Rights Reserved. ********************/

#pragma once

#include "CoreMinimal.h"

#include "UrCefEnums.h"
#include "UrBrowserView.h"
#include "Blueprint/UserWidget.h"
#include "UrBrowserWidget.generated.h"

class UUrBrowserWidget;
class SUrBrowser;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FWidgetCreationDel, UUrBrowserWidget*, parentWidget, UUserWidget*, newWidget);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnAfterNewWindowWidgetCreatedDel, UUrBrowserWidget*, parentWidget, UUserWidget*, newWidget, UUrBrowserView*, newBrowser, FNewBrowserMetadata, metadata);

UINTERFACE(Blueprintable)
class URANIUM_API UUrNativePopupWidget : public UInterface
{
	GENERATED_BODY()
};

class URANIUM_API IUrNativePopupWidget
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Uranium")
	void OnBeforeAttached(UUrBrowserWidget* parentWidget);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Uranium")
	void OnRemoveRequested(UUrBrowserWidget* parentWidget);
};

UINTERFACE(Blueprintable)
class URANIUM_API UUrBrowserWindowWidget : public UInterface
{
	GENERATED_BODY()
};

class URANIUM_API IUrBrowserWindowWidget
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Uranium")
	void OnNewWindowCreated(UUrBrowserWidget* parentWidget, UUrBrowserView* newBrowser, FNewBrowserMetadata metadata);
};

/**
 * An HTML rendering browser widget using CEF (Chromium)
 */
UCLASS()
class URANIUM_API UUrBrowserWidget
	: public UWidget
	, public IUrBrowserCompositor
{
	GENERATED_BODY()
protected:

	virtual TSharedRef<SWidget> RebuildWidget() override;
	TSharedPtr<SUrBrowser> SlateBrowser;

	UPROPERTY()
	UUrBrowserView* AssociatedBrowser;

	UPROPERTY()
	bool DoTick = true;

	UPROPERTY()
	FNewBrowserMetadata InitMetadata;

	void DeInit();

public:

	UUrBrowserWidget(const FObjectInitializer& oi);

	
	virtual void RemoveFromParent() override;
	virtual void BeginDestroy() override;

	/* When true, |Initialize| or |InitializeWithEvents| are not necessary to be called from Blueprint. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Browser")
	bool AutoInitialize = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Appearance, meta = (sRGB = "true"))
	FSlateColor ColorAndOpacity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Browser")
	FString InitialUrl;

	/**
	 * Decide how to determine the resolution of the HTML content
	 * (default is set from widget size)
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Browser")
	EUrGetBrowserSize BrowserSizeMode = EUrGetBrowserSize::FromWidgetLocalSize;

	/** Used when |BrowserSizeMode| set to manual or "desired or manual" */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Browser")
	FVector2D ManualSize = {800, 800};

	/** Use thumb mouse buttons for navigating the history of associated browser */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Browser")
	bool UseThumbMouseButtons = true;

	/**
	 * Determines the class of widget which should be created on new window events
	 * If null/none the managed this widget will not listen to new window events.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "Browser", meta=(MustImplement="UrBrowserWindowWidget"))
	TSubclassOf<UUserWidget> NewBrowserWidgetClass;

	/**
	 * Determines the class of widget which should be created when the native popup is supposed
	 * to be shown. If null the internal simplistic Slate widget is used to display the native popup.
	 * Specified widget class must implement UrNativePopupWidget interface
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "Browser", meta=(MustImplement="UrNativePopupWidget"))
	TSubclassOf<UUserWidget> NativePopupWidgetClass;

	/** If set override the default browser composition. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "Browser")
	TSubclassOf<UUrBrowserComposition> BrowserComposition;
	
	/** If set override the default browser view class (even the one inside a specified composition). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "Browser")
	TSubclassOf<UUrBrowserView> BrowserViewClass;

	virtual UUrBrowserComposition* GetComposition_Implementation() override;
	
	/**
	 * Initialize this browser widget from a UMG widget no earlier than the Construct event.
	 * Calling this function is required, otherwise no browser will be working in this widget.
	 * For most top-level cases leaving everything default is sufficient, and therefore pins
	 * are hidden behind the expander.
	 *
	 * @param onBeforeCreatedIn         Called before creating a new Uranium browser
	 * @param onAfterCreatedIn          Called after the internal new Uranium browser is created
	 *
	 * @param initMetadataIn            If created from new window events, feed the metadata of the new
	 *                                  browser into this parameter.
	 *
	 * @param associatedBrowserIn       If null/none this widget will be the authority of a Uranium
	 *                                  browser object. Otherwise caller can provide their own browser object
	 *                                  but they're responsible then managing its lifetime.
	 *
	 * @param doTickIn                  When true (default) this widget will tick the associated Uranium browser
	 */
	UFUNCTION(BlueprintCallable, Category = "Uranium", meta = (AdvancedDisplay = 1))
	void InitializeWithEvents(
		FOnBeforeCreatedFunc onBeforeCreatedIn,
		FOnAfterCreatedFunc onAfterCreatedIn,
		FNewBrowserMetadata initMetadataIn,
		UUrBrowserView* associatedBrowserIn = nullptr,
		bool doTickIn = true
	);

	/**
	 * Initialize this browser widget from a UMG widget no earlier than the Construct event.
	 * Calling this function is required, otherwise no browser will be working in this widget.
	 * For most top-level cases leaving everything default is sufficient, and therefore pins
	 * are hidden behind the expander.
	 *
	 * @param initMetadataIn            If created from new window events, feed the metadata of the new
	 *                                  browser into this parameter.
	 *
	 * @param associatedBrowserIn       If null/none this widget will be the authority of a Uranium
	 *                                  browser object. Otherwise caller can provide their own browser object
	 *                                  but they're responsible then managing its lifetime.
	 *
	 * @param doTickIn                  When true (default) this widget will tick the associated Uranium browser
	 */
	UFUNCTION(BlueprintCallable, Category = "Uranium", meta = (AdvancedDisplay = 1))
	void Initialize(
		FNewBrowserMetadata initMetadataIn,
		UUrBrowserView* associatedBrowserIn = nullptr,
		bool doTickIn = true
	);

	UFUNCTION(BlueprintCallable, Category = "Appearance")
	void SetOpacity(float opacity);

	UPROPERTY()
	FGetSlateColor ColorAndOpacityDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Uarnium")
	FWidgetCreationDel OnBeforeNativePopupWidgetAttached;

	UPROPERTY(BlueprintAssignable, Category = "Uarnium")
	FOnAfterNewWindowWidgetCreatedDel OnAfterNewWindowWidgetCreated;

	UFUNCTION(BlueprintPure, Category = "Uranium")
	UUrBrowserView* GetAssociatedBrowser() const;

	/**
	 * By default the life-span of native popups are managed by the native Uranium Slate Browser
	 * widget, so |RemoveFromParent| function of UMG widgets are not working in that case.
	 * Call this function triggered from |UrNativePopupWidget::OnRemoveRequested|
	 * when the given widget inherits from that interface. This gives a chance to remove the
	 * widget only after some fade-out animation.
	 */
	UFUNCTION(BlueprintCallable, Category = "Uranium")
	void RemoveNativePopupWidget();

	UFUNCTION(CallInEditor, Category = "Preview")
	void StartPreview();

	UFUNCTION(CallInEditor, Category = "Preview")
	void StopPreview();

	UFUNCTION(CallInEditor, Category = "Preview")
	void OpenDevTools();

	/** Leave it empty if the same as Initial Url */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Preview")
	FString PreviewWithUrl;

	virtual void SynchronizeProperties() override;
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

#if WITH_EDITORONLY_DATA
	virtual void PostLoad() override;
#endif

#if WITH_EDITOR
	virtual const FText GetPaletteCategory() override;
#endif

protected:
	PROPERTY_BINDING_IMPLEMENTATION(FSlateColor, ColorAndOpacity);

	float GetViewportScale();

private:

	TSharedPtr<SWidget> SlateGetPopupWidget();
	bool IsNativePopupWidgetClassValid() const;
	
	void SubscribeToEvents();
	void RemoveFromEvents();

	UPROPERTY()
	UUserWidget* NativePopupWidgetCache;

	UFUNCTION()
	void HandleNativePopupShow(UUrBrowserView* browser, bool show);

	UFUNCTION()
	void HandleNewWindowOpened(UUrBrowserView* parentBrowser, UUrBrowserView* newBrowser, FNewBrowserMetadata newBrowserMetadata);
};