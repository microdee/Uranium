/******************** (C) Copyright 2021 realworld one GmbH & Co. KG All Rights Reserved. ********************/
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "UrSettings.generated.h"

USTRUCT(BlueprintType)
struct FUraniumSwitch
{
	GENERATED_BODY()

	FUraniumSwitch()
		: Switch("")
		, HasValue(false)
		, Value("") {}

	FUraniumSwitch(FString switchName)
		: Switch(switchName)
		, HasValue(false)
		, Value("") {}

	FUraniumSwitch(FString switchName, FString val)
		: Switch(switchName)
		, HasValue(true)
		, Value(val) {}

	UPROPERTY(EditAnywhere)
	FString Switch;

	UPROPERTY(EditAnywhere, meta = (InlineEditConditionToggle))
	bool HasValue;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "HasValue"))
	FString Value;

	bool IsEmpty();
};

UCLASS(Config = Game, defaultconfig)
class URANIUM_API UUrSettings : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
	TArray<FUraniumSwitch> ChromiumSwitches =
	{
		FUraniumSwitch("ignore-gpu-blacklist"),
		FUraniumSwitch("enable-experimental-canvas-features"),
		//FUraniumSwitch("disable-gpu-vsync"),
		//FUraniumSwitch("use-angle", "d3d11"),
		FUraniumSwitch("smooth-scrolling"),
		FUraniumSwitch("enable-features", "OverlayScrollbar"),
		FUraniumSwitch("allow-file-access"),
		FUraniumSwitch("allow-file-access-from-files"),
		FUraniumSwitch("allow-cross-origin-auth-prompt"),
		FUraniumSwitch("autoplay-policy", "no-user-gesture-required"),
		FUraniumSwitch("touch-events", "enabled")
	};

	UPROPERTY(EditAnywhere)
	int RemoteDebugPort = 9223;

	UPROPERTY(EditAnywhere)
	bool PersistSessionCookies = true;

	UPROPERTY(EditAnywhere)
	bool PersistUserPreferences = true;

	UPROPERTY(EditAnywhere)
	FColor DefaultBackgroundColor = FColor::White;

	static void RegisterSettings();

	static void UnregisterSettings();
};