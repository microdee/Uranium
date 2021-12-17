/******************** (C) Copyright 2021 realworld one GmbH & Co. KG All Rights Reserved. ********************/

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UrCommonEnums.generated.h"

UENUM(BlueprintType)
enum class EUrNewWindowBehavior : uint8
{
	OpenInNewBrowser,
	OpenInSameBrowser,
	Suppress,
	Default
};

UENUM(BlueprintType)
enum class EUrBrowserCompositionSource : uint8
{
	CurrentComposition,
	ParentComposition,
	GlobalDefault
};

UENUM(BlueprintType)
enum class EUrBrowserViewClassSource : uint8
{
	CurrentComposition,
	CurrentCompositionClass,
	ParentComposition,
	GlobalDefault
};