// Copyright (c) 2019 The Chromium Embedded Framework Authors. All rights
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
// $hash=904d9a64d7afbfb3adfea9069c6b5340d511a08f$
//

#ifndef CEF_LIBCEF_DLL_CTOCPP_EXTENSION_CTOCPP_H_
#define CEF_LIBCEF_DLL_CTOCPP_EXTENSION_CTOCPP_H_
#pragma once

#if !defined(WRAPPING_CEF_SHARED)
#error This file can be included wrapper-side only
#endif

#include "include/capi/cef_extension_capi.h"
#include "include/capi/cef_extension_handler_capi.h"
#include "include/capi/cef_request_context_capi.h"
#include "include/cef_extension.h"
#include "include/cef_extension_handler.h"
#include "include/cef_request_context.h"
#include "libcef_dll/ctocpp/ctocpp_ref_counted.h"

// Wrap a C structure with a C++ class.
// This class may be instantiated and accessed wrapper-side only.
class CefExtensionCToCpp : public CefCToCppRefCounted<CefExtensionCToCpp,
                                                      CefExtension,
                                                      cef_extension_t> {
 public:
  CefExtensionCToCpp();

  // CefExtension methods.
  CefString GetIdentifier() OVERRIDE;
  CefString GetPath() OVERRIDE;
  CefRefPtr<CefDictionaryValue> GetManifest() OVERRIDE;
  bool IsSame(CefRefPtr<CefExtension> that) OVERRIDE;
  CefRefPtr<CefExtensionHandler> GetHandler() OVERRIDE;
  CefRefPtr<CefRequestContext> GetLoaderContext() OVERRIDE;
  bool IsLoaded() OVERRIDE;
  void Unload() OVERRIDE;
};

#endif  // CEF_LIBCEF_DLL_CTOCPP_EXTENSION_CTOCPP_H_
