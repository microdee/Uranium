// 

#include "Handlers//UrDefaultLoadHandler.h"

void UUrDefaultLoadHandler::PostInitProperties()
{
	Super::PostInitProperties();
	CefLoadHandler = new FCefDefaultLoadHandler();
	CefLoadHandler->UrLoadHandler = TScriptInterface<IUrLoadHandler>(this);
}

void UUrDefaultLoadHandlerStatics::CreateDefaultCefLoadHandler(TScriptInterface<IUrLoadHandler> handler)
{
	handler->CefLoadHandler = new FCefDefaultLoadHandler();
	handler->CefLoadHandler->UrLoadHandler = handler;
}