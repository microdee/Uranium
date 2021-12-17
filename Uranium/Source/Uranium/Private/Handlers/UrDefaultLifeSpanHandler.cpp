// 

#include "Handlers//UrDefaultLifeSpanHandler.h"

#include "UrBrowserView.h"

void UUrDefaultLifeSpanHandler::PostInitProperties()
{
	Super::PostInitProperties();
	CefLifeSpanHandler = new FCefDefaultLifeSpanHandler();
	CefLifeSpanHandler->UrLifeSpanHandler = TScriptInterface<IUrLifeSpanHandler>(this);
}

EUrNewWindowBehavior UUrDefaultLifeSpanHandler::GetNewWindowBehavior_Implementation(UUrBrowserView* browser, FNewBrowserMetadata newWindowArguments)
{
	return NewWindowBehavior;
}

void UUrDefaultLifeSpanHandlerStatics::CreateDefaultCefLifeSpanHandler(TScriptInterface<IUrLifeSpanHandler> handler)
{
	handler->CefLifeSpanHandler = new FCefDefaultLifeSpanHandler();
	handler->CefLifeSpanHandler->UrLifeSpanHandler = handler;
}