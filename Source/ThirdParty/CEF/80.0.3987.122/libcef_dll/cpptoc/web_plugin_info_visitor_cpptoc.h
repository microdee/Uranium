// Copyright (c) 2020 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.
//
// ---------------------------------------------------------------------------
//
// This file was generated by the CEF translator tool. If making changes by
// hand only do so within the body of existing method and function
// implementations. See the translator.README.txt file in the tools directory
// for more information.
//
// $hash=45695bea6b9ce6484f1321fd84bf11521d732b2c$
//

#ifndef CEF_LIBCEF_DLL_CPPTOC_WEB_PLUGIN_INFO_VISITOR_CPPTOC_H_
#define CEF_LIBCEF_DLL_CPPTOC_WEB_PLUGIN_INFO_VISITOR_CPPTOC_H_
#pragma once

#if !defined(WRAPPING_CEF_SHARED)
#error This file can be included wrapper-side only
#endif

#include "include/capi/cef_browser_capi.h"
#include "include/capi/cef_web_plugin_capi.h"
#include "include/cef_browser.h"
#include "include/cef_web_plugin.h"
#include "libcef_dll/cpptoc/cpptoc_ref_counted.h"

// Wrap a C++ class with a C structure.
// This class may be instantiated and accessed wrapper-side only.
class CefWebPluginInfoVisitorCppToC
    : public CefCppToCRefCounted<CefWebPluginInfoVisitorCppToC,
                                 CefWebPluginInfoVisitor,
                                 cef_web_plugin_info_visitor_t> {
 public:
  CefWebPluginInfoVisitorCppToC();
  virtual ~CefWebPluginInfoVisitorCppToC();
};

#endif  // CEF_LIBCEF_DLL_CPPTOC_WEB_PLUGIN_INFO_VISITOR_CPPTOC_H_
