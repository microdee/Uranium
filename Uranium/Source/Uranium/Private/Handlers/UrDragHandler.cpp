// 

#include "Handlers/UrDragHandler.h"

#include "Handlers/UrHandlerEvents.h"

UUrDragEvents* ICefDragHandler::GetDragEvents()
{
	return UUrDragEvents::GetEvents(UrDragHandler.ToScriptInterface());
}

UUrDragEvents* UUrDragEvents::GetEvents(TScriptInterface<IUrDragHandler> handler)
{
	auto subSys = GEngine->GetEngineSubsystem<UUrHandlerEvents>();
	return Cast<UUrDragEvents>(subSys->GetEvents(handler.GetObject(), StaticClass()));
}