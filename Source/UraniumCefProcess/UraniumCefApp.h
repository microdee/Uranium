#pragma once

#include <thread>

#include "include/cef_app.h"
#include "include/cef_browser.h"

class UraniumCefApp
    : public CefApp
    , public CefRenderProcessHandler
    , public CefBrowserProcessHandler
{
public:

    std::shared_ptr<std::thread> ParentCheckerThread;
    static void ParentCheckerBody(CefString pid_cla /* intentional copy */);

    CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() override;

    CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override;

    void OnBeforeCommandLineProcessing(const CefString& process_type, CefRefPtr<CefCommandLine> command_line) override;

    //void OnBrowserCreated(CefRefPtr<CefBrowser> browser) override;

    void OnBrowserDestroyed(CefRefPtr<CefBrowser> browser) override;

    void OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) override;

    void OnContextReleased(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) override;

    void OnFocusedNodeChanged(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefDOMNode> node) override;

    //bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefProcessId source_process, CefRefPtr<CefProcessMessage> message) override;

    //void OnRenderThreadCreated(CefRefPtr<CefListValue> extra_info) override;

    IMPLEMENT_REFCOUNTING(UraniumCefApp);
};

