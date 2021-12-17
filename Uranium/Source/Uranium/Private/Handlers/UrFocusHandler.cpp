// 

#include "Handlers/UrFocusHandler.h"

#include "Handlers/UrHandlerEvents.h"

UUrFocusEvents* ICefFocusHandler::GetFocusEvents()
{
	return UUrFocusEvents::GetEvents(UrFocusHandler.ToScriptInterface());
}

UUrFocusEvents* UUrFocusEvents::GetEvents(TScriptInterface<IUrFocusHandler> handler)
{
	auto subSys = GEngine->GetEngineSubsystem<UUrHandlerEvents>();
	return Cast<UUrFocusEvents>(subSys->GetEvents(handler.GetObject(), StaticClass()));
}