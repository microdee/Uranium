// 

#include "Handlers/UrDevToolsHandler.h"

#include "Handlers/UrHandlerEvents.h"

UUrDevToolEvents* UUrDevToolEvents::GetEvents(TScriptInterface<IUrDevToolsHandler> handler)
{
	auto subSys = GEngine->GetEngineSubsystem<UUrHandlerEvents>();
	return Cast<UUrDevToolEvents>(subSys->GetEvents(handler.GetObject(), StaticClass()));
}