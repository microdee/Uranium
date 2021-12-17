// 

#include "Handlers//UrDefaultDialogHandler.h"

bool FCefDefaultDialogHandler::OnFileDialog(CefRefPtr<CefBrowser> browser, FileDialogMode mode, const CefString& title, const CefString& defaultFilePath, const std::vector<CefString>& acceptFilters, int selectedAcceptFilter, CefRefPtr<CefFileDialogCallback> callback)
{
	// TODO: handle file selection dialog
	callback->Cancel();
	return true;
}

void UUrDefaultDialogHandler::PostInitProperties()
{
	Super::PostInitProperties();
	CefDialogHandler = new FCefDefaultDialogHandler();
	CefDialogHandler->UrDialogHandler = TScriptInterface<IUrDialogHandler>(this);
}

void UUrDefaultDialogHandlerStatics::CreateDefaultCefDialogHandler(TScriptInterface<IUrDialogHandler> handler)
{
	handler->CefDialogHandler = new FCefDefaultDialogHandler();
	handler->CefDialogHandler->UrDialogHandler = handler;
}