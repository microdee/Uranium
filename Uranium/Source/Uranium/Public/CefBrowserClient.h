/******************** (C) Copyright 2021 realworld one GmbH & Co. KG All Rights Reserved. ********************/

#pragma once

#include "CoreMinimal.h"

#include "CefIncludesStart.h"
#include "include/cef_client.h"
#include "CefIncludesEnd.h"
#include "CefPtr.h"

/**
 * Called when a new message is received from a different process. Return true
 * if the message was handled or false otherwise. Do not keep a reference to
 * or attempt to access the message outside of this callback.
 */
DECLARE_DELEGATE_RetVal_FourParams(
	bool, FOnProcessMessageReceived,
	TCefPtr<CefBrowser>,
	TCefPtr<CefFrame>,
	CefProcessId,
	TCefPtr<CefProcessMessage>
);

struct URANIUM_API FCefHandlers
{
	TCefPtr<class ICefAudioHandler> AudioHandler;
	TCefPtr<class ICefContextMenuHandler> ContextMenuHandler;
	TCefPtr<class ICefDialogHandler> DialogHandler;
	TCefPtr<class ICefDisplayHandler> DisplayHandler;
	TCefPtr<class ICefDownloadHandler> DownloadHandler;
	TCefPtr<class ICefDragHandler> DragHandler;
	TCefPtr<class ICefFindHandler> FindHandler;
	TCefPtr<class ICefFocusHandler> FocusHandler;
	TCefPtr<class ICefJSDialogHandler> JSDialogHandler;
	TCefPtr<class ICefKeyboardHandler> KeyboardHandler;
	TCefPtr<class ICefLifeSpanHandler> LifeSpanHandler;
	TCefPtr<class ICefLoadHandler> LoadHandler;
	TCefPtr<class ICefRenderHandler> RenderHandler;
	TCefPtr<class ICefRequestHandler> RequestHandler;
};

/**
 * Native client for CEF browsers which composes together individual handlers
 * This class requires handlers to be created before construction and it's merely just a bridge for
 * CEF to register input handlers
 */
class URANIUM_API FCefBrowserClient : public CefClient
{
protected:
	FCefHandlers Handlers;
	FOnProcessMessageReceived OnProcessMessageReceivedDel;
	
public:
	FCefBrowserClient(FCefHandlers handlers, FOnProcessMessageReceived onProcessMessageReceived);

	virtual CefRefPtr<CefAudioHandler> GetAudioHandler() override;
	virtual CefRefPtr<CefContextMenuHandler> GetContextMenuHandler() override;
	virtual CefRefPtr<CefDialogHandler> GetDialogHandler() override;
	virtual CefRefPtr<CefDisplayHandler> GetDisplayHandler() override;
	virtual CefRefPtr<CefDownloadHandler> GetDownloadHandler() override;
	virtual CefRefPtr<CefDragHandler> GetDragHandler() override;
	virtual CefRefPtr<CefFindHandler> GetFindHandler() override;
	virtual CefRefPtr<CefFocusHandler> GetFocusHandler() override;
	virtual CefRefPtr<CefJSDialogHandler> GetJSDialogHandler() override;
	virtual CefRefPtr<CefKeyboardHandler> GetKeyboardHandler() override;
	virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override;
	virtual CefRefPtr<CefLoadHandler> GetLoadHandler() override;
	virtual CefRefPtr<CefRenderHandler> GetRenderHandler() override;
	virtual CefRefPtr<CefRequestHandler> GetRequestHandler() override;

	virtual bool OnProcessMessageReceived(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefProcessId sourceProcess,
		CefRefPtr<CefProcessMessage> message
	) override;
	
	IMPLEMENT_REFCOUNTING(FCefBrowserClient);
};