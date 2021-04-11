// Fill out your copyright notice in the Description page of Project Settings.

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
