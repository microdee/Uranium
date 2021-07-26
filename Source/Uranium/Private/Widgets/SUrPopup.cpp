// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/SUrPopup.h"

#include "SlateOptMacros.h"
#include "UrBrowserView.h"
#include "Slate/DeferredCleanupSlateBrush.h"
#include "Widgets/SUrBrowser.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Layout/SConstraintCanvas.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SUrPopup::Construct(const FArguments& args)
{
	ParentBrowserWidget = args._ParentBrowserWidget.Get();

	ChildSlot
	[
		SAssignNew(BaseImage, SImage)
		. RenderTransformPivot(FVector2D(0.5, 0.5))
		. RenderTransform(FSlateRenderTransform(FScale2D( 1.0f, -1.0f )))
		. Visibility(EVisibility::HitTestInvisible)
	];
}

void SUrPopup::Tick(const FGeometry& allottedGeometry, const double currentTime, const float deltaTime)
{
	if(!ParentBrowserWidget.IsValid())
	{
		return;
	}
	
	auto parent = ParentBrowserWidget.Pin();
	auto browser = parent->PersistentArgs._AssociatedBrowser.Get();
	if(browser.IsValid())
	{
		UTexture2D* popupTex = browser->GetNativePopupTexture();
		if(PrevTextureID != popupTex)
		{
			PersistentBrush.Reset();
			PersistentBrush =
				FDeferredCleanupSlateBrush::CreateBrush(browser->GetNativePopupTexture());
			PrevTextureID = popupTex;
			BaseImage->SetImage(PersistentBrush->GetSlateBrush());
		}
		BaseImage->SetVisibility(
			popupTex ?
			EVisibility::HitTestInvisible :
			EVisibility::Hidden
		);
	}
	else
	{
		BaseImage->SetVisibility(EVisibility::Hidden);
	}
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION