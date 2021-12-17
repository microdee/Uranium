/******************** (C) Copyright 2021 realworld one GmbH & Co. KG All Rights Reserved. ********************/

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UrHandlerEvents.generated.h"

USTRUCT()
struct FHandlerEventsPair
{
	GENERATED_BODY()
	
	UPROPERTY()
	TSoftObjectPtr<UObject> Handler;
	
	UPROPERTY()
	UObject* Events;
};

/**
 * Manage linkage between handler classes and events fired on them
 */
UCLASS()
class URANIUM_API UUrHandlerEvents : public UEngineSubsystem
{
	GENERATED_BODY()
private:

	UPROPERTY()
	TMap<int64, FHandlerEventsPair> HandlersEvents;
	
	FCriticalSection DataLock;

	void CleanUp();
	
public:
	virtual void Initialize(FSubsystemCollectionBase& collection) override;
	virtual void Deinitialize() override;

	static int64 GetID(TSoftObjectPtr<UObject> handler, UClass* eventsObjectClass);
	UObject* GetEvents(TSoftObjectPtr<UObject> handler, UClass* eventsObjectClass);
};
