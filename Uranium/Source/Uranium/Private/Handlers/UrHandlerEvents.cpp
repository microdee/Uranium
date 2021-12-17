/******************** (C) Copyright 2021 realworld one GmbH & Co. KG All Rights Reserved. ********************/

#include "Handlers/UrHandlerEvents.h"

void UUrHandlerEvents::CleanUp()
{
	FScopeLock lock(&DataLock);
	
	TArray<int64> removables;
	for(auto kvp : HandlersEvents)
	{
		if(kvp.Value.Handler.IsValid()) continue;
		removables.Add(kvp.Key);
	}
	for (auto handler : removables)
	{
		HandlersEvents.Remove(handler);
	}
}

void UUrHandlerEvents::Initialize(FSubsystemCollectionBase& collection)
{
}

void UUrHandlerEvents::Deinitialize()
{
}

int64 UUrHandlerEvents::GetID(TSoftObjectPtr<UObject> handler, UClass* eventsObjectClass)
{
	const int64 handlerPtr = reinterpret_cast<int64>(handler.Get());
	const int64 classPtr = reinterpret_cast<int64>(eventsObjectClass);
	return handlerPtr ^ classPtr;
}

UObject* UUrHandlerEvents::GetEvents(TSoftObjectPtr<UObject> handler, UClass* eventsObjectClass)
{
	if(IsGarbageCollecting() && IsInGameThread())
	{
		return nullptr;
	}
	
	if(handler.IsValid() && eventsObjectClass)
	{
		CleanUp();
		int64 id = GetID(handler, eventsObjectClass);
		if(!HandlersEvents.Contains(id))
		{
			HandlersEvents.Add(id, FHandlerEventsPair{
				handler,
				NewObject<UObject>(handler.Get(), eventsObjectClass)
			});
		}
		return HandlersEvents[id].Events;
	}
	return nullptr;
}
