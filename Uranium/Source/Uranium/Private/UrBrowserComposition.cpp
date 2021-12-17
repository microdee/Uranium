/******************** (C) Copyright 2021 realworld one GmbH & Co. KG All Rights Reserved. ********************/


#include "UrBrowserComposition.h"

#include "UrBrowserView.h"
#include "Handlers/UrDefaultAudioHandler.h"
#include "Handlers/UrDefaultContextMenuHandler.h"
#include "Handlers/UrDefaultDialogHandler.h"
#include "Handlers/UrDefaultDisplayHandler.h"
#include "Handlers/UrDefaultDownloadHandler.h"
#include "Handlers/UrDefaultDragHandler.h"
#include "Handlers/UrDefaultFindHandler.h"
#include "Handlers/UrDefaultFocusHandler.h"
#include "Handlers/UrDefaultJSDialogHandler.h"
#include "Handlers/UrDefaultKeyboardHandler.h"
#include "Handlers/UrDefaultLifeSpanHandler.h"
#include "Handlers/UrDefaultLoadHandler.h"
#include "Handlers/UrDefaultRenderHandler.h"
#include "Handlers/UrDefaultRequestHandler.h"
#include "Handlers/UrWindowedDevToolsHandler.h"
#include "Handlers/SlowRenderResourceHandler.h"

UUrBrowserComposition::UUrBrowserComposition(const FObjectInitializer& oi) : Super(oi)
{
	BrowserView = UUrBrowserView::StaticClass();
	
	AudioHandler = UUrDefaultAudioHandler::StaticClass();
	ContextMenuHandler = UUrDefaultContextMenuHandler::StaticClass();
	DialogHandler = UUrDefaultDialogHandler::StaticClass();
	DisplayHandler = UUrDefaultDisplayHandler::StaticClass();
	DownloadHandler = UUrDefaultDownloadHandler::StaticClass();
	DragHandler = UUrDefaultDragHandler::StaticClass();
	FindHandler = UUrDefaultFindHandler::StaticClass();
	FocusHandler = UUrDefaultFocusHandler::StaticClass();
	JSDialogHandler = UUrDefaultJSDialogHandler::StaticClass();
	KeyboardHandler = UUrDefaultKeyboardHandler::StaticClass();
	LifeSpanHandler = UUrDefaultLifeSpanHandler::StaticClass();
	LoadHandler = UUrDefaultLoadHandler::StaticClass();
	
#if UR_WITH_SHARED_TEXTURE
	RenderResourceHandler = UUrDefaultRenderResourceHandler::StaticClass();
#else
	RenderResourceHandler = USlowRenderResourceHandler::StaticClass();
#endif
	
	RenderEventsHandler = UUrDefaultRenderEventsHandler::StaticClass();
	RequestHandler = UUrDefaultRequestHandler::StaticClass();
	DevToolsHandler = UUrWindowedDevToolsHandler::StaticClass();
}
