// 

#include "Handlers/UrAudioHandler.h"

#include "Handlers/UrHandlerEvents.h"

UUrAudioEvents* ICefAudioHandler::GetAudioEvents()
{
	return UUrAudioEvents::GetEvents(UrAudioHandler.ToScriptInterface());
}

UUrAudioEvents* UUrAudioEvents::GetEvents(TScriptInterface<IUrAudioHandler> handler)
{
	auto subSys = GEngine->GetEngineSubsystem<UUrHandlerEvents>();
	return Cast<UUrAudioEvents>(subSys->GetEvents(handler.GetObject(), StaticClass()));
}