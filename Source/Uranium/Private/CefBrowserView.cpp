// MESO Digital Interiors GmbH. (C) 2020-

#include "CefBrowserView.h"
#include "UrBrowserView.h"
#include "UrCefEnums.h"
#include "Async/Async.h"
#include "CefBrowserManagedRef.h"
#include "HAL/Event.h"
#include "SharedTexture/SharedTextureInterface.h"

CefRefPtr<FCefBrowserView> FCefBrowserView::CreateNew(
    FVector2D initialSize,
    FString url,
    UUrBrowserView* wrapper)
{
    initialSize = FMath::Max(initialSize, FVector2D(128, 128));

    CefWindowInfo WindowInfo;
    WindowInfo.width = static_cast<int>(initialSize.X);
    WindowInfo.height = static_cast<int>(initialSize.Y);

    CefBrowserSettings brows;
    SetCommonBrowserSettings(WindowInfo, brows);

    CefRefPtr<FCefBrowserView> res(new FCefBrowserView());
    res->vMainSize = initialSize;

    if(CefBrowserHost::CreateBrowser(
        WindowInfo, res, CefString(*url), brows,
#if UR_CHROM_COMPAT_EXTRAINFO()
        nullptr,
#endif
        nullptr)
    ) {
        UE_LOG(LogUranium, Verbose, TEXT("Created CEF browser."));
        res->UrWrapper = wrapper;
        return res;
    }
    else
    {
        UE_LOG(LogUranium, Error, TEXT("Couldn't create CEF browser."));
        return nullptr;
    }
}

#pragma region CefRenderHandler

void FCefBrowserView::SetCommonBrowserSettings(CefWindowInfo& winInfo, CefBrowserSettings& settings)
{
    winInfo.SetAsWindowless(nullptr);
    winInfo.windowless_rendering_enabled = true;
    winInfo.x = winInfo.y = 0;
    winInfo.shared_texture_enabled = true;
    winInfo.external_begin_frame_enabled = SEND_EXTERNAL_BEGINFRAME;

    settings.file_access_from_file_urls = STATE_ENABLED;
    settings.image_loading = STATE_ENABLED;
    settings.javascript = STATE_ENABLED;
    settings.javascript_access_clipboard = STATE_ENABLED;
    settings.local_storage = STATE_ENABLED;
    settings.remote_fonts = STATE_ENABLED;
    settings.universal_access_from_file_urls = STATE_ENABLED;
    settings.web_security = STATE_DISABLED;
    settings.webgl = STATE_ENABLED;
    settings.windowless_frame_rate = 60;
}

void FCefBrowserView::ShowDevToolsWindow(FVector2D InspectElementAt)
{
    CefWindowInfo WindowInfo;
    WindowInfo.SetAsPopup(nullptr, "Uranium Developer Tools");
    WindowInfo.style = WS_VISIBLE | WS_OVERLAPPEDWINDOW;
    WindowInfo.x = 0;
    WindowInfo.y = 0;
    WindowInfo.width = 640;
    WindowInfo.height = 480;
    BrowserHost->ShowDevTools(
        WindowInfo,
        new FCefWindowedDevToolsClient,
        CefBrowserSettings(),
        {
            FMath::FloorToInt(InspectElementAt.X),
            FMath::FloorToInt(InspectElementAt.Y)
        }
    );
}

void FCefBrowserView::ShowDevToolsEmbedded(FVector2D InspectElementAt, int width, int height)
{
    CefWindowInfo WindowInfo;
    CefBrowserSettings DtBrowserSettings;
    SetCommonBrowserSettings(WindowInfo, DtBrowserSettings);
    WindowInfo.width = width;
    WindowInfo.height = height;

    CefRefPtr<FCefBrowserView> DevToolBrowser(new FCefBrowserView());
    DevToolBrowser->vMainSize =
    {
        static_cast<float>(width),
        static_cast<float>(height)
    };

    BrowserHost->ShowDevTools(
        WindowInfo,
        DevToolBrowser,
        DtBrowserSettings,
        {
            FMath::FloorToInt(InspectElementAt.X),
            FMath::FloorToInt(InspectElementAt.Y)
        }
    );

    UUrBrowserView* newWindow = UUrBrowserView::CreateNew(UrWrapper.Get());
    auto BrowserRef = NewObject<UCefBrowserManagedRef>(UrWrapper.Get());
    BrowserRef->Obj = DevToolBrowser;
    newWindow->DeferredInit(BrowserRef);
    UrWrapper->OnEmbeddedDevToolOpenedStatic.Broadcast(newWindow);
    UrWrapper->OnEmbeddedDevToolOpened.Broadcast(newWindow);
}

void FCefBrowserView::Close()
{
    BrowserHost->CloseBrowser(false);
}

bool FCefBrowserView::IsAllReady()
{
    if(!this) return false;
    return IsBrowserReady() && UrWrapper.IsValid();
}

bool FCefBrowserView::IsBrowserReady()
{
    if(!this) return false;
    return bInitialized && Browser && BrowserHost;
}

bool FCefBrowserView::GetRootScreenRect(CefRefPtr<CefBrowser> browser, CefRect& rect)
{
    // If this method returns false the rectangle from GetViewRect will be used.
    return false;
}

bool FCefBrowserView::GetScreenInfo(CefRefPtr<CefBrowser> browser, CefScreenInfo& screen_info)
{
    if(UrWrapper.IsValid())
        screen_info.device_scale_factor = UrWrapper->ScaleFactor;
    screen_info.available_rect =
    {
        0, 0,
        static_cast<int>(vMainSize.X),
        static_cast<int>(vMainSize.Y)
    };

    // Return true if the |screen_info| structure has been modified.
    // If the screen info rectangle is left empty the rectangle from GetViewRect will be used.
    // If the rectangle is still empty or invalid popups may not be drawn correctly.
    return true;
}

void FCefBrowserView::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect)
{
    rect.Set(
        0, 0,
        static_cast<int>(vMainSize.X),
        static_cast<int>(vMainSize.Y)
    );
}

void FCefBrowserView::OnAcceleratedPaint(
    CefRefPtr<CefBrowser> browser,
    PaintElementType type,
    const RectList& dirtyRects,
    void* shared_handle)
{
    if (!UrWrapper.IsValid()) return;

    switch (type)
    {
    case PET_VIEW:
    {
        UrWrapper->MainTexture->OnAcceleratedPaint(shared_handle);
    } break;

    case PET_POPUP:
    {
        UrWrapper->NativePopupTexture->OnAcceleratedPaint(shared_handle);
    } break;
    default: break;
    }
}

void FCefBrowserView::OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList& dirtyRects, const void* buffer, int width, int height)
{
    // Nothing should happen here afaik
}

void FCefBrowserView::OnPopupShow(CefRefPtr<CefBrowser> browser, bool show)
{
    AsyncTask(ENamedThreads::GameThread, [this, show]()
    {
        if (!UrWrapper.IsValid()) return;
        UrWrapper->bPopupShown = show;
        UrWrapper->OnNativePopupShowStatic.Broadcast(show);
        UrWrapper->OnNativePopupShow.Broadcast(show);
    });
}

void FCefBrowserView::OnPopupSize(CefRefPtr<CefBrowser> browser, const CefRect& rect)
{
    AsyncTask(ENamedThreads::GameThread, [this, rect]()
    {
        if (!UrWrapper.IsValid()) return;
        UrWrapper->NativePopupPosition = FVector2D(rect.x, rect.y);
        UrWrapper->NativePopupSize = FVector2D(rect.width, rect.height);
        UrWrapper->OnNativePopupSizeStatic.Broadcast(
            FVector2D(rect.x, rect.y),
            FVector2D(rect.width, rect.height)
        );
        UrWrapper->OnNativePopupSize.Broadcast(
            FVector2D(rect.x, rect.y),
            FVector2D(rect.width, rect.height)
        );
    });
}

void FCefBrowserView::OnScrollOffsetChanged(CefRefPtr<CefBrowser> browser, double x, double y)
{
}

void FCefBrowserView::OnTextSelectionChanged(CefRefPtr<CefBrowser> browser, const CefString& selected_text, const CefRange& selected_range)
{
}

void FCefBrowserView::OnVirtualKeyboardRequested(CefRefPtr<CefBrowser> browser, TextInputMode input_mode)
{
    AsyncTask(ENamedThreads::GameThread, [this, input_mode]()
    {
        if (!UrWrapper.IsValid()) return;
        UrWrapper->OnVirtualKeyboardRequestedStatic.Broadcast(static_cast<EUrTextInputMode>(input_mode));
        UrWrapper->OnVirtualKeyboardRequested.Broadcast(static_cast<EUrTextInputMode>(input_mode));
    });
}
#pragma endregion CefRenderHandler

#pragma region CefLifeSpanHandler

void FCefBrowserView::OnAfterCreated(CefRefPtr<CefBrowser> browser)
{
    bInitialized = true;
    Browser = browser;
    BrowserHost = browser->GetHost();

    if (!UrWrapper.IsValid()) return;

    AsyncTask(ENamedThreads::GameThread, [this]()
    {
        UrWrapper->OnAfterCreatedStatic.Broadcast(UrWrapper.Get());
        UrWrapper->OnAfterCreated.Broadcast(UrWrapper.Get());
    });
}

void FCefBrowserView::OnBeforeClose(CefRefPtr<CefBrowser> browser)
{
    bInitialized = false;
    AsyncTask(ENamedThreads::GameThread, [this]()
    {
        if(UrWrapper.IsValid())
        {
            UrWrapper->OnBeforeCloseStatic.Broadcast(UrWrapper.Get());
            UrWrapper->OnBeforeClose.Broadcast(UrWrapper.Get());
            UrWrapper->OnBeforeCloseInternal();
            UrWrapper.Reset();
        }
    });
}

bool FCefBrowserView::OnBeforePopup(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    const CefString& target_url,
    const CefString& target_frame_name,
    CefLifeSpanHandler::WindowOpenDisposition target_disposition,
    bool user_gesture,
    const CefPopupFeatures& popupFeatures,
    CefWindowInfo& windowInfo,
    CefRefPtr<CefClient>& client,
    CefBrowserSettings& settings,
#if UR_CHROM_COMPAT_EXTRAINFO()
    CefRefPtr<CefDictionaryValue>& extra_info,
#endif
    bool* no_javascript_access)
{
    if (!UrWrapper.IsValid()) return true;

    switch (UrWrapper->NewWindowBehavior)
    {
    case EUrNewWindowBehavior::OpenInNewBrowser:
    {
        SetCommonBrowserSettings(windowInfo, settings);
        AsyncTask(ENamedThreads::GameThread, [=]()
        {
            if(!UrWrapper.IsValid()) return;

            FNewBrowserMetadata Metadata(
                FString(target_url.c_str()),
                FString(target_frame_name.c_str()),
                static_cast<EUrWindowDisposition>(target_disposition),
                {
                    static_cast<float>(windowInfo.width),
                    static_cast<float>(windowInfo.height)
                }
            );
            
            UUrBrowserView* newWindow = UUrBrowserView::CreateNew(UrWrapper.Get());
            UrWrapper->OnNewWindowOpenedStatic.Broadcast(newWindow, Metadata);
            UrWrapper->OnNewWindowOpened.Broadcast(newWindow, Metadata);
        });
        return false;
    }
    case EUrNewWindowBehavior::OpenInSameBrowser:
    {
        Browser->GetMainFrame()->LoadURL(target_url);
        return true;
    }
    case EUrNewWindowBehavior::Suppress:
    {
        UE_LOG(LogUranium, Display, TEXT("Opening a new browser window has been suppressed (%s)"), target_url.c_str());
        return true;
    }
    default: { return true; }
    }
}
#pragma endregion CefLifeSpanHandler

#pragma region CefLoadHandler
void FCefBrowserView::OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode)
{
    AsyncTask(ENamedThreads::GameThread, [this, httpStatusCode]()
    {
        if(!UrWrapper.IsValid()) return;

        UrWrapper->bLoading = false;
        UrWrapper->OnLoadEndStatic.Broadcast(httpStatusCode);
        UrWrapper->OnLoadEnd.Broadcast(httpStatusCode);
    });
}

void FCefBrowserView::OnLoadError(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame, 
    ErrorCode errorCode,
    const CefString& errorText,
    const CefString& failedUrl)
{
    AsyncTask(ENamedThreads::GameThread, [=]()
    {
        if (UrWrapper.IsValid()) return;
        UrWrapper->OnLoadErrorStatic.Broadcast(FUrCefErrorCode(errorCode), FString(failedUrl.c_str()));
        UrWrapper->OnLoadError.Broadcast(FUrCefErrorCode(errorCode), FString(failedUrl.c_str()));
    });
}

void FCefBrowserView::OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, TransitionType transition_type)
{
    if (!UrWrapper.IsValid()) return;
    UrWrapper->bLoading = true;
}

void FCefBrowserView::OnLoadingStateChange(CefRefPtr<CefBrowser> browser, bool isLoading, bool canGoBack, bool canGoForward)
{
}
#pragma endregion CefLoadHandler

#pragma region CefRequestHandler
//bool FCefBrowserView::CanGetCookies(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request)
//{
//	// TODO: set cookies by user
//	return false;
//}
//
//bool FCefBrowserView::CanSetCookie(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, const CefCookie& cookie)
//{
//	// TODO: set cookies by user
//	return false;
//}

bool FCefBrowserView::OnBeforeBrowse(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefRequest> request,
    bool user_gesture,
    bool is_redirect)
{
    // TODO: url filter
    return false;
}

void FCefBrowserView::OnPluginCrashed(CefRefPtr<CefBrowser> browser, const CefString& plugin_path)
{
    UE_LOG(LogUranium, Display, TEXT("[CEF] Plugin crashed: %s"), plugin_path.c_str());
}

void FCefBrowserView::OnRenderProcessTerminated(CefRefPtr<CefBrowser> browser, TerminationStatus status)
{
    UE_LOG(LogUranium, Display, TEXT("[CEF] RenderProcess terminated with reason: %i"), status);
}

void FCefBrowserView::OnRenderViewReady(CefRefPtr<CefBrowser> browser)
{
}

CefRefPtr<CefResourceRequestHandler> FCefBrowserView::GetResourceRequestHandler(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefRequest> request,
    bool is_navigation,
    bool is_download,
    const CefString& request_initiator,
    bool& disable_default_handling)
{
    // TODO implement custom html loading with CefResourceRequestHandlers and CefResourceHandlers
    return nullptr;
}
#pragma endregion CefRequestHandler

#pragma region CefDisplayHandler
void FCefBrowserView::OnAddressChange(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url)
{
    AsyncTask(ENamedThreads::GameThread, [this, url]()
    {
        if (!UrWrapper.IsValid()) return;
        UrWrapper->OnAddressChangeStatic.Broadcast(FString(url.c_str()));
        UrWrapper->OnAddressChange.Broadcast(FString(url.c_str()));
    });
}

bool FCefBrowserView::OnAutoResize(CefRefPtr<CefBrowser> browser, const CefSize& new_size)
{
    // TODO set with CefBrowserHost::SetAutoResizeEnabled and handle with SetMainSize
    AsyncTask(ENamedThreads::GameThread, [this, new_size]()
    {
        if (!UrWrapper.IsValid()) return;
        UrWrapper->OnAutoResizeStatic.Broadcast(FVector2D(new_size.width, new_size.height));
        UrWrapper->OnAutoResize.Broadcast(FVector2D(new_size.width, new_size.height));
    });
    return true;
}

bool FCefBrowserView::OnConsoleMessage(
    CefRefPtr<CefBrowser> browser,
    cef_log_severity_t level,
    const CefString& message,
    const CefString& source,
    int line)
{
    UE_LOG(LogUraniumConsole, Display, TEXT("(%s %i) %s"), source.c_str(), line, message.c_str());

    AsyncTask(ENamedThreads::GameThread, [=]()
    {
        if (!UrWrapper.IsValid()) return;
        UrWrapper->OnConsoleMessageStatic.Broadcast(
            static_cast<EUrLogSeverity>(level), FString(message.c_str()), FString(source.c_str()), line
        );
        UrWrapper->OnConsoleMessage.Broadcast(
            static_cast<EUrLogSeverity>(level), FString(message.c_str()), FString(source.c_str()), line
        );
    });
    return false;
}

void FCefBrowserView::OnFullscreenModeChange(CefRefPtr<CefBrowser> browser, bool fullscreen)
{
    AsyncTask(ENamedThreads::GameThread, [this, fullscreen]()
    {
        if (!UrWrapper.IsValid()) return;
        UrWrapper->OnFullscreenModeChangeStatic.Broadcast(fullscreen);
        UrWrapper->OnFullscreenModeChange.Broadcast(fullscreen);
    });
}

void FCefBrowserView::OnLoadingProgressChange(CefRefPtr<CefBrowser> browser, double progress)
{
    AsyncTask(ENamedThreads::GameThread, [this, progress]()
    {
        if (!UrWrapper.IsValid()) return;
        UrWrapper->OnLoadingProgressChangeStatic.Broadcast(static_cast<float>(progress));
        UrWrapper->OnLoadingProgressChange.Broadcast(static_cast<float>(progress));
    });
}

void FCefBrowserView::OnStatusMessage(CefRefPtr<CefBrowser> browser, const CefString& value)
{
    UE_LOG(LogUranium, Display, TEXT("[CEF Status] %s"), value.c_str());
}
#pragma endregion CefDisplayHandler

void FCefBrowserView::OnBeforeContextMenu(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefContextMenuParams> params,
    CefRefPtr<CefMenuModel> model)
{
    model->Clear();
}

bool FCefBrowserView::OnJSDialog(
    CefRefPtr<CefBrowser> browser,
    const CefString& origin_url,
    JSDialogType dialog_type,
    const CefString& message_text,
    const CefString& default_prompt_text,
    CefRefPtr<CefJSDialogCallback> callback,
    bool& suppress_message)
{
    suppress_message = true;
    return false;
}

bool FCefBrowserView::OnFileDialog(
    CefRefPtr<CefBrowser> browser, 
    FileDialogMode mode, 
    const CefString& title, 
    const CefString& default_file_path,
    const std::vector<CefString>& accept_filters,
    int selected_accept_filter,
    CefRefPtr<CefFileDialogCallback> callback)
{
    callback->Cancel();
    return true;
}
