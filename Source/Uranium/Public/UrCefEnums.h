#pragma once

#include "CoreMinimal.h"

#include "UObject/Interface.h"

#include "UrCefEnums.generated.h"

UENUM(BlueprintType)
enum class EUrTextInputMode : uint8
{
	Default,
	None,
	Text,
	Tel,
	Url,
	Email,
	Numeric,
	Decimal,
	Search
};

UENUM(BlueprintType)
enum class EUrLogSeverity : uint8
{
	Default,
	Verbose,
	Debug = Verbose,
	Info,
	Warning,
	Error,
	Fatal,
	Disable = 99
};

UENUM(BlueprintType)
enum class EUrWindowDisposition : uint8
{
	Unknown,
	CurrentTab,
	SingletonTab,
	NewForegroundTab,
	NewBackgroundTab,
	NewPopup,
	NewWindow,
	SaveToDisk,
	OffTheRecord,
	IgnoreAction
};

UENUM(BlueprintType)
enum class EUrGetBrowserSize : uint8
{
	/** Set by a size parameter */
	Manually,

	/**
	 * Set by the desired size of a new window,
	 * or manually when it's a top level browser.
	 */
	FromDesiredOrManually,

	/**
	 * Set by the desired size of a new window,
	 * or from the local size of the widget's geometry when it's a top level browser.
	 */
	FromDesiredOrFromWidgetLocalSize,

	/**
	 * Set by the desired size of a new window,
	 * or from the absolute size of the widget's geometry when it's a top level browser.
	 */
	FromDesiredOrFromWidgetAbsoluteSize,

	/**
	 * Determine size from the local size of the widget's geometry.
	 * This respects scaling of the widget but it won't be pixel-perfect.
	 */
	FromWidgetLocalSize,

	/**
	 * Determine size from the absolute size of the widget's geometry.
	 * This maintains pixel-perfect look, but it won't respect scaling
	 * including scaling from render transformation.
	 */
	FromWidgetAbsoluteSize
};

USTRUCT(BlueprintType)
struct FUrCefErrorCode
{
	GENERATED_BODY()

private:
	static TMap<int, FString> ErrorCodes;

public:

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Uranium")
	int Code = 0;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Uranium")
	FString Output;

	FUrCefErrorCode() { }

	FUrCefErrorCode(int input)
	{
		Code = input;
		Output = ErrorCodes[input];
	}
};

USTRUCT(BlueprintType)
struct FNewBrowserMetadata
{
	GENERATED_BODY()

	FNewBrowserMetadata() {}

	FNewBrowserMetadata(
		const FString& TargetUrl,
		const FString& TargetFrameName,
		const EUrWindowDisposition TargetDisposition,
		const FVector2D& DesiredSize
	) : TargetUrl(TargetUrl)
	  , TargetFrameName(TargetFrameName)
	  , TargetDisposition(TargetDisposition)
	  , DesiredSize(DesiredSize)
	{
	}

	UPROPERTY(BlueprintReadWrite, Category = "Uranium")
	FString TargetUrl = "";

	UPROPERTY(BlueprintReadWrite, Category = "Uranium")
	FString TargetFrameName = "";

	UPROPERTY(BlueprintReadWrite, Category = "Uranium")
	EUrWindowDisposition TargetDisposition = EUrWindowDisposition::NewWindow;

	UPROPERTY(BlueprintReadWrite, Category = "Uranium")
	FVector2D DesiredSize = { 800, 600 };

	// TODO: Popup features CefPopupFeatures
};

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