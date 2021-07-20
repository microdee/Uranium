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
// $hash=4568fbb1f264fe9a900784aa3040c406a919ad37$
//

#include "libcef_dll/cpptoc/audio_handler_cpptoc.h"
#include "libcef_dll/ctocpp/browser_ctocpp.h"
#include "libcef_dll/shutdown_checker.h"

namespace {

// MEMBER FUNCTIONS - Body may be edited by hand.

int CEF_CALLBACK
audio_handler_get_audio_parameters(struct _cef_audio_handler_t* self,
                                   struct _cef_browser_t* browser,
                                   cef_audio_parameters_t* params) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return 0;
  // Verify param: browser; type: refptr_diff
  DCHECK(browser);
  if (!browser)
    return 0;
  // Verify param: params; type: simple_byref
  DCHECK(params);
  if (!params)
    return 0;

  // Translate param: params; type: simple_byref
  CefAudioParameters paramsVal = params ? *params : CefAudioParameters();

  // Execute
  bool _retval = CefAudioHandlerCppToC::Get(self)->GetAudioParameters(
      CefBrowserCToCpp::Wrap(browser), paramsVal);

  // Restore param: params; type: simple_byref
  if (params)
    *params = paramsVal;

  // Return type: bool
  return _retval;
}

void CEF_CALLBACK
audio_handler_on_audio_stream_started(struct _cef_audio_handler_t* self,
                                      struct _cef_browser_t* browser,
                                      const cef_audio_parameters_t* params,
                                      int channels) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return;
  // Verify param: browser; type: refptr_diff
  DCHECK(browser);
  if (!browser)
    return;
  // Verify param: params; type: simple_byref_const
  DCHECK(params);
  if (!params)
    return;

  // Translate param: params; type: simple_byref_const
  CefAudioParameters paramsVal = params ? *params : CefAudioParameters();

  // Execute
  CefAudioHandlerCppToC::Get(self)->OnAudioStreamStarted(
      CefBrowserCToCpp::Wrap(browser), paramsVal, channels);
}

void CEF_CALLBACK
audio_handler_on_audio_stream_packet(struct _cef_audio_handler_t* self,
                                     struct _cef_browser_t* browser,
                                     const float** data,
                                     int frames,
                                     int64 pts) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return;
  // Verify param: browser; type: refptr_diff
  DCHECK(browser);
  if (!browser)
    return;
  // Verify param: data; type: simple_byaddr
  DCHECK(data);
  if (!data)
    return;

  // Execute
  CefAudioHandlerCppToC::Get(self)->OnAudioStreamPacket(
      CefBrowserCToCpp::Wrap(browser), data, frames, pts);
}

void CEF_CALLBACK
audio_handler_on_audio_stream_stopped(struct _cef_audio_handler_t* self,
                                      struct _cef_browser_t* browser) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return;
  // Verify param: browser; type: refptr_diff
  DCHECK(browser);
  if (!browser)
    return;

  // Execute
  CefAudioHandlerCppToC::Get(self)->OnAudioStreamStopped(
      CefBrowserCToCpp::Wrap(browser));
}

void CEF_CALLBACK
audio_handler_on_audio_stream_error(struct _cef_audio_handler_t* self,
                                    struct _cef_browser_t* browser,
                                    const cef_string_t* message) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return;
  // Verify param: browser; type: refptr_diff
  DCHECK(browser);
  if (!browser)
    return;
  // Verify param: message; type: string_byref_const
  DCHECK(message);
  if (!message)
    return;

  // Execute
  CefAudioHandlerCppToC::Get(self)->OnAudioStreamError(
      CefBrowserCToCpp::Wrap(browser), CefString(message));
}

}  // namespace

// CONSTRUCTOR - Do not edit by hand.

CefAudioHandlerCppToC::CefAudioHandlerCppToC() {
  GetStruct()->get_audio_parameters = audio_handler_get_audio_parameters;
  GetStruct()->on_audio_stream_started = audio_handler_on_audio_stream_started;
  GetStruct()->on_audio_stream_packet = audio_handler_on_audio_stream_packet;
  GetStruct()->on_audio_stream_stopped = audio_handler_on_audio_stream_stopped;
  GetStruct()->on_audio_stream_error = audio_handler_on_audio_stream_error;
}

// DESTRUCTOR - Do not edit by hand.

CefAudioHandlerCppToC::~CefAudioHandlerCppToC() {
  shutdown_checker::AssertNotShutdown();
}

template <>
CefRefPtr<CefAudioHandler> CefCppToCRefCounted<
    CefAudioHandlerCppToC,
    CefAudioHandler,
    cef_audio_handler_t>::UnwrapDerived(CefWrapperType type,
                                        cef_audio_handler_t* s) {
  NOTREACHED() << "Unexpected class type: " << type;
  return nullptr;
}

template <>
CefWrapperType CefCppToCRefCounted<CefAudioHandlerCppToC,
                                   CefAudioHandler,
                                   cef_audio_handler_t>::kWrapperType =
    WT_AUDIO_HANDLER;
