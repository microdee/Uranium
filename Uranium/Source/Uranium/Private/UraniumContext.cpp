/******************** (C) Copyright 2021 realworld one GmbH & Co. KG All Rights Reserved. ********************/


#include "UraniumContext.h"

#include "CefIncludesStart.h"

#include "include/cef_base.h"
#include "include/cef_app.h"
#include "include/cef_browser.h"
#include "include/cef_client.h"
#include "include/cef_version.h"

#include "CefIncludesEnd.h"

FString IUraniumContext::GetCefVersion()
{
	return TEXT(CEF_VERSION);
}

FString IUraniumContext::GetChromiumVersion()
{
	std::wstringstream ver;
	ver << CHROME_VERSION_MAJOR << L"."
		<< CHROME_VERSION_MINOR << L"."
		<< CHROME_VERSION_BUILD << L"."
		<< CHROME_VERSION_PATCH;

	return FString(ver.str().c_str());
}
