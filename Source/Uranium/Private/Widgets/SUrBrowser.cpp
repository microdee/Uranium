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
void SUrBrowser::Construct(const FArguments& InArgs)
{
	PersistentArgs = InArgs;

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

void SUrBrowser::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	auto AssocBrows = PersistentArgs._AssociatedBrowser.Get();
	if(AssocBrows.IsValid() && AssocBrows->Instance)
	{
		if(PrevBrowser != AssocBrows.Get())
		{
			PrevBrowser = AssocBrows.Get();
			AssocBrows->OnNativePopupShowStatic.AddRaw(this, &SUrBrowser::HandleOnPopupShow);
			AssocBrows->OnNativePopupSizeStatic.AddRaw(this, &SUrBrowser::HandleOnPopupSize);
		}

		FVector2D LocalSize = GetSizeForBrowserContent(AllottedGeometry);
		FVector2D ActualSize;
		switch (PersistentArgs._BrowserSizeMode.Get())
		{
		case EUrGetBrowserSize::Manually:
			ActualSize = PersistentArgs._ManualSize.Get();
			break;

		case EUrGetBrowserSize::FromDesiredOrManually:
			ActualSize = PersistentArgs._InitMetadata.Get().DesiredSize.Size() < UE_SQRT_2
				? PersistentArgs._ManualSize.Get()
				: PersistentArgs._InitMetadata.Get().DesiredSize;
			break;

		case EUrGetBrowserSize::FromDesiredOrFromWidgetLocalSize:
			ActualSize = PersistentArgs._InitMetadata.Get().DesiredSize.Size() < UE_SQRT_2
				? AllottedGeometry.GetLocalSize()
				: PersistentArgs._InitMetadata.Get().DesiredSize;
			break;

		case EUrGetBrowserSize::FromDesiredOrFromWidgetAbsoluteSize:
			ActualSize = PersistentArgs._InitMetadata.Get().DesiredSize.Size() < UE_SQRT_2
				? AllottedGeometry.GetAbsoluteSize()
				: PersistentArgs._InitMetadata.Get().DesiredSize;
			break;

		case EUrGetBrowserSize::FromWidgetLocalSize:
			ActualSize = AllottedGeometry.GetLocalSize();
			break;

		case EUrGetBrowserSize::FromWidgetAbsoluteSize:
			ActualSize = AllottedGeometry.GetAbsoluteSize();
			break;

		default: break;
		}

		//AssocBrows->ScaleFactor = FMath::Max(ScaleFactor.Get(), 1.0f);
		//ActualSize *= AssocBrows->ScaleFactor;

		if (PersistentArgs._DoDeferredInit.Get() && !AssocBrows->Instance->Obj)
		{
			AssocBrows->DeferredInit(ActualSize, PersistentArgs._InitMetadata.Get().TargetUrl);
		}
		else
		{
			AssocBrows->SetMainSize(ActualSize);
		}

		if (PersistentArgs._DoTick.Get())
			AssocBrows->Tick(InDeltaTime);

		if(AssocBrows->GetMainTexture())
		{
			BaseImage->SetVisibility(EVisibility::Visible);
			FVector2D Desired, Actual;
			AssocBrows->GetMainSize(Desired, Actual);

			if (PrevTextureID != AssocBrows->GetMainTexture())
			{
				PrevTextureID = AssocBrows->GetMainTexture();
				if (AssocBrows->GetMainTexture())
				{
					PersistentBrush.Reset();
					PersistentBrush =
						FDeferredCleanupSlateBrush::CreateBrush(AssocBrows->GetMainTexture());
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

FCefKeyEventFlags SUrBrowser::GetModifiers(const FInputEvent& Event)
{
	PersistentModifiers.CapsLocked = Event.AreCapsLocked();
	PersistentModifiers.ShiftDown = Event.IsShiftDown();
	PersistentModifiers.ControlDown = Event.IsControlDown();
	PersistentModifiers.AltDown = Event.IsLeftAltDown();
	PersistentModifiers.CommandDown = Event.IsCommandDown();
	PersistentModifiers.AltGrDown = Event.IsRightAltDown();
	return PersistentModifiers;
}

FCefKeyEventFlags SUrBrowser::GetPointerModifiers(const FPointerEvent& Event)
{
	PersistentModifiers = {};
	for(auto& Button : Event.GetPressedButtons())
	{
		PersistentModifiers.LeftMouseButton |= Button == EKeys::LeftMouseButton;
		PersistentModifiers.MiddleMouseButton |= Button == EKeys::MiddleMouseButton;
		PersistentModifiers.RightMouseButton |= Button == EKeys::RightMouseButton;
	}
	return GetModifiers(Event);
}

void SUrBrowser::OnMouseLeave(const FPointerEvent& MouseEvent)
{
	auto AssocBrows = PersistentArgs._AssociatedBrowser.Get();
	if (!AssocBrows.IsValid()) return;
	AssocBrows->SendMouseMove(PrevMouseLoc, true, true, GetPointerModifiers(MouseEvent));
}

FReply SUrBrowser::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	auto AssocBrows = PersistentArgs._AssociatedBrowser.Get();
	if(!AssocBrows.IsValid()) return FReply::Unhandled();
	
	if (ActiveTouches.Num() > 0)
	{
		return FReply::Handled();
	}

	auto CurrLoc = GetUvFromScreenSpace(MyGeometry, MouseEvent.GetScreenSpacePosition());
	if(PrevMouseLoc != CurrLoc)
	{
		PrevMouseLoc = CurrLoc;
		AssocBrows->SendMouseMove(CurrLoc, true, false, GetPointerModifiers(MouseEvent));
	}
	return FReply::Handled();
}

FReply SUrBrowser::OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	auto AssocBrows = PersistentArgs._AssociatedBrowser.Get();
	if (!AssocBrows.IsValid()) return FReply::Unhandled();
	AssocBrows->SendMouseWheel(
		GetUvFromScreenSpace(MyGeometry, MouseEvent.GetScreenSpacePosition()),
		true,
		{ 0.0f, MouseEvent.GetWheelDelta() * 120 },
		GetPointerModifiers(MouseEvent)
	);
	return FReply::Handled();
}

FReply SUrBrowser::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	auto AssocBrows = PersistentArgs._AssociatedBrowser.Get();
	if (!AssocBrows.IsValid()) return FReply::Unhandled();

	if(PersistentArgs._UseThumbMouseButtons.Get())
	{
		if (MouseEvent.GetEffectingButton() == EKeys::ThumbMouseButton2)
		{
			AssocBrows->GoForward();
			return FReply::Handled();
		}
		if (MouseEvent.GetEffectingButton() == EKeys::ThumbMouseButton)
		{
			AssocBrows->GoBack();
			return FReply::Handled();
		}
	}

	if(MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton && ActiveTouches.Num() > 0)
	{
		return FReply::Handled();
	}
	
	AssocBrows->SendMouseButton(
		GetUvFromScreenSpace(MyGeometry, MouseEvent.GetScreenSpacePosition()),
		true,
		MouseEvent.GetEffectingButton(),
		false,
		GetPointerModifiers(MouseEvent)
	);
	return FReply::Handled();
}

FReply SUrBrowser::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	auto AssocBrows = PersistentArgs._AssociatedBrowser.Get();
	if (!AssocBrows.IsValid()) return FReply::Unhandled();

	AssocBrows->SendMouseButton(
		GetUvFromScreenSpace(MyGeometry, MouseEvent.GetScreenSpacePosition()),
		true,
		MouseEvent.GetEffectingButton(),
		true,
		GetPointerModifiers(MouseEvent)
	);
	return FReply::Handled();
}

FReply SUrBrowser::OnTouchStarted(const FGeometry& MyGeometry, const FPointerEvent& InTouchEvent)
{
	auto AssocBrows = PersistentArgs._AssociatedBrowser.Get();
	if (!AssocBrows.IsValid()) return FReply::Unhandled();
	ActiveTouches.Add(InTouchEvent.GetPointerIndex());
	AssocBrows->SendTouchEvent(
		{
			MyGeometry,
			InTouchEvent,
			EUrPointerEventType::Pressed
		},
		GetPointerModifiers(InTouchEvent)
	);
	return FReply::Handled();
}

FReply SUrBrowser::OnTouchMoved(const FGeometry& MyGeometry, const FPointerEvent& InTouchEvent)
{
	auto AssocBrows = PersistentArgs._AssociatedBrowser.Get();
	if (!AssocBrows.IsValid()) return FReply::Unhandled();
	AssocBrows->SendTouchEvent(
		{
			MyGeometry,
			InTouchEvent,
			EUrPointerEventType::Moved
		},
		GetPointerModifiers(InTouchEvent)
	);
	return FReply::Handled();
}

FReply SUrBrowser::OnTouchForceChanged(const FGeometry& MyGeometry, const FPointerEvent& TouchEvent)
{
	auto AssocBrows = PersistentArgs._AssociatedBrowser.Get();
	if (!AssocBrows.IsValid()) return FReply::Unhandled();
	AssocBrows->SendTouchEvent(
		{
			MyGeometry,
			TouchEvent,
			EUrPointerEventType::Moved
		},
		GetPointerModifiers(TouchEvent)
	);
	return FReply::Handled();
}

FReply SUrBrowser::OnTouchEnded(const FGeometry& MyGeometry, const FPointerEvent& InTouchEvent)
{
	auto AssocBrows = PersistentArgs._AssociatedBrowser.Get();
	if (!AssocBrows.IsValid()) return FReply::Unhandled();

	ActiveTouches.Remove(InTouchEvent.GetPointerIndex());
	AssocBrows->SendTouchEvent(
		{
			MyGeometry,
			InTouchEvent,
			EUrPointerEventType::Released
		},
		GetPointerModifiers(InTouchEvent)
	);
	return FReply::Handled();
}

bool SUrBrowser::SupportsKeyboardFocus() const
{
	return true;
}

FReply SUrBrowser::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	auto AssocBrows = PersistentArgs._AssociatedBrowser.Get();
	if (!AssocBrows.IsValid()) return FReply::Unhandled();
	AssocBrows->SendKeyEvent(InKeyEvent, false, GetModifiers(InKeyEvent));
	return FReply::Handled();
}

FReply SUrBrowser::OnKeyChar(const FGeometry& MyGeometry, const FCharacterEvent& InCharacterEvent)
{
	auto AssocBrows = PersistentArgs._AssociatedBrowser.Get();
	if (!AssocBrows.IsValid()) return FReply::Unhandled();
	AssocBrows->SendCharacterKey(InCharacterEvent, GetModifiers(InCharacterEvent));
	return FReply::Handled();
}

FReply SUrBrowser::OnKeyUp(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	auto AssocBrows = PersistentArgs._AssociatedBrowser.Get();
	if (!AssocBrows.IsValid()) return FReply::Unhandled();
	AssocBrows->SendKeyEvent(InKeyEvent, true, GetModifiers(InKeyEvent));
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

FVector2D SUrBrowser::GetUvFromScreenSpace(const FGeometry& Geom, FVector2D InPos)
{
	return Geom.AbsoluteToLocal(InPos) / Geom.GetLocalSize();
}

FVector2D SUrBrowser::GetUvFromBrowser(FVector2D InPos) const
{
	FVector2D Desired, Actual;
	PersistentArgs._AssociatedBrowser.Get()->GetMainSize(Desired, Actual);
	auto Uv = InPos / FVector2D(
		FMath::Max(Actual.X, 1.0f),
		FMath::Max(Actual.Y, 1.0f)
	);
	return Uv;
}

FVector2D SUrBrowser::GetSizeForBrowserContent(const FGeometry& Geom)
{
	auto Ret = Geom.GetAbsoluteSize();
	return Ret;
}

void SUrBrowser::HandleOnPopupShow(bool Show)
{
	if (Show && !PopupWidgetPersistent)
	{
		PopupWidgetPersistent = CreatePopupWidget();
	}
	else if(PersistentArgs._AutoRemovePopup.Get())
		RemovePopupWidget();
}

void SUrBrowser::HandleOnPopupSize(FVector2D Location, FVector2D Size)
{
	auto LocalPos = GetUvFromBrowser(Location);
	auto LocalSize = GetUvFromBrowser(Size);
	LocalSize *= GetCachedGeometry().GetLocalSize();
	if (PersistentArgs._AssociatedBrowser.Get()->GetNativePopupShown())
	{
		// I'm not trusting execution order of PopupShow and PopupSize events of CEF
		if(!PopupWidgetPersistent) PopupWidgetPersistent = CreatePopupWidget();
		if(!PopupWidgetPersistent) return;

		Layout->AddSlot()
			. Anchors(FAnchors(LocalPos.X, LocalPos.Y, LocalPos.X, LocalPos.Y))
			. Alignment(FVector2D(0, 0))
			. Offset(FMargin(0, 0, LocalSize.X, LocalSize.Y))
			. ZOrder(1.0f)
			[ PopupWidgetPersistent.ToSharedRef() ];
	}
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION