// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/UrBrowserWidget.h"



#include "UraniumContext.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Engine/UserInterfaceSettings.h"
#include "Widgets/SUrBrowser.h"


#define LOCTEXT_NAMESPACE "UMG"

TSharedRef<SWidget> UUrBrowserWidget::RebuildWidget()
{
	SAssignNew(SlateBrowser, SUrBrowser)
		.AssociatedBrowser_Lambda([this]() { return AssociatedBrowser; })
		.UseOwnPopupWidget_Lambda([this]() { return !static_cast<bool>(NativePopupWidgetClass); })
		.DoDeferredInit(true)
		.DoTick_Lambda([this]() { return DoTick; })
		.AutoRemovePopup_Lambda([this]() { return ShouldAutoRemoveNativePopupWidget(); })
		.BrowserSizeMode_Lambda([this]() { return BrowserSizeMode; })
		.ManualSize_Lambda([this]() { return ManualSize; })
		.InitMetadata_Lambda([this]() { return InitMetadata; })
		.ColorAndOpacity_Lambda([this]() { return ColorAndOpacity; })
		.ScaleFactor_Lambda([this]() { return GetViewportScale(); })
		.PopupWidget_Lambda([this]() { return SlateGetPopupWidget(); })
		.UseThumbMouseButtons_Lambda([this]() { return UseThumbMouseButtons; });

	return SlateBrowser.ToSharedRef();
}

UUrBrowserWidget::UUrBrowserWidget(const FObjectInitializer& Oi)
	: Super(Oi)
	, ColorAndOpacity(FLinearColor::White)
{
}

void UUrBrowserWidget::InitializeWithEvents(
	FOnBeforeCreatedDel OnBeforeCreatedIn,
	FOnAfterCreatedFuncDel OnAfterCreatedIn,
	FNewBrowserMetadata InInitMetadata,
	UUrBrowserView* InAssociatedBrowser,
	bool InDoTick
) {
	DoTick = InDoTick;
	if(InInitMetadata.TargetUrl.IsEmpty())
		InitMetadata.TargetUrl = InitialUrl;
	else
		InitMetadata = InInitMetadata;
	if(InAssociatedBrowser)
		AssociatedBrowser = InAssociatedBrowser;
	else
	{
		AssociatedBrowser = UUrBrowserView::CreateNewUraniumBrowser(this, OnBeforeCreatedIn, OnAfterCreatedIn);
	}
	ensureMsgf(AssociatedBrowser, TEXT("At this point there should be a valid Associated Browser, but there wasn't"));

	AssociatedBrowser->OnNativePopupShowStatic.AddUObject(this, &UUrBrowserWidget::HandleNativePopupShow);
}

void UUrBrowserWidget::Initialize(FNewBrowserMetadata InInitMetadata, UUrBrowserView* InAssociatedBrowser, bool InDoTick)
{
	DoTick = InDoTick;
	if (InInitMetadata.TargetUrl.IsEmpty())
		InitMetadata.TargetUrl = InitialUrl;
	else
		InitMetadata = InInitMetadata;
	if (InAssociatedBrowser)
		AssociatedBrowser = InAssociatedBrowser;
	else
	{
		AssociatedBrowser = UUrBrowserView::CreateNew(this);
	}
	ensureMsgf(AssociatedBrowser, TEXT("At this point there should be a valid Associated Browser, but there wasn't"));

	AssociatedBrowser->OnNativePopupShowStatic.AddUObject(this, &UUrBrowserWidget::HandleNativePopupShow);
}

void UUrBrowserWidget::SetOpacity(float InOpacity)
{
	auto OutColBinding = PROPERTY_BINDING(FSlateColor, ColorAndOpacity);
	auto OutCol = OutColBinding.Get().GetSpecifiedColor();
	OutCol.A = InOpacity;
	ColorAndOpacity = OutCol;
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
				SlateBrowser->RemovePopupWidget();
		}
		NativePopupWidgetCache->Destruct();
		NativePopupWidgetCache = nullptr;
	}
}

void UUrBrowserWidget::StartPreview()
{
	if(!IsDesignTime()) return;
	FOnCefInitializedDel OnCefInit;
	OnCefInit.BindDynamic(this, &UUrBrowserWidget::HandlePreviewCefInit);
	UUraniumContext::InitializeUranium(OnCefInit, this);
}

void UUrBrowserWidget::StopPreview()
{
	if(AssociatedBrowser) AssociatedBrowser->Close();
}

void UUrBrowserWidget::OpenDevTools()
{
	// TODO: Inspect
	if (AssociatedBrowser) AssociatedBrowser->ShowDevToolsWindow({0,0});
}

void UUrBrowserWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();
}

void UUrBrowserWidget::ReleaseSlateResources(bool bReleaseChildren)
{
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
	if(!GEngine) return 1.0f;
	if(!GEngine->GameViewport) return 1.0;
	FVector2D VpSize;
	GEngine->GameViewport->GetViewportSize(VpSize);
	float Ret = GetDefault<UUserInterfaceSettings>()->GetDPIScaleBasedOnSize(FIntPoint(VpSize.X, VpSize.Y));
	return Ret;
}

TSharedPtr<SWidget> UUrBrowserWidget::SlateGetPopupWidget()
{
	if(!NativePopupWidgetClass) return nullptr;
	NativePopupWidgetCache = CreateWidget(this, NativePopupWidgetClass);

	if(NativePopupWidgetCache->Implements<UUrNativePopupWidget>())
	{
		IUrNativePopupWidget::Execute_OnBeforeAttached(NativePopupWidgetCache, this);
	}
	OnBeforeNativePopupWidgetAttached.Broadcast(this, NativePopupWidgetCache);
	return NativePopupWidgetCache->TakeWidget();
}

bool UUrBrowserWidget::ShouldAutoRemoveNativePopupWidget() const
{
	if (NativePopupWidgetClass)
	{
		return !NativePopupWidgetClass->ImplementsInterface(UUrNativePopupWidget::StaticClass());
	}
	return true;
}

void UUrBrowserWidget::HandleNativePopupShow(bool bShow)
{
	if (!NativePopupWidgetClass) return;
	if(bShow)
	{
		if(NativePopupWidgetCache && NativePopupWidgetCache->Implements<UUrNativePopupWidget>())
		{
			RemoveNativePopupWidget();
		}
	}
	else
	{
		if(ShouldAutoRemoveNativePopupWidget())
			RemoveNativePopupWidget();
		else if(NativePopupWidgetCache)
		{
			IUrNativePopupWidget::Execute_OnRemoveRequested(NativePopupWidgetCache, this);
		}
	}
}

void UUrBrowserWidget::HandlePreviewCefInit()
{
	InitMetadata = {};
	InitMetadata.TargetUrl = PreviewWithUrl.IsEmpty() ? InitialUrl : PreviewWithUrl;
	AssociatedBrowser = UUrBrowserView::CreateNew(this);

	ensureMsgf(AssociatedBrowser, TEXT("At this point there should be a valid Associated Browser, but there wasn't"));

	AssociatedBrowser->OnNativePopupShowStatic.AddUObject(this, &UUrBrowserWidget::HandleNativePopupShow);
	DoTick = true;
}

#undef LOCTEXT_NAMESPACE