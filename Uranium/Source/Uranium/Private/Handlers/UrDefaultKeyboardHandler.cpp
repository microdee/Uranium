// 

#include "Handlers//UrDefaultKeyboardHandler.h"

void UUrDefaultKeyboardHandler::PostInitProperties()
{
	Super::PostInitProperties();
	CefKeyboardHandler = new FCefDefaultKeyboardHandler();
	CefKeyboardHandler->UrKeyboardHandler = TScriptInterface<IUrKeyboardHandler>(this);
}

void UUrDefaultKeyboardHandlerStatics::CreateDefaultCefKeyboardHandler(TScriptInterface<IUrKeyboardHandler> handler)
{
	handler->CefKeyboardHandler = new FCefDefaultKeyboardHandler();
	handler->CefKeyboardHandler->UrKeyboardHandler = handler;
}