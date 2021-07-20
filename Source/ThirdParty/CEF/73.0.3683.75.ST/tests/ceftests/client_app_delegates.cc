// Copyright (c) 2012 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "tests/shared/browser/client_app_browser.h"
#include "tests/shared/renderer/client_app_renderer.h"

using client::ClientAppBrowser;
using client::ClientAppRenderer;

void CreateBrowserDelegates(ClientAppBrowser::DelegateSet& delegates) {
  // Bring in the Frame tests.
  extern void CreateFrameBrowserTests(ClientAppBrowser::DelegateSet &
                                      delegates);
  CreateFrameBrowserTests(delegates);

  // Bring in the Navigation tests.
  extern void CreateNavigationBrowserTests(ClientAppBrowser::DelegateSet &
                                           delegates);
  CreateNavigationBrowserTests(delegates);

  // Bring in the plugin tests.
  extern void CreatePluginBrowserTests(ClientAppBrowser::DelegateSet &
                                       delegates);
  CreatePluginBrowserTests(delegates);

  // Bring in the preference tests.
  extern void CreatePreferenceBrowserTests(ClientAppBrowser::DelegateSet &
                                           delegates);
  CreatePreferenceBrowserTests(delegates);

  // Bring in the RequestHandler tests.
  extern void CreateRequestHandlerBrowserTests(ClientAppBrowser::DelegateSet &
                                               delegates);
  CreateRequestHandlerBrowserTests(delegates);

  // Bring in the V8 tests.
  extern void CreateV8BrowserTests(ClientAppBrowser::DelegateSet & delegates);
  CreateV8BrowserTests(delegates);
}

void CreateRenderDelegates(ClientAppRenderer::DelegateSet& delegates) {
  // Bring in the Frame tests.
  extern void CreateFrameRendererTests(ClientAppRenderer::DelegateSet &
                                       delegates);
  CreateFrameRendererTests(delegates);

  // Bring in the DOM tests.
  extern void CreateDOMRendererTests(ClientAppRenderer::DelegateSet &
                                     delegates);
  CreateDOMRendererTests(delegates);

  // Bring in the message router tests.
  extern void CreateMessageRouterRendererTests(ClientAppRenderer::DelegateSet &
                                               delegates);
  CreateMessageRouterRendererTests(delegates);

  // Bring in the Navigation tests.
  extern void CreateNavigationRendererTests(ClientAppRenderer::DelegateSet &
                                            delegates);
  CreateNavigationRendererTests(delegates);

  // Bring in the process message tests.
  extern void CreateProcessMessageRendererTests(ClientAppRenderer::DelegateSet &
                                                delegates);
  CreateProcessMessageRendererTests(delegates);

  // Bring in the RequestHandler tests.
  extern void CreateRequestHandlerRendererTests(ClientAppRenderer::DelegateSet &
                                                delegates);
  CreateRequestHandlerRendererTests(delegates);

  // Bring in the routing test handler delegate.
  extern void CreateRoutingTestHandlerDelegate(ClientAppRenderer::DelegateSet &
                                               delegates);
  CreateRoutingTestHandlerDelegate(delegates);

  // Bring in the thread tests.
  extern void CreateThreadRendererTests(ClientAppRenderer::DelegateSet &
                                        delegates);
  CreateThreadRendererTests(delegates);

  // Bring in the URLRequest tests.
  extern void CreateURLRequestRendererTests(ClientAppRenderer::DelegateSet &
                                            delegates);
  CreateURLRequestRendererTests(delegates);

  // Bring in the V8 tests.
  extern void CreateV8RendererTests(ClientAppRenderer::DelegateSet & delegates);
  CreateV8RendererTests(delegates);
}

void RegisterCustomSchemes(CefRawPtr<CefSchemeRegistrar> registrar,
                           std::vector<CefString>& cookiable_schemes) {
  // Bring in the scheme handler tests.
  extern void RegisterSchemeHandlerCustomSchemes(
      CefRawPtr<CefSchemeRegistrar> registrar,
      std::vector<CefString> & cookiable_schemes);
  RegisterSchemeHandlerCustomSchemes(registrar, cookiable_schemes);

  // Bring in the cookie tests.
  extern void RegisterCookieCustomSchemes(
      CefRawPtr<CefSchemeRegistrar> registrar,
      std::vector<CefString> & cookiable_schemes);
  RegisterCookieCustomSchemes(registrar, cookiable_schemes);

  // Bring in the URLRequest tests.
  extern void RegisterURLRequestCustomSchemes(
      CefRawPtr<CefSchemeRegistrar> registrar,
      std::vector<CefString> & cookiable_schemes);
  RegisterURLRequestCustomSchemes(registrar, cookiable_schemes);
}

namespace client {

// static
void ClientAppBrowser::CreateDelegates(DelegateSet& delegates) {
  ::CreateBrowserDelegates(delegates);
}

// static
CefRefPtr<CefPrintHandler> ClientAppBrowser::CreatePrintHandler() {
  return NULL;
}

// static
void ClientAppRenderer::CreateDelegates(DelegateSet& delegates) {
  ::CreateRenderDelegates(delegates);
}

// static
void ClientApp::RegisterCustomSchemes(
    CefRawPtr<CefSchemeRegistrar> registrar,
    std::vector<CefString>& cookiable_schemes) {
  ::RegisterCustomSchemes(registrar, cookiable_schemes);
}

}  // namespace client
