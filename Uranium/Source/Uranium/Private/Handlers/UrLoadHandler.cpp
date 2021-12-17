// 

#include "Handlers/UrLoadHandler.h"

#include "Handlers/UrHandlerEvents.h"
#include "Uranium.h"

UUrLoadEvents* ICefLoadHandler::GetLoadEvents()
{
	return UUrLoadEvents::GetEvents(UrLoadHandler.ToScriptInterface());
}

void ICefLoadHandler::OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, TransitionType transitionType)
{
	uint8 trType = transitionType & TT_SOURCE_MASK;
	CEF_REDIRECT_EVENT(Load, OnLoadStart, (static_cast<EUrTransitionType>(trType)));
}

void ICefLoadHandler::OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode)
{
	CEF_REDIRECT_EVENT(Load, OnLoadEnd, (httpStatusCode));
}

void ICefLoadHandler::OnLoadError(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, ErrorCode errorCode, const CefString& errorText, const CefString& failedUrl)
{
	FUrCefErrorCode urError(errorCode);
	FString failedUrlStr(failedUrl.c_str());
	CEF_REDIRECT_EVENT(Load, OnLoadError, (urError, failedUrlStr));
}

void ICefLoadHandler::OnLoadingStateChange(CefRefPtr<CefBrowser> browser, bool isLoading, bool canGoBack, bool canGoForward)
{
	CEF_REDIRECT_EVENT(Load, OnLoadingStateChange, (isLoading, canGoBack, canGoForward));
}

UUrLoadEvents* UUrLoadEvents::GetEvents(TScriptInterface<IUrLoadHandler> handler)
{
	auto subSys = GEngine->GetEngineSubsystem<UUrHandlerEvents>();
	return Cast<UUrLoadEvents>(subSys->GetEvents(handler.GetObject(), StaticClass()));
}
