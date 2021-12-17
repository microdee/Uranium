/******************** (C) Copyright 2021 realworld one GmbH & Co. KG All Rights Reserved. ********************/

#pragma once

#include "CoreMinimal.h"

#include "CefIncludesStart.h"
#include "include/cef_base.h"
#include "CefIncludesEnd.h"

#include "UrInput.generated.h"


USTRUCT(BlueprintType)
struct FCefKeyEventFlags
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite)
	bool CapsLocked = false;

	UPROPERTY(BlueprintReadWrite)
	bool ShiftDown = false;

	UPROPERTY(BlueprintReadWrite)
	bool ControlDown = false;

	UPROPERTY(BlueprintReadWrite)
	bool AltDown = false;
	
	UPROPERTY(BlueprintReadWrite)
	bool LeftMouseButton = false;
	
	UPROPERTY(BlueprintReadWrite)
	bool MiddleMouseButton = false;
	
	UPROPERTY(BlueprintReadWrite)
	bool RightMouseButton = false;

	UPROPERTY(BlueprintReadWrite)
	bool CommandDown = false;
	
	UPROPERTY(BlueprintReadWrite)
	bool NumLocked = false;
	
	UPROPERTY(BlueprintReadWrite)
	bool IsKeypad = false;
	
	UPROPERTY(BlueprintReadWrite)
	bool Left = false;
	
	UPROPERTY(BlueprintReadWrite)
	bool Right = false;

	UPROPERTY(BlueprintReadWrite)
	bool AltGrDown = false;

	uint32 OrFlag = 0;

	uint32  GetEventFlags() const;
};

UENUM(BlueprintType)
enum class EUrPointerType : uint8
{
	Touch = 0,
	Mouse,
	Pen,
	Eraser,
	Unknown
};

UENUM(BlueprintType)
enum class EUrPointerEventType : uint8
{
	Released = 0,
	Pressed,
	Moved,
	Cancelled
};

namespace UrInput
{
	inline int GetUraniumMouseButton(const FKey& button)
	{
		if (button == EKeys::LeftMouseButton) return MBT_LEFT;
		if (button == EKeys::RightMouseButton) return MBT_RIGHT;
		if (button == EKeys::MiddleMouseButton) return MBT_MIDDLE;
		return MBT_LEFT;
	}

	inline bool IsValidMouseButton(const FKey& button)
	{
		return button == EKeys::LeftMouseButton
			|| button == EKeys::RightMouseButton
			|| button == EKeys::MiddleMouseButton;
	}

	inline cef_event_flags_t GetCefModifierFlagForKey(const FKey& button)
	{
		if (button == EKeys::LeftShift || button == EKeys::RightShift)
			return EVENTFLAG_SHIFT_DOWN;
		if (button == EKeys::LeftControl || button == EKeys::RightControl)
			return EVENTFLAG_CONTROL_DOWN;
		if (button == EKeys::LeftAlt) return EVENTFLAG_ALT_DOWN;
		if (button == EKeys::LeftMouseButton) return EVENTFLAG_LEFT_MOUSE_BUTTON;
		if (button == EKeys::MiddleMouseButton) return EVENTFLAG_MIDDLE_MOUSE_BUTTON;
		if (button == EKeys::RightMouseButton) return EVENTFLAG_RIGHT_MOUSE_BUTTON;
		if (button == EKeys::LeftCommand || button == EKeys::RightCommand)
			return EVENTFLAG_COMMAND_DOWN;
		if (button == EKeys::RightAlt) return EVENTFLAG_ALTGR_DOWN;
		return EVENTFLAG_NONE;
	}
	
	template<typename TCefEvent>
	void GetModifierKeys(const FInputEvent& ueKey, TCefEvent& cefKey, const FCefKeyEventFlags& extraFlags)
	{
		// as of cef_event_flags_t
		cefKey.modifiers = ueKey.AreCapsLocked() << 0
			| ueKey.IsShiftDown() << 1
			| ueKey.IsControlDown() << 2
			| ueKey.IsLeftAltDown() << 3
			| extraFlags.LeftMouseButton << 4
			| extraFlags.MiddleMouseButton << 5
			| extraFlags.RightMouseButton << 6
			| ueKey.IsCommandDown() << 7
			| extraFlags.NumLocked << 8
			| extraFlags.IsKeypad << 9
			| extraFlags.Left << 10
			| extraFlags.Right << 11
			| ueKey.IsRightAltDown() << 12
			| extraFlags.OrFlag;
	}

	inline FCefKeyEventFlags EnforceFlagsFromEvent(const FInputEvent& ueKey, FCefKeyEventFlags extraFlags)
	{
		extraFlags.CapsLocked = ueKey.AreCapsLocked();
		extraFlags.ShiftDown = ueKey.IsShiftDown();
		extraFlags.ControlDown = ueKey.IsControlDown();
		extraFlags.AltDown = ueKey.IsLeftAltDown();
		extraFlags.CommandDown = ueKey.IsCommandDown();
		extraFlags.AltGrDown = ueKey.IsRightAltDown();
		return extraFlags;
	}
}

USTRUCT(BlueprintType)
struct FUrPointerEvent
{
	GENERATED_BODY()

	FUrPointerEvent(): Id(), IsUVSpace(false), RotationDegrees(0), Pressure(0)
	{
	}

	FUrPointerEvent(
		const FGeometry& geometry,
		const FPointerEvent& inEvent,
		EUrPointerEventType eventType,
		EUrPointerType pointerType = EUrPointerType::Touch
	)	: Id(static_cast<ETouchIndex::Type>(inEvent.GetPointerIndex()))
		, Location(geometry.AbsoluteToLocal(inEvent.GetScreenSpacePosition()) / geometry.GetLocalSize())
		, Size(FVector2D::ZeroVector)
		, IsUVSpace(true)
		, RotationDegrees(0.0f)
		, Pressure(inEvent.GetTouchForce())
		, EventType(eventType)
		, PointerType(pointerType)
	{
	}

	FUrPointerEvent(
		uint32 id,
		const FVector2D& location,
		const FVector2D& size,
		bool isUVSpace,
		float rotationDegrees,
		float pressure,
		EUrPointerEventType eventType,
		EUrPointerType pointerType
	)	: Id(static_cast<ETouchIndex::Type>(id))
		, Location(location)
		, Size(size)
		, IsUVSpace(isUVSpace)
		, RotationDegrees(rotationDegrees)
		, Pressure(pressure)
		, EventType(eventType)
		, PointerType(pointerType)
	{
	}

	UPROPERTY(BlueprintReadWrite, Category = "Uranium")
	TEnumAsByte<ETouchIndex::Type> Id;

	UPROPERTY(BlueprintReadWrite, Category = "Uranium")
	FVector2D Location;

	UPROPERTY(BlueprintReadWrite, Category = "Uranium")
	FVector2D Size = FVector2D::ZeroVector;

	UPROPERTY(BlueprintReadWrite, Category = "Uranium")
	bool IsUVSpace;

	UPROPERTY(BlueprintReadWrite, Category = "Uranium")
	float RotationDegrees;

	UPROPERTY(BlueprintReadWrite, Category = "Uranium")
	float Pressure;

	UPROPERTY(BlueprintReadWrite, Category = "Uranium")
	EUrPointerEventType EventType = EUrPointerEventType::Pressed;

	UPROPERTY(BlueprintReadWrite, Category = "Uranium")
	EUrPointerType PointerType = EUrPointerType::Touch;
};