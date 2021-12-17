// 

#include "Handlers//UrDefaultJSDialogHandler.h"

bool FCefDefaultJSDialogHandler::OnJSDialog(CefRefPtr<CefBrowser> browser, const CefString& originUrl, JSDialogType dialogType, const CefString& messageText, const CefString& defaultPromptText, CefRefPtr<CefJSDialogCallback> callback, bool& suppressMessage)
{
	// TODO: handle javascript dialogs in UE4
	suppressMessage = true;
	return false;
}

void UUrDefaultJSDialogHandler::PostInitProperties()
{
	Super::PostInitProperties();
	CefJSDialogHandler = new FCefDefaultJSDialogHandler();
	CefJSDialogHandler->UrJSDialogHandler = TScriptInterface<IUrJSDialogHandler>(this);
}

void UUrDefaultJSDialogHandlerStatics::CreateDefaultCefJSDialogHandler(TScriptInterface<IUrJSDialogHandler> handler)
{
	handler->CefJSDialogHandler = new FCefDefaultJSDialogHandler();
	handler->CefJSDialogHandler->UrJSDialogHandler = handler;
}