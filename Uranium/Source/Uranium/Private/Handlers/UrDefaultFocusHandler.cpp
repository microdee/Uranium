// 

#include "Handlers//UrDefaultFocusHandler.h"

void UUrDefaultFocusHandler::PostInitProperties()
{
	Super::PostInitProperties();
	CefFocusHandler = new FCefDefaultFocusHandler();
	CefFocusHandler->UrFocusHandler = TScriptInterface<IUrFocusHandler>(this);
}

void UUrDefaultFocusHandlerStatics::CreateDefaultCefFocusHandler(TScriptInterface<IUrFocusHandler> handler)
{
	handler->CefFocusHandler = new FCefDefaultFocusHandler();
	handler->CefFocusHandler->UrFocusHandler = handler;
}