/******************** (C) Copyright 2021 realworld one GmbH & Co. KG All Rights Reserved. ********************/

#pragma once

#include "CoreMinimal.h"
#include "CoreUObject.h"
#include "UrJsObjectInterop.generated.h"

/**
 * 
 */
UCLASS()
class URANIUM_API UUrJsObjectInterop : public UObject
{
	GENERATED_BODY()

	TMap<FString, UObject> PersistentWindowObjects;

public:
};