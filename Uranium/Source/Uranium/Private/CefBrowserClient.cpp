/******************** (C) Copyright 2021 realworld one GmbH & Co. KG All Rights Reserved. ********************/


#include "CefBrowserClient.h"
#include "Handlers/UrAudioHandler.h"
#include "Handlers/UrContextMenuHandler.h"
#include "Handlers/UrDialogHandler.h"
#include "Handlers/UrDisplayHandler.h"
#include "Handlers/UrDownloadHandler.h"
#include "Handlers/UrDragHandler.h"
#include "Handlers/UrFindHandler.h"
#include "Handlers/UrFocusHandler.h"
#include "Handlers/UrJSDialogHandler.h"
#include "Handlers/UrKeyboardHandler.h"
#include "Handlers/UrLifeSpanHandler.h"
#include "Handlers/UrLoadHandler.h"
#include "Handlers/UrRenderHandler.h"
#include "Handlers/UrRequestHandler.h"

FCefBrowserClient::FCefBrowserClient(FCefHandlers handlers, FOnProcessMessageReceived onProcessMessageReceived)
	: Handlers(handlers)
	, OnProcessMessageReceivedDel(onProcessMessageReceived)
{
}

CefRefPtr<CefAudioHandler> FCefBrowserClient::GetAudioHandler()
{
	return Handlers.AudioHandler.Get();
}

CefRefPtr<CefContextMenuHandler> FCefBrowserClient::GetContextMenuHandler()
{
	return Handlers.ContextMenuHandler.Get();
}

CefRefPtr<CefDialogHandler> FCefBrowserClient::GetDialogHandler()
{
	return Handlers.DialogHandler.Get();
}

CefRefPtr<CefDisplayHandler> FCefBrowserClient::GetDisplayHandler()
{
	return Handlers.DisplayHandler.Get();
}

CefRefPtr<CefDownloadHandler> FCefBrowserClient::GetDownloadHandler()
{
	return Handlers.DownloadHandler.Get();
}

CefRefPtr<CefDragHandler> FCefBrowserClient::GetDragHandler()
{
	return Handlers.DragHandler.Get();
}

CefRefPtr<CefFindHandler> FCefBrowserClient::GetFindHandler()
{
	return Handlers.FindHandler.Get();
}

CefRefPtr<CefFocusHandler> FCefBrowserClient::GetFocusHandler()
{
	return Handlers.FocusHandler.Get();
}

CefRefPtr<CefJSDialogHandler> FCefBrowserClient::GetJSDialogHandler()
{
	return Handlers.JSDialogHandler.Get();
}

CefRefPtr<CefKeyboardHandler> FCefBrowserClient::GetKeyboardHandler()
{
	return Handlers.KeyboardHandler.Get();
}

CefRefPtr<CefLifeSpanHandler> FCefBrowserClient::GetLifeSpanHandler()
{
	return Handlers.LifeSpanHandler.Get();
}

CefRefPtr<CefLoadHandler> FCefBrowserClient::GetLoadHandler()
{
	return Handlers.LoadHandler.Get();
}

CefRefPtr<CefRenderHandler> FCefBrowserClient::GetRenderHandler()
{
	return Handlers.RenderHandler.Get();
}

CefRefPtr<CefRequestHandler> FCefBrowserClient::GetRequestHandler()
{
	return Handlers.RequestHandler.Get();
}

bool FCefBrowserClient::OnProcessMessageReceived(
	CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	CefProcessId sourceProcess,
	CefRefPtr<CefProcessMessage> message
) {
	if(OnProcessMessageReceivedDel.IsBound())
	{
		return OnProcessMessageReceivedDel.Execute(AsCefPtr(browser), AsCefPtr(frame), sourceProcess, AsCefPtr(message));
	}
	return false;
}
