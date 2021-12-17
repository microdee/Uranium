// 

#include "Handlers/UrContextMenuHandler.h"

#include "Handlers/UrHandlerEvents.h"

UUrContextMenuEvents* UUrContextMenuEvents::GetEvents(TScriptInterface<IUrContextMenuHandler> handler)
{
	auto subSys = GEngine->GetEngineSubsystem<UUrHandlerEvents>();
	return Cast<UUrContextMenuEvents>(subSys->GetEvents(handler.GetObject(), StaticClass()));
}