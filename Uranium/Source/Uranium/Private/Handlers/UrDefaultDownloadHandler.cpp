// 

#include "Handlers//UrDefaultDownloadHandler.h"

void FCefDefaultDownloadHandler::OnBeforeDownload(
	CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefDownloadItem> downloadItem,
	const CefString& suggestedName,
	CefRefPtr<CefBeforeDownloadCallback> callback
) {
	// TODO: expose downloading functionality to UE4 interfaces
}

void FCefDefaultDownloadHandler::OnDownloadUpdated(
	CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefDownloadItem> downloadItem,
	CefRefPtr<CefDownloadItemCallback> callback
) {
	// TODO: expose downloading functionality to UE4 interfaces
	callback->Cancel();
}

void UUrDefaultDownloadHandler::PostInitProperties()
{
	Super::PostInitProperties();
	CefDownloadHandler = new FCefDefaultDownloadHandler();
	CefDownloadHandler->UrDownloadHandler = TScriptInterface<IUrDownloadHandler>(this);
}

void UUrDefaultDownloadHandlerStatics::CreateDefaultCefDownloadHandler(TScriptInterface<IUrDownloadHandler> handler)
{
	handler->CefDownloadHandler = new FCefDefaultDownloadHandler();
	handler->CefDownloadHandler->UrDownloadHandler = handler;
}
