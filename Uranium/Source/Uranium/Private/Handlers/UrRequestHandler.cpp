// 

#include "Handlers/UrRequestHandler.h"
#include "Handlers/UrHandlerEvents.h"
#include "Uranium.h"

UUrRequestEvents* ICefRequestHandler::GetRequestEvents()
{
	return UUrRequestEvents::GetEvents(UrRequestHandler.ToScriptInterface());
}

bool ICefRequestHandler::OnBeforeBrowse(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, bool userGesture, bool isRedirect)
{
	// TODO: map CefRequest to UE4 IHttpRequest
	FString url = request->GetURL().c_str();
	return CEF_REDIRECT_EVENT_RETURN(Request, OnBeforeBrowse, (url, userGesture, isRedirect), false);
}

void ICefRequestHandler::OnPluginCrashed(CefRefPtr<CefBrowser> browser, const CefString& pluginPath)
{
	UE_LOG(LogUranium, Display, TEXT("[CEF] Plugin crashed: %s"), pluginPath.c_str());
}

void ICefRequestHandler::OnRenderProcessTerminated(CefRefPtr<CefBrowser> browser, TerminationStatus status)
{
	UE_LOG(LogUranium, Display, TEXT("[CEF] RenderProcess terminated with reason: %i"), status);
}

void ICefRequestHandler::OnRenderViewReady(CefRefPtr<CefBrowser> browser)
{
	// TODO: this might become important in JS interop
}

CefRefPtr<CefResourceRequestHandler> ICefRequestHandler::GetResourceRequestHandler(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, bool isNavigation, bool isDownload, const CefString& requestInitiator, bool& disableDefaultHandling)
{
	// TODO implement custom html loading with CefResourceRequestHandlers and CefResourceHandlers
	return nullptr;
}

UUrRequestEvents* UUrRequestEvents::GetEvents(TScriptInterface<IUrRequestHandler> handler)
{
	auto subSys = GEngine->GetEngineSubsystem<UUrHandlerEvents>();
	return Cast<UUrRequestEvents>(subSys->GetEvents(handler.GetObject(), StaticClass()));
}
