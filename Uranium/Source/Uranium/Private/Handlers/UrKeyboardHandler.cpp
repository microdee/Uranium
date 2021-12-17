// 

#include "Handlers/UrKeyboardHandler.h"

#include "Handlers/UrHandlerEvents.h"

UUrKeyboardEvents* ICefKeyboardHandler::GetKeyboardEvents()
{
	return UUrKeyboardEvents::GetEvents(UrKeyboardHandler.ToScriptInterface());
}

UUrKeyboardEvents* UUrKeyboardEvents::GetEvents(TScriptInterface<IUrKeyboardHandler> handler)
{
	auto subSys = GEngine->GetEngineSubsystem<UUrHandlerEvents>();
	return Cast<UUrKeyboardEvents>(subSys->GetEvents(handler.GetObject(), StaticClass()));
}