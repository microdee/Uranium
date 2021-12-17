// 

#include "Handlers//UrDefaultRequestHandler.h"

void UUrDefaultRequestHandler::PostInitProperties()
{
	Super::PostInitProperties();
	CefRequestHandler = new FCefDefaultRequestHandler();
	CefRequestHandler->UrRequestHandler = TScriptInterface<IUrRequestHandler>(this);
}

void UUrDefaultRequestHandlerStatics::CreateDefaultCefRequestHandler(TScriptInterface<IUrRequestHandler> handler)
{
	handler->CefRequestHandler = new FCefDefaultRequestHandler();
	handler->CefRequestHandler->UrRequestHandler = handler;
}