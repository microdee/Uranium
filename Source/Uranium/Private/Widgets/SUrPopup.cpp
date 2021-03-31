// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/SUrPopup.h"

#include "SlateOptMacros.h"
#include "UrBrowserView.h"
#include "Slate/DeferredCleanupSlateBrush.h"
#include "Widgets/SUrBrowser.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Layout/SConstraintCanvas.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SUrPopup::Construct(const FArguments& InArgs)
{
    ParentBrowserWidget = InArgs._ParentBrowserWidget.Get();

    ChildSlot
    [
        SAssignNew(BaseImage, SImage)
        . RenderTransformPivot(FVector2D(0.5, 0.5))
        . RenderTransform(FSlateRenderTransform(FScale2D( 1.0f, -1.0f )))
        . Visibility(EVisibility::HitTestInvisible)
    ];
}

void SUrPopup::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
    if(!ParentBrowserWidget.IsValid()) return;
    auto Parent = ParentBrowserWidget.Pin();
    auto Browser = Parent->PersistentArgs._AssociatedBrowser.Get();
    if(Browser.IsValid())
    {
        auto PopupTex = Browser->GetNativePopupTexture();
        if(PrevTextureID != PopupTex)
        {
            PersistentBrush.Reset();
            PersistentBrush =
                FDeferredCleanupSlateBrush::CreateBrush(Browser->GetNativePopupTexture());
            PrevTextureID = PopupTex;
            BaseImage->SetImage(PersistentBrush->GetSlateBrush());
        }
        BaseImage->SetVisibility(
            PopupTex ?
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
