// 

#include "Handlers//UrDefaultAudioHandler.h"

void UUrDefaultAudioHandler::PostInitProperties()
{
	Super::PostInitProperties();
	CefAudioHandler = new FCefDefaultAudioHandler();
	CefAudioHandler->UrAudioHandler = TScriptInterface<IUrAudioHandler>(this);
}

void UUrDefaultAudioHandlerStatics::CreateDefaultCefAudioHandler(TScriptInterface<IUrAudioHandler> handler)
{
	handler->CefAudioHandler = new FCefDefaultAudioHandler();
	handler->CefAudioHandler->UrAudioHandler = handler;
}
