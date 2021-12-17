// 

#include "Handlers/UrRenderHandler.h"

#include "UrBrowserView.h"
#include "Handlers/UrHandlerEvents.h"
#include "Uranium.h"

UUrRenderEvents* ICefRenderHandler::GetRenderEvents()
{
	return UUrRenderEvents::GetEvents(UrRenderHandler.ToScriptInterface());
}

bool ICefRenderHandler::GetRootScreenRect(CefRefPtr<CefBrowser> browser, CefRect& rect)
{
	// If this method returns false the rectangle from GetViewRect will be used.
	// This might stay here because it's simpler to not care about "screens" in whatever custom wild situation Uranium is used
	return false;
}

bool ICefRenderHandler::GetScreenInfo(CefRefPtr<CefBrowser> browser, CefScreenInfo& screenInfo)
{
	if(!UrRenderResourceHandler.IsValid())
	{
		return false;
	}

	FVector2D mainImageSize = UrRenderResourceHandler->GetMainSize();
	
	screenInfo.available_rect =
	{
		0, 0,
		static_cast<int>(mainImageSize.X),
		static_cast<int>(mainImageSize.Y)
	};

	// Return true if the |screen_info| structure has been modified.
	// If the screen info rectangle is left empty the rectangle from GetViewRect will be used.
	// If the rectangle is still empty or invalid popups may not be drawn correctly.
	return true;
}

void ICefRenderHandler::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect)
{
	if(!UrRenderResourceHandler.IsValid())
	{
		return;
	}
	
	FVector2D mainImageSize = UrRenderResourceHandler->GetMainSize();
	rect.Set(
		0, 0,
		static_cast<int>(mainImageSize.X),
		static_cast<int>(mainImageSize.Y)
	);
}

void ICefRenderHandler::OnPaint(
	CefRefPtr<CefBrowser> browser,
	PaintElementType type,
	const RectList& dirtyRects,
	const void* buffer,
	int width, int height
) {
	if(UrRenderResourceHandler.IsValid())
	{
		UrRenderResourceHandler->OnPaint(browser, buffer, dirtyRects, width, height, type == PET_POPUP);
	}
}

void ICefRenderHandler::OnAcceleratedPaint(
	CefRefPtr<CefBrowser> browser,
	PaintElementType type,
	const RectList& dirtyRects,
	void* shared_handle
) {
	if(UrRenderResourceHandler.IsValid())
	{
		UrRenderResourceHandler->OnAcceleratedPaint(browser, shared_handle, dirtyRects, type == PET_POPUP);
	}
}

void ICefRenderHandler::OnPopupShow(CefRefPtr<CefBrowser> browser, bool show)
{
	InvokeLatentWithBrowser(UrRenderHandler, [=](UUrBrowserView* view)
	{
		CEF_REDIRECT_EVENT_BODY_I(
			RenderEventsHandler, RenderHandler, RenderEvents,
			OnNativePopupShow, (show)
		)
	});
}

void ICefRenderHandler::OnPopupSize(CefRefPtr<CefBrowser> browser, const CefRect& rect)
{
	FVector2D location(rect.x, rect.y), size(rect.width, rect.height);
	InvokeLatentWithBrowser(UrRenderHandler, [=](UUrBrowserView* view)
	{
		CEF_REDIRECT_EVENT_BODY_I(
			RenderEventsHandler, RenderHandler, RenderEvents,
			OnNativePopupSize, (location, size)
		)
	});
}

void ICefRenderHandler::OnScrollOffsetChanged(CefRefPtr<CefBrowser> browser, double x, double y)
{
	FVector2D offset(x, y);
	InvokeLatentWithBrowser(UrRenderHandler, [=](UUrBrowserView* view)
	{
		CEF_REDIRECT_EVENT_BODY_I(
			RenderEventsHandler, RenderHandler, RenderEvents,
			OnScrollOffsetChanged, (offset)
		)
	});
}

void ICefRenderHandler::OnTextSelectionChanged(CefRefPtr<CefBrowser> browser, const CefString& selectedText, const CefRange& selectedRange)
{
	FString textStr(selectedText.c_str());
	InvokeLatentWithBrowser(UrRenderHandler, [=](UUrBrowserView* view)
	{
		CEF_REDIRECT_EVENT_BODY_I(
			RenderEventsHandler, RenderHandler, RenderEvents,
			OnTextSelectionChanged, (textStr, selectedRange.from, selectedRange.to)
		)
	});
}

void ICefRenderHandler::OnVirtualKeyboardRequested(CefRefPtr<CefBrowser> browser, TextInputMode inputMode)
{
	InvokeLatentWithBrowser(UrRenderHandler, [=](UUrBrowserView* view)
	{
		CEF_REDIRECT_EVENT_BODY_I(
			RenderEventsHandler, RenderHandler, RenderEvents,
			OnVirtualKeyboardRequested, (static_cast<EUrTextInputMode>(inputMode))
		)
	});
}

void IUrRenderEventsHandler::AssignCefRenderHandler(CefRefPtr<ICefRenderHandler> cefHandler)
{
	check(cefHandler);
	
	CefRenderResourceHandler = cefHandler;
	CefRenderResourceHandler->UrRenderHandler = TScriptInterface<IUrRenderEventsHandler>(_getUObject());
}

UUrRenderEvents* UUrRenderEvents::GetEvents(TScriptInterface<IUrRenderEventsHandler> handler)
{
	auto subSys = GEngine->GetEngineSubsystem<UUrHandlerEvents>();
	return Cast<UUrRenderEvents>(subSys->GetEvents(handler.GetObject(), StaticClass()));
}