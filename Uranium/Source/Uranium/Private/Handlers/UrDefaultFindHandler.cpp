// 

#include "Handlers//UrDefaultFindHandler.h"

void UUrDefaultFindHandler::PostInitProperties()
{
	Super::PostInitProperties();
	CefFindHandler = new FCefDefaultFindHandler();
	CefFindHandler->UrFindHandler = TScriptInterface<IUrFindHandler>(this);
}

void UUrDefaultFindHandlerStatics::CreateDefaultCefFindHandler(TScriptInterface<IUrFindHandler> handler)
{
	handler->CefFindHandler = new FCefDefaultFindHandler();
	handler->CefFindHandler->UrFindHandler = handler;
}