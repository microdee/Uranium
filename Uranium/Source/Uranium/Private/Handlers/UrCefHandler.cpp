/******************** (C) Copyright 2021 realworld one GmbH & Co. KG All Rights Reserved. ********************/


#include "Handlers/UrCefHandler.h"

void IUrHandler::SetBrowserView(TBadge<UUrBrowserView> badge, UUrBrowserView* view)
{
	BrowserView = view;
	Execute_OnPersistentBrowserViewSet(_getUObject());
}

void IUrHandler::SetBrowser(TBadge<UUrBrowserView> badge, TCefPtr<CefBrowser> browser)
{
	Browser = browser;
	Execute_OnPersistentBrowserSet(_getUObject());
}
