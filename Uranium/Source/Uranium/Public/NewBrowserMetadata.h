/******************** (C) Copyright 2021 realworld one GmbH & Co. KG All Rights Reserved. ********************/

#pragma once

#include "CoreMinimal.h"

#include "UrCefEnums.h"
#include "UObject/Object.h"
#include "NewBrowserMetadata.generated.h"

USTRUCT(BlueprintType)
struct FNewBrowserMetadata
{
	GENERATED_BODY()

	FNewBrowserMetadata() {}

	FNewBrowserMetadata(
		const FString& targetUrl,
		const FString& targetFrameName,
		const EUrWindowDisposition targetDisposition,
		const FVector2D& desiredSize = { 800, 600 },
		bool userGesture = false
	) : TargetUrl(targetUrl)
	, TargetFrameName(targetFrameName)
	, TargetDisposition(targetDisposition)
	, DesiredSize(desiredSize)
	, UserGesture(userGesture)
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
	
	UPROPERTY(BlueprintReadWrite, Category = "Uranium")
	bool UserGesture = false;

	// TODO: Popup features CefPopupFeatures
};