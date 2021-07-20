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
// $hash=fdcd22125bd1d234bb0948e70b9605ac448167d5$
//

#include "libcef_dll/ctocpp/resource_skip_callback_ctocpp.h"
#include "libcef_dll/shutdown_checker.h"

// VIRTUAL METHODS - Body may be edited by hand.

NO_SANITIZE("cfi-icall")
void CefResourceSkipCallbackCToCpp::Continue(int64 bytes_skipped) {
  shutdown_checker::AssertNotShutdown();

  cef_resource_skip_callback_t* _struct = GetStruct();
  if (CEF_MEMBER_MISSING(_struct, cont))
    return;

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  _struct->cont(_struct, bytes_skipped);
}

// CONSTRUCTOR - Do not edit by hand.

CefResourceSkipCallbackCToCpp::CefResourceSkipCallbackCToCpp() {}

// DESTRUCTOR - Do not edit by hand.

CefResourceSkipCallbackCToCpp::~CefResourceSkipCallbackCToCpp() {
  shutdown_checker::AssertNotShutdown();
}

template <>
cef_resource_skip_callback_t* CefCToCppRefCounted<
    CefResourceSkipCallbackCToCpp,
    CefResourceSkipCallback,
    cef_resource_skip_callback_t>::UnwrapDerived(CefWrapperType type,
                                                 CefResourceSkipCallback* c) {
  NOTREACHED() << "Unexpected class type: " << type;
  return NULL;
}

template <>
CefWrapperType CefCToCppRefCounted<CefResourceSkipCallbackCToCpp,
                                   CefResourceSkipCallback,
                                   cef_resource_skip_callback_t>::kWrapperType =
    WT_RESOURCE_SKIP_CALLBACK;
