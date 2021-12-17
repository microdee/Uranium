// 

#include "Handlers//UrDefaultDragHandler.h"

void UUrDefaultDragHandler::PostInitProperties()
{
	Super::PostInitProperties();
	CefDragHandler = new FCefDefaultDragHandler();
	CefDragHandler->UrDragHandler = TScriptInterface<IUrDragHandler>(this);
}

void UUrDefaultDragHandlerStatics::CreateDefaultCefDragHandler(TScriptInterface<IUrDragHandler> handler)
{
	handler->CefDragHandler = new FCefDefaultDragHandler();
	handler->CefDragHandler->UrDragHandler = handler;
}