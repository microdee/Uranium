// 

#include "Handlers/UrDisplayHandler.h"

#include "Handlers/UrHandlerEvents.h"
#include "Uranium.h"

UUrDisplayEvents* ICefDisplayHandler::GetDisplayEvents()
{
	return UUrDisplayEvents::GetEvents(UrDisplayHandler.ToScriptInterface());
}

void ICefDisplayHandler::OnAddressChange(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url)
{
	FString address(url.c_str());
	CEF_REDIRECT_EVENT(Display, OnAddressChange, (address));
}

bool ICefDisplayHandler::OnAutoResize(CefRefPtr<CefBrowser> browser, const CefSize& newSize)
{
	FVector2D newSizeVec =
	{
		static_cast<float>(newSize.width),
		static_cast<float>(newSize.height)
	};
	return CEF_REDIRECT_EVENT_RETURN(Display, OnAutoResize, (newSizeVec), true);
}

bool ICefDisplayHandler::OnConsoleMessage(CefRefPtr<CefBrowser> browser, cef_log_severity_t level, const CefString& message, const CefString& source, int line)
{
	UE_LOG(LogUranium, Display, TEXT("(%s %i) %s"), source.c_str(), line, message.c_str());
	
	FString messageStr(message.c_str());
	FString sourceStr(source.c_str());

	CEF_REDIRECT_EVENT(Display, OnConsoleMessage, (static_cast<EUrLogSeverity>(level), messageStr, sourceStr, line));
	return false;
}

void ICefDisplayHandler::OnFullscreenModeChange(CefRefPtr<CefBrowser> browser, bool fullscreen)
{
	CEF_REDIRECT_EVENT(Display, OnFullscreenModeChange, (fullscreen));
}

void ICefDisplayHandler::OnLoadingProgressChange(CefRefPtr<CefBrowser> browser, double progress)
{
	CEF_REDIRECT_EVENT(Display, OnLoadingProgressChange, (progress));
}

void ICefDisplayHandler::OnStatusMessage(CefRefPtr<CefBrowser> browser, const CefString& value)
{
	FString text(value.c_str());
	CEF_REDIRECT_EVENT(Display, OnStatusMessage, (text));
}

void ICefDisplayHandler::OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title)
{
	FString text(title.c_str());
	CEF_REDIRECT_EVENT(Display, OnTitleChange, (text));
}

bool ICefDisplayHandler::OnTooltip(CefRefPtr<CefBrowser> browser, CefString& text)
{
	FString textStr(text.c_str());
	CEF_REDIRECT_EVENT(Display, OnTooltip, (textStr));
	return false;
}

UUrDisplayEvents* UUrDisplayEvents::GetEvents(TScriptInterface<IUrDisplayHandler> handler)
{
	auto subSys = GEngine->GetEngineSubsystem<UUrHandlerEvents>();
	return Cast<UUrDisplayEvents>(subSys->GetEvents(handler.GetObject(), StaticClass()));
}
