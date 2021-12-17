// 

#include "Handlers/UrDialogHandler.h"

#include "Handlers/UrHandlerEvents.h"

UUrDialogEvents* UUrDialogEvents::GetEvents(TScriptInterface<IUrDialogHandler> handler)
{
	auto subSys = GEngine->GetEngineSubsystem<UUrHandlerEvents>();
	return Cast<UUrDialogEvents>(subSys->GetEvents(handler.GetObject(), StaticClass()));
}