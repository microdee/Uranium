// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/SUrBrowser.h"

#include "CefBrowserManagedRef.h"
#include "Widgets/SUrPopup.h"

#include "SlateOptMacros.h"
#include "UrBrowserView.h"
#include "Slate/DeferredCleanupSlateBrush.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Layout/SConstraintCanvas.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

SUrBrowser::~SUrBrowser()
{
	if(PrevBrowser.IsValid())
	{
		PrevBrowser->OnNativePopupShowStatic.RemoveAll(this);
		PrevBrowser->OnNativePopupSizeStatic.RemoveAll(this);
	}
}

void SUrBrowser::Construct(const FArguments& args)
{
	PersistentArgs = args;

	ChildSlot
	[
		SAssignNew(Layout, SConstraintCanvas)
		+ SConstraintCanvas::Slot()
		. Anchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f))
		. Offset(FMargin(0,0))
		. ZOrder(0.0f)
		[
			SAssignNew(BaseImage, SImage)
			. RenderTransformPivot(FVector2D(0.5, 0.5))
			. RenderTransform(FSlateRenderTransform(FScale2D(1.0f, -1.0f)))
		]
	];
}

void SUrBrowser::RemovePopupWidget()
{
	if (PopupWidgetPersistent)
	{
		Layout->RemoveSlot(PopupWidgetPersistent.ToSharedRef());
		PopupWidgetPersistent.Reset();
	}
}

void SUrBrowser::Tick(const FGeometry& allottedGeometry, const double currentTime, const float deltaTime)
{
	auto assocBrowser = PersistentArgs._AssociatedBrowser.Get();
	if(assocBrowser.IsValid() && assocBrowser->Instance)
	{
		CheckBrowserChange(assocBrowser);

		FVector2D localSize = GetSizeForBrowserContent(allottedGeometry);
		FVector2D actualSize;
		switch (PersistentArgs._BrowserSizeMode.Get())
		{
		case EUrGetBrowserSize::Manually:
			actualSize = PersistentArgs._ManualSize.Get();
			break;

		case EUrGetBrowserSize::FromDesiredOrManually:
			actualSize = PersistentArgs._InitMetadata.Get().DesiredSize.Size() < UE_SQRT_2
				? PersistentArgs._ManualSize.Get()
				: PersistentArgs._InitMetadata.Get().DesiredSize;
			break;

		case EUrGetBrowserSize::FromDesiredOrFromWidgetLocalSize:
			actualSize = PersistentArgs._InitMetadata.Get().DesiredSize.Size() < UE_SQRT_2
				? allottedGeometry.GetLocalSize()
				: PersistentArgs._InitMetadata.Get().DesiredSize;
			break;

		case EUrGetBrowserSize::FromDesiredOrFromWidgetAbsoluteSize:
			actualSize = PersistentArgs._InitMetadata.Get().DesiredSize.Size() < UE_SQRT_2
				? allottedGeometry.GetAbsoluteSize()
				: PersistentArgs._InitMetadata.Get().DesiredSize;
			break;

		case EUrGetBrowserSize::FromWidgetLocalSize:
			actualSize = allottedGeometry.GetLocalSize();
			break;

		case EUrGetBrowserSize::FromWidgetAbsoluteSize:
			actualSize = allottedGeometry.GetAbsoluteSize();
			break;

		default: break;
		}

		//AssocBrows->ScaleFactor = FMath::Max(ScaleFactor.Get(), 1.0f);
		//ActualSize *= AssocBrows->ScaleFactor;

		// If requested initialize browser if it's not yet initialized
		if (PersistentArgs._DoDeferredInit.Get() && !assocBrowser->Instance->Obj)
		{
			assocBrowser->DeferredInit(actualSize, PersistentArgs._InitMetadata.Get().TargetUrl);
		}
		else
		{
			assocBrowser->SetMainSize(actualSize);
		}

		if (PersistentArgs._DoTick.Get())
		{
			assocBrowser->Tick(deltaTime);
		}

		if(assocBrowser->GetMainTexture())
		{
			BaseImage->SetVisibility(EVisibility::Visible);
			FVector2D desired, actual;
			assocBrowser->GetMainSize(desired, actual);

			if (PrevTextureID != assocBrowser->GetMainTexture())
			{
				PrevTextureID = assocBrowser->GetMainTexture();
				if (assocBrowser->GetMainTexture())
				{
					PersistentBrush.Reset();
					PersistentBrush =
						FDeferredCleanupSlateBrush::CreateBrush(assocBrowser->GetMainTexture());
					BaseImage->SetImage(PersistentBrush->GetSlateBrush());
				}
			}
		}
		else
		{
			BaseImage->SetVisibility(EVisibility::Hidden);
		}
	}
	else
	{
		BaseImage->SetVisibility(EVisibility::Hidden);
	}
	BaseImage->SetColorAndOpacity(ColorAndOpacity.Get());
}

FCefKeyEventFlags SUrBrowser::GetModifiers(const FInputEvent& event)
{
	PersistentModifiers.CapsLocked = event.AreCapsLocked();
	PersistentModifiers.ShiftDown = event.IsShiftDown();
	PersistentModifiers.ControlDown = event.IsControlDown();
	PersistentModifiers.AltDown = event.IsLeftAltDown();
	PersistentModifiers.CommandDown = event.IsCommandDown();
	PersistentModifiers.AltGrDown = event.IsRightAltDown();
	return PersistentModifiers;
}

FCefKeyEventFlags SUrBrowser::GetPointerModifiers(const FPointerEvent& event)
{
	PersistentModifiers = {};
	for(const FKey& Button : event.GetPressedButtons())
	{
		PersistentModifiers.LeftMouseButton |= Button == EKeys::LeftMouseButton;
		PersistentModifiers.MiddleMouseButton |= Button == EKeys::MiddleMouseButton;
		PersistentModifiers.RightMouseButton |= Button == EKeys::RightMouseButton;
	}
	return GetModifiers(event);
}

void SUrBrowser::OnMouseLeave(const FPointerEvent& mouseEvent)
{
	auto assocBrowser = PersistentArgs._AssociatedBrowser.Get();
	if (!assocBrowser.IsValid())
	{
		return;
	}
	
	assocBrowser->SendMouseMove(PrevMouseLoc, true, true, GetPointerModifiers(mouseEvent));
}

FReply SUrBrowser::OnMouseMove(const FGeometry& geometry, const FPointerEvent& mouseEvent)
{
	auto assocBrowser = PersistentArgs._AssociatedBrowser.Get();
	if(!assocBrowser.IsValid())
	{
		return FReply::Unhandled();
	}
	
	if (ActiveTouches.Num() > 0)
	{
		return FReply::Handled();
	}

	FVector2D currLoc = GetUvFromScreenSpace(geometry, mouseEvent.GetScreenSpacePosition());
	if(PrevMouseLoc != currLoc)
	{
		PrevMouseLoc = currLoc;
		assocBrowser->SendMouseMove(currLoc, true, false, GetPointerModifiers(mouseEvent));
	}
	return FReply::Handled();
}

FReply SUrBrowser::OnMouseWheel(const FGeometry& geometry, const FPointerEvent& mouseEvent)
{
	auto assocBrowser = PersistentArgs._AssociatedBrowser.Get();
	if (!assocBrowser.IsValid())
	{
		return FReply::Unhandled();
	}
	
	assocBrowser->SendMouseWheel(
		GetUvFromScreenSpace(geometry, mouseEvent.GetScreenSpacePosition()),
		true,
		{ 0.0f, mouseEvent.GetWheelDelta() * 120 },
		GetPointerModifiers(mouseEvent)
	);
	return FReply::Handled();
}

FReply SUrBrowser::OnMouseButtonDown(const FGeometry& geometry, const FPointerEvent& mouseEvent)
{
	auto assocBrowser = PersistentArgs._AssociatedBrowser.Get();
	if (!assocBrowser.IsValid())
	{
		return FReply::Unhandled();
	}
	
	// Navigating in history with the thumb mouse buttons, and not send them explicitly to the browser
	if(PersistentArgs._UseThumbMouseButtons.Get())
	{
		if (mouseEvent.GetEffectingButton() == EKeys::ThumbMouseButton2)
		{
			assocBrowser->GoForward();
			return FReply::Handled();
		}
		if (mouseEvent.GetEffectingButton() == EKeys::ThumbMouseButton)
		{
			assocBrowser->GoBack();
			return FReply::Handled();
		}
	}

	if(mouseEvent.GetEffectingButton() == EKeys::LeftMouseButton && ActiveTouches.Num() > 0)
	{
		return FReply::Handled();
	}
	
	assocBrowser->SendMouseButton(
		GetUvFromScreenSpace(geometry, mouseEvent.GetScreenSpacePosition()),
		true,
		mouseEvent.GetEffectingButton(),
		false,
		GetPointerModifiers(mouseEvent)
	);
	return FReply::Handled();
}

FReply SUrBrowser::OnMouseButtonUp(const FGeometry& geometry, const FPointerEvent& mouseEvent)
{
	auto assocBrowser = PersistentArgs._AssociatedBrowser.Get();
	if (!assocBrowser.IsValid())
	{
		return FReply::Unhandled();
	}
	

	assocBrowser->SendMouseButton(
		GetUvFromScreenSpace(geometry, mouseEvent.GetScreenSpacePosition()),
		true,
		mouseEvent.GetEffectingButton(),
		true,
		GetPointerModifiers(mouseEvent)
	);
	return FReply::Handled();
}

FReply SUrBrowser::OnTouchStarted(const FGeometry& geometry, const FPointerEvent& touchEvent)
{
	auto assocBrowser = PersistentArgs._AssociatedBrowser.Get();
	if (!assocBrowser.IsValid())
	{
		return FReply::Unhandled();
	}
	
	ActiveTouches.Add(touchEvent.GetPointerIndex());
	assocBrowser->SendTouchEvent(
		{
			geometry,
			touchEvent,
			EUrPointerEventType::Pressed
		},
		GetPointerModifiers(touchEvent)
	);
	return FReply::Handled();
}

FReply SUrBrowser::OnTouchMoved(const FGeometry& geometry, const FPointerEvent& touchEvent)
{
	auto assocBrowser = PersistentArgs._AssociatedBrowser.Get();
	if (!assocBrowser.IsValid())
	{
		return FReply::Unhandled();
	}
	
	assocBrowser->SendTouchEvent(
		{
			geometry,
			touchEvent,
			EUrPointerEventType::Moved
		},
		GetPointerModifiers(touchEvent)
	);
	return FReply::Handled();
}

FReply SUrBrowser::OnTouchForceChanged(const FGeometry& geometry, const FPointerEvent& touchEvent)
{
	auto assocBrowser = PersistentArgs._AssociatedBrowser.Get();
	if (!assocBrowser.IsValid())
	{
		return FReply::Unhandled();
	}
	
	assocBrowser->SendTouchEvent(
		{
			geometry,
			touchEvent,
			EUrPointerEventType::Moved
		},
		GetPointerModifiers(touchEvent)
	);
	return FReply::Handled();
}

FReply SUrBrowser::OnTouchEnded(const FGeometry& geometry, const FPointerEvent& touchEvent)
{
	auto assocBrowser = PersistentArgs._AssociatedBrowser.Get();
	if (!assocBrowser.IsValid())
	{
		return FReply::Unhandled();
	}
	

	ActiveTouches.Remove(touchEvent.GetPointerIndex());
	assocBrowser->SendTouchEvent(
		{
			geometry,
			touchEvent,
			EUrPointerEventType::Released
		},
		GetPointerModifiers(touchEvent)
	);
	return FReply::Handled();
}

bool SUrBrowser::SupportsKeyboardFocus() const
{
	return true;
}

FReply SUrBrowser::OnKeyDown(const FGeometry& geometry, const FKeyEvent& keyEvent)
{
	auto assocBrowser = PersistentArgs._AssociatedBrowser.Get();
	if (!assocBrowser.IsValid())
	{
		return FReply::Unhandled();
	}
	
	assocBrowser->SendKeyEvent(keyEvent, false, GetModifiers(keyEvent));
	return FReply::Handled();
}

FReply SUrBrowser::OnKeyChar(const FGeometry& geometry, const FCharacterEvent& characterEvent)
{
	auto assocBrowser = PersistentArgs._AssociatedBrowser.Get();
	if (!assocBrowser.IsValid())
	{
		return FReply::Unhandled();
	}
	
	assocBrowser->SendCharacterKey(characterEvent, GetModifiers(characterEvent));
	return FReply::Handled();
}

FReply SUrBrowser::OnKeyUp(const FGeometry& geometry, const FKeyEvent& keyEvent)
{
	auto assocBrowser = PersistentArgs._AssociatedBrowser.Get();
	if (!assocBrowser.IsValid())
	{
		return FReply::Unhandled();
	}
	
	assocBrowser->SendKeyEvent(keyEvent, true, GetModifiers(keyEvent));
	return FReply::Handled();
}

TSharedPtr<SWidget> SUrBrowser::CreatePopupWidget()
{
	TSharedPtr<SWidget> OutWidget;

	if (PersistentArgs._UseOwnPopupWidget.Get())
	{
		SAssignNew(OutWidget, SUrPopup)
		. ParentBrowserWidget(SharedThis(this))
		. Visibility(EVisibility::HitTestInvisible);
	}
	else
	{
		OutWidget = PersistentArgs._PopupWidget.Get();
	}
	return OutWidget;
}

FVector2D SUrBrowser::GetUvFromScreenSpace(const FGeometry& geometry, FVector2D position)
{
	return geometry.AbsoluteToLocal(position) / geometry.GetLocalSize();
}

FVector2D SUrBrowser::GetUvFromBrowser(FVector2D position) const
{
	FVector2D desired, actual;
	PersistentArgs._AssociatedBrowser.Get()->GetMainSize(desired, actual);
	FVector2D uv = position / FVector2D(
		FMath::Max(actual.X, 1.0f),
		FMath::Max(actual.Y, 1.0f)
	);
	return uv;
}

FVector2D SUrBrowser::GetSizeForBrowserContent(const FGeometry& geometry)
{
	return geometry.GetAbsoluteSize();
}

void SUrBrowser::HandleOnPopupShow(bool show)
{
	if (show && !PopupWidgetPersistent)
	{
		PopupWidgetPersistent = CreatePopupWidget();
	}
	if (!show && PersistentArgs._AutoRemovePopup.Get())
	{
		RemovePopupWidget();
	}
}

void SUrBrowser::HandleOnPopupSize(FVector2D location, FVector2D size)
{
	FVector2D localPos = GetUvFromBrowser(location);
	FVector2D localSize = GetUvFromBrowser(size);
	localSize *= GetCachedGeometry().GetLocalSize();
	if (PersistentArgs._AssociatedBrowser.Get()->GetNativePopupShown())
	{
		// I'm not trusting execution order of PopupShow and PopupSize events of CEF
		if(!PopupWidgetPersistent)
		{
			PopupWidgetPersistent = CreatePopupWidget();
		}
		
		if(!PopupWidgetPersistent)
		{
			return;
		}

		Layout->AddSlot()
			. Anchors(FAnchors(localPos.X, localPos.Y, localPos.X, localPos.Y))
			. Alignment(FVector2D(0, 0))
			. Offset(FMargin(0, 0, localSize.X, localSize.Y))
			. ZOrder(1.0f)
			[ PopupWidgetPersistent.ToSharedRef() ];
	}
}

void SUrBrowser::CheckBrowserChange(TWeakObjectPtr<UUrBrowserView> assocBrowser)
{
	if(PrevBrowser.Get() != assocBrowser.Get())
	{
		if(PrevBrowser.IsValid())
		{
			PrevBrowser->OnNativePopupShowStatic.RemoveAll(this);
			PrevBrowser->OnNativePopupSizeStatic.RemoveAll(this);
		}
		PrevBrowser = assocBrowser;
		assocBrowser->OnNativePopupShowStatic.AddRaw(this, &SUrBrowser::HandleOnPopupShow);
		assocBrowser->OnNativePopupSizeStatic.AddRaw(this, &SUrBrowser::HandleOnPopupSize);
	}
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION