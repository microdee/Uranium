// 

#include "Handlers/UrJSDialogHandler.h"

#include "Handlers/UrHandlerEvents.h"

UUrJSDialogEvents* ICefJSDialogHandler::GetJSDialogEvents()
{
	return UUrJSDialogEvents::GetEvents(UrJSDialogHandler.ToScriptInterface());
}

UUrJSDialogEvents* UUrJSDialogEvents::GetEvents(TScriptInterface<IUrJSDialogHandler> handler)
{
	auto subSys = GEngine->GetEngineSubsystem<UUrHandlerEvents>();
	return Cast<UUrJSDialogEvents>(subSys->GetEvents(handler.GetObject(), StaticClass()));
}