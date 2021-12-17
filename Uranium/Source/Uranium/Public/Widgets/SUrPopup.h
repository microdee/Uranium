/******************** (C) Copyright 2021 realworld one GmbH & Co. KG All Rights Reserved. ********************/

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
 * This is the basic widget used to display native-popups.
 * It's intended to be used only by |SUrBrowser| widget.
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
	void Construct(const FArguments& args);

	virtual void Tick(const FGeometry& allottedGeometry, const double currentTime, const float deltaTime) override;

private:
	TSharedPtr<FDeferredCleanupSlateBrush> PersistentBrush;
	void* PrevTextureID = nullptr;
};