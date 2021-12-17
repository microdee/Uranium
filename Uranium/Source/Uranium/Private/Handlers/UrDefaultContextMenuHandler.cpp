// 

#include "Handlers//UrDefaultContextMenuHandler.h"

void FCefDefaultContextMenuHandler::OnBeforeContextMenu(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefContextMenuParams> params, CefRefPtr<CefMenuModel> model)
{
	// by default just clear
	// TODO: expose context menu stuff to unreal
	model->Clear();
}

void UUrDefaultContextMenuHandler::PostInitProperties()
{
	Super::PostInitProperties();
	CefContextMenuHandler = new FCefDefaultContextMenuHandler();
	CefContextMenuHandler->UrContextMenuHandler = TScriptInterface<IUrContextMenuHandler>(this);
}

void UUrDefaultContextMenuHandlerStatics::CreateDefaultCefContextMenuHandler(TScriptInterface<IUrContextMenuHandler> handler)
{
	handler->CefContextMenuHandler = new FCefDefaultContextMenuHandler();
	handler->CefContextMenuHandler->UrContextMenuHandler = handler;
}
