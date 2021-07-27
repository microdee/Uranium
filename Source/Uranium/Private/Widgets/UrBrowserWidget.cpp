// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/UrBrowserWidget.h"

#include "Uranium.h"
#include "UraniumContext.h"
#include "UraniumSubsystem.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Engine/UserInterfaceSettings.h"
#include "Widgets/SUrBrowser.h"


#define LOCTEXT_NAMESPACE "UMG"

TSharedRef<SWidget> UUrBrowserWidget::RebuildWidget()
{
	SAssignNew(SlateBrowser, SUrBrowser)
		.AssociatedBrowser_Lambda([this]() { return AssociatedBrowser; })
		.UseOwnPopupWidget_Lambda([this]()
		{
			return !IsNativePopupWidgetClassValid();
		})
		.DoDeferredInit(true)
		.DoTick_Lambda([this]() { return DoTick; })
		.AutoRemovePopup_Lambda([this]()
		{
			return !IsNativePopupWidgetClassValid();
		})
		.BrowserSizeMode_Lambda([this]() { return BrowserSizeMode; })
		.ManualSize_Lambda([this]() { return ManualSize; })
		.InitMetadata_Lambda([this]() { return InitMetadata; })
		.ColorAndOpacity_Lambda([this]() { return ColorAndOpacity; })
		.ScaleFactor_Lambda([this]() { return GetViewportScale(); })
		.PopupWidget_Lambda([this]() { return SlateGetPopupWidget(); })
		.UseThumbMouseButtons_Lambda([this]() { return UseThumbMouseButtons; });

	if(AutoInitialize && !IsDesignTime())
	{
		Initialize({});
	}

	return SlateBrowser.ToSharedRef();
}

UUrBrowserWidget::UUrBrowserWidget(const FObjectInitializer& oi)
	: Super(oi)
	, ColorAndOpacity(FLinearColor::White)
{
}

void UUrBrowserWidget::InitializeWithEvents(
	FOnBeforeCreatedDel onBeforeCreatedIn,
	FOnAfterCreatedFuncDel onAfterCreatedIn,
	FNewBrowserMetadata initMetadataIn,
	UUrBrowserView* associatedBrowserIn,
	bool doTickIn
) {
	DoTick = doTickIn;
	if(initMetadataIn.TargetUrl.IsEmpty())
	{
		InitMetadata.TargetUrl = InitialUrl;
	}
	else
	{
		InitMetadata = initMetadataIn;
	}
	
	if(associatedBrowserIn)
	{
		AssociatedBrowser = associatedBrowserIn;
	}
	else
	{
		AssociatedBrowser = UUrBrowserView::CreateNewUraniumBrowser(this, onBeforeCreatedIn, onAfterCreatedIn);
	}
	ensureMsgf(AssociatedBrowser, TEXT("At this point there should be a valid Associated Browser, but there wasn't"));

	AssociatedBrowser->OnNativePopupShowStatic.AddUObject(this, &UUrBrowserWidget::HandleNativePopupShow);
}

void UUrBrowserWidget::Initialize(FNewBrowserMetadata initMetadataIn, UUrBrowserView* associatedBrowserIn, bool doTickIn)
{
	DoTick = doTickIn;
	if (initMetadataIn.TargetUrl.IsEmpty())
	{
		InitMetadata.TargetUrl = InitialUrl;
	}
	else
	{
		InitMetadata = initMetadataIn;
	}
	
	if (associatedBrowserIn)
	{
		AssociatedBrowser = associatedBrowserIn;
	}
	else
	{
		AssociatedBrowser = UUrBrowserView::CreateNew(this);
	}
	ensureMsgf(AssociatedBrowser, TEXT("At this point there should be a valid Associated Browser, but there wasn't"));

	AssociatedBrowser->OnNativePopupShowStatic.AddUObject(this, &UUrBrowserWidget::HandleNativePopupShow);
}

void UUrBrowserWidget::SetOpacity(float opacity)
{
	auto outColBinding = PROPERTY_BINDING(FSlateColor, ColorAndOpacity);
	FLinearColor outCol = outColBinding.Get().GetSpecifiedColor();
	outCol.A = opacity;
	ColorAndOpacity = outCol;
}

UUrBrowserView* UUrBrowserWidget::GetAssociatedBrowser() const
{
	return AssociatedBrowser;
}

void UUrBrowserWidget::RemoveNativePopupWidget()
{
	if(NativePopupWidgetCache)
	{
		if(NativePopupWidgetCache->Implements<UUrNativePopupWidget>())
		{
			if(SlateBrowser)
			{
				SlateBrowser->RemovePopupWidget();
			}
		}
		NativePopupWidgetCache->Destruct();
		NativePopupWidgetCache = nullptr;
	}
}

void UUrBrowserWidget::StartPreview()
{
	if(!IsDesignTime())
	{
		return;
	}
	
	InitMetadata = {};
	InitMetadata.TargetUrl = PreviewWithUrl.IsEmpty() ? InitialUrl : PreviewWithUrl;
	AssociatedBrowser = UUrBrowserView::CreateNew(this);

	checkf(AssociatedBrowser, TEXT("At this point there should be a valid Associated Browser, but there wasn't"));

	AssociatedBrowser->OnNativePopupShowStatic.AddUObject(this, &UUrBrowserWidget::HandleNativePopupShow);
	DoTick = true;
}

void UUrBrowserWidget::StopPreview()
{
	if(AssociatedBrowser)
	{
		AssociatedBrowser->Close();
	}
}

void UUrBrowserWidget::OpenDevTools()
{
	// TODO: Inspect
	if (AssociatedBrowser)
	{
		AssociatedBrowser->ShowDevToolsWindow({0,0});
	}
}

void UUrBrowserWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();
}

void UUrBrowserWidget::ReleaseSlateResources(bool bReleaseChildren)
{
	if(NativePopupWidgetCache)
	{
		NativePopupWidgetCache->RemoveFromParent();
		NativePopupWidgetCache = nullptr;
	}
	SlateBrowser.Reset();
	
	Super::ReleaseSlateResources(bReleaseChildren);
}

#if WITH_EDITORONLY_DATA
void UUrBrowserWidget::PostLoad()
{
	Super::PostLoad();
}
#endif

#if WITH_EDITOR
const FText UUrBrowserWidget::GetPaletteCategory()
{
	return LOCTEXT("Uranium", "Uranium");
}
#endif

float UUrBrowserWidget::GetViewportScale()
{
	if(!GEngine || !GEngine->GameViewport)
	{
		return 1.0f;
	}
	
	FVector2D vpSize;
	GEngine->GameViewport->GetViewportSize(vpSize);
	float ret = GetDefault<UUserInterfaceSettings>()->GetDPIScaleBasedOnSize(FIntPoint(vpSize.X, vpSize.Y));
	return ret;
}

TSharedPtr<SWidget> UUrBrowserWidget::SlateGetPopupWidget()
{
	if(!NativePopupWidgetClass)
	{
		return nullptr;
	}
	
	NativePopupWidgetCache = CreateWidget(this, NativePopupWidgetClass);

	if(NativePopupWidgetCache->Implements<UUrNativePopupWidget>())
	{
		IUrNativePopupWidget::Execute_OnBeforeAttached(NativePopupWidgetCache, this);
	}
	
	OnBeforeNativePopupWidgetAttached.Broadcast(this, NativePopupWidgetCache);
	return NativePopupWidgetCache->TakeWidget();
}

bool UUrBrowserWidget::IsNativePopupWidgetClassValid() const
{
	if (NativePopupWidgetClass)
	{
		bool popupClassValid = NativePopupWidgetClass->ImplementsInterface(UUrNativePopupWidget::StaticClass());
		if(!popupClassValid)
		{
			UE_LOG(
				LogUranium, Warning,
				TEXT("Custom native-popup widget class is specified (%s), but it doesn't implement UrNativePopupWidget interface."),
				*NativePopupWidgetClass->GetName()
			);
		}
		return popupClassValid;
	}
	return false;
}

void UUrBrowserWidget::HandleNativePopupShow(bool show)
{
	if (!NativePopupWidgetClass)
	{
		return;
	}
	
	if(show)
	{
		if(NativePopupWidgetCache && NativePopupWidgetCache->Implements<UUrNativePopupWidget>())
		{
			RemoveNativePopupWidget();
		}
	}
	else
	{
		if(IsNativePopupWidgetClassValid())
		{
			RemoveNativePopupWidget();
		}
		
		else if(NativePopupWidgetCache)
		{
			IUrNativePopupWidget::Execute_OnRemoveRequested(NativePopupWidgetCache, this);
		}
	}
}

#undef LOCTEXT_NAMESPACE