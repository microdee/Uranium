// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Widget.h"
#include "Widgets/SCompoundWidget.h"

class FDeferredCleanupSlateBrush;
class UUrBrowserView;
class SConstraintCanvas;
class UWidget;
class SUrBrowser;

/**
 * 
 */
class URANIUM_API SUrPopup : public SCompoundWidget
{
public:

	DECLARE_DELEGATE_OneParam(FGetPopupWidget, TSharedPtr<SWidget>& /* OutWidget */)

	SLATE_BEGIN_ARGS(SUrPopup)
		{ }

		SLATE_ATTRIBUTE(TWeakPtr<SUrBrowser>, ParentBrowserWidget)

	SLATE_END_ARGS()

	SUrPopup()
	{
		SetCanTick(true);
		bCanSupportFocus = true;
	}

	TWeakPtr<SUrBrowser> ParentBrowserWidget;
	TSharedPtr<SImage> BaseImage;

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

private:
	TSharedPtr<FDeferredCleanupSlateBrush> PersistentBrush;
	void* PrevTextureID = nullptr;
};