// 

#include "Handlers/UrFindHandler.h"

#include "Handlers/UrHandlerEvents.h"

UUrFindEvents* ICefFindHandler::GetFindEvents()
{
	return UUrFindEvents::GetEvents(UrFindHandler.ToScriptInterface());
}

UUrFindEvents* UUrFindEvents::GetEvents(TScriptInterface<IUrFindHandler> handler)
{
	auto subSys = GEngine->GetEngineSubsystem<UUrHandlerEvents>();
	return Cast<UUrFindEvents>(subSys->GetEvents(handler.GetObject(), StaticClass()));
}