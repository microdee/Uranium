// 

#include "Handlers/UrDownloadHandler.h"

#include "Handlers/UrHandlerEvents.h"

UUrDownloadEvents* ICefDownloadHandler::GetDownloadEvents()
{
	return UUrDownloadEvents::GetEvents(UrDownloadHandler.ToScriptInterface());
}

UUrDownloadEvents* UUrDownloadEvents::GetEvents(TScriptInterface<IUrDownloadHandler> handler)
{
	auto subSys = GEngine->GetEngineSubsystem<UUrHandlerEvents>();
	return Cast<UUrDownloadEvents>(subSys->GetEvents(handler.GetObject(), StaticClass()));
}