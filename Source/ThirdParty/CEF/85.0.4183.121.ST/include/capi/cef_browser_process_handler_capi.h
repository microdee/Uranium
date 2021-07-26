// Copyright (c) 2020 Marshall A. Greenblatt. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//    * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//    * Neither the name of Google Inc. nor the name Chromium Embedded
// Framework nor the names of its contributors may be used to endorse
// or promote products derived from this software without specific prior
// written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// ---------------------------------------------------------------------------
//
// This file was generated by the CEF translator tool and should not edited
// by hand. See the translator.README.txt file in the tools directory for
// more information.
//
// $hash=306236316b35037523ca566068d133755bce48fd$
//

#ifndef CEF_INCLUDE_CAPI_CEF_BROWSER_PROCESS_HANDLER_CAPI_H_
#define CEF_INCLUDE_CAPI_CEF_BROWSER_PROCESS_HANDLER_CAPI_H_
#pragma once

#include "include/capi/cef_base_capi.h"
#include "include/capi/cef_command_line_capi.h"
#include "include/capi/cef_print_handler_capi.h"
#include "include/capi/cef_values_capi.h"

#ifdef __cplusplus
extern "C" {
#endif

///
// Structure used to implement browser process callbacks. The functions of this
// structure will be called on the browser process main thread unless otherwise
// indicated.
///
typedef struct _cef_browser_process_handler_t {
  ///
  // Base structure.
  ///
  cef_base_ref_counted_t base;

  ///
  // Called on the browser process UI thread immediately after the CEF context
  // has been initialized.
  ///
  void(CEF_CALLBACK* on_context_initialized)(
      struct _cef_browser_process_handler_t* self);

  ///
  // Called before a child process is launched. Will be called on the browser
  // process UI thread when launching a render process and on the browser
  // process IO thread when launching a GPU or plugin process. Provides an
  // opportunity to modify the child process command line. Do not keep a
  // reference to |command_line| outside of this function.
  ///
  void(CEF_CALLBACK* on_before_child_process_launch)(
      struct _cef_browser_process_handler_t* self,
      struct _cef_command_line_t* command_line);

  ///
  // Return the handler for printing on Linux. If a print handler is not
  // provided then printing will not be supported on the Linux platform.
  ///
  struct _cef_print_handler_t*(CEF_CALLBACK* get_print_handler)(
      struct _cef_browser_process_handler_t* self);

  ///
  // Called from any thread when work has been scheduled for the browser process
  // main (UI) thread. This callback is used in combination with CefSettings.
  // external_message_pump and cef_do_message_loop_work() in cases where the CEF
  // message loop must be integrated into an existing application message loop
  // (see additional comments and warnings on CefDoMessageLoopWork). This
  // callback should schedule a cef_do_message_loop_work() call to happen on the
  // main (UI) thread. |delay_ms| is the requested delay in milliseconds. If
  // |delay_ms| is <= 0 then the call should happen reasonably soon. If
  // |delay_ms| is > 0 then the call should be scheduled to happen after the
  // specified delay and any currently pending scheduled call should be
  // cancelled.
  ///
  void(CEF_CALLBACK* on_schedule_message_pump_work)(
      struct _cef_browser_process_handler_t* self,
      int64 delay_ms);
} cef_browser_process_handler_t;

#ifdef __cplusplus
}
#endif

#endif  // CEF_INCLUDE_CAPI_CEF_BROWSER_PROCESS_HANDLER_CAPI_H_