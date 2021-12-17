/******************** (C) Copyright 2021 realworld one GmbH & Co. KG All Rights Reserved. ********************/
#pragma once

#include "CefIncludesStart.h"
#include "include/cef_app.h"
#include "CefIncludesEnd.h"

#define CEFV8_UTILS_MAX_RECLEVEL 100

class CefV8Utils
{
public:

	static CefRefPtr<CefV8Value> to_v8object(
		CefRefPtr<CefDictionaryValue> const& dictionary,
		int reclevel = 0
	);

	static CefRefPtr<CefV8Value> to_v8array(
		CefRefPtr<CefListValue> const& list,
		int reclevel = 0
	);

	static CefRefPtr<CefDictionaryValue> to_dictionary(
		CefRefPtr<CefV8Value> const& obj,
		int reclevel = 0
	);

	static CefRefPtr<CefListValue> to_list(
		CefRefPtr<CefV8Value> const& arr,
		int reclevel = 0
	);
};
