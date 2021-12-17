// 

#include "Handlers/UrLifeSpanHandler.h"

#include "UrBrowserView.h"
#include "CefBrowserClient.h"
#include "Handlers/UrHandlerEvents.h"
#include "Uranium.h"

UUrLifeSpanEvents* ICefLifeSpanHandler::GetLifeSpanEvents()
{
	return UUrLifeSpanEvents::GetEvents(UrLifeSpanHandler.ToScriptInterface());
}

void ICefLifeSpanHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser)
{
	if (UrLifeSpanHandler.IsValid())
	{
		UrLifeSpanHandler->AssignBrowserToView(AsCefPtr(browser));
		CEF_REDIRECT_EVENT(LifeSpan, OnAfterCreated, _);
	}
}

void ICefLifeSpanHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser)
{
	bIsClosed = true;
	InvokeLatentWithBrowser(UrLifeSpanHandler, [=](UUrBrowserView* view)
	{
		CEF_REDIRECT_EVENT_BODY(LifeSpan, OnBeforeClose, _);
		UrLifeSpanHandler->InvalidateBrowserAtView();
	});
}

bool ICefLifeSpanHandler::OnBeforePopup(
	CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	const CefString& targetUrl,
	const CefString& targetFrameName,
	CefLifeSpanHandler::WindowOpenDisposition targetDisposition,
	bool userGesture,
	const CefPopupFeatures& popupFeatures,
	CefWindowInfo& windowInfo,
	CefRefPtr<CefClient>& client,
	CefBrowserSettings& settings,
#if UR_CHROM_COMPAT_EXTRAINFO()
	CefRefPtr<CefDictionaryValue>& extraInfo,
#endif
	bool* noJavascriptAccess
) {
	if (!UrLifeSpanHandler.IsValid())
	{
		return true;
	}

	FNewBrowserMetadata newWindowData(
		targetUrl.c_str(),
		targetFrameName.c_str(),
		static_cast<EUrWindowDisposition>(targetDisposition),
		{
			static_cast<float>(windowInfo.width),
			static_cast<float>(windowInfo.height)
		},
		userGesture
	);

	EUrNewWindowBehavior behavior = InvokeLatentWithBrowserReturn(UrLifeSpanHandler, [=](UUrBrowserView* view)
	{
		return IUrLifeSpanHandler::Execute_GetNewWindowBehavior(UrLifeSpanHandler->_getUObject(), view, newWindowData);
	}, EUrNewWindowBehavior::Default);

	switch (behavior)
	{
	case EUrNewWindowBehavior::OpenInNewBrowser:
		{
			InvokeLatentWithBrowser(UrLifeSpanHandler, [=](UUrBrowserView* view)
			{
				// TODO: manipulate composition source from interface
				auto newBrowser = UUrBrowserView::CreateNew(view);
				
				CEF_REDIRECT_EVENT_BODY(LifeSpan, OnNewWindowOpened, (newBrowser, newWindowData))
			});
			return true;
		}
	case EUrNewWindowBehavior::Default:
	case EUrNewWindowBehavior::OpenInSameBrowser:
		{
			browser->GetMainFrame()->LoadURL(targetUrl);
			return true;
		}
	case EUrNewWindowBehavior::Suppress:
		{
			UE_LOG(LogUranium, Display, TEXT("Opening a new browser window has been suppressed (%s)"), targetUrl.c_str());
			return true;
		}
	default:
		{
			return true;
		}
	}
}

void ICefLifeSpanHandler::SetBrowserSettings(CefWindowInfo& winInfo, CefBrowserSettings& settings)
{
	winInfo.SetAsWindowless(nullptr);
	winInfo.windowless_rendering_enabled = true;
	winInfo.x = winInfo.y = 0;
	winInfo.shared_texture_enabled = UR_WITH_SHARED_TEXTURE;
	winInfo.external_begin_frame_enabled = 0; // TODO: expose

	// TODO: expose these as interface getters
	settings.file_access_from_file_urls = STATE_ENABLED;
	settings.image_loading = STATE_ENABLED;
	settings.javascript = STATE_ENABLED;
	settings.javascript_access_clipboard = STATE_ENABLED;
	settings.local_storage = STATE_ENABLED;
	settings.remote_fonts = STATE_ENABLED;
	settings.universal_access_from_file_urls = STATE_ENABLED;
#if UR_CHROM_VERSION_BELOW(92, 0, 0)
	settings.web_security = STATE_DISABLED;
#endif
	settings.webgl = STATE_ENABLED;
	settings.windowless_frame_rate = 60;
}

bool ICefLifeSpanHandler::CreateNewCefBrowser(UUrBrowserView* view, FVector2D initialSize, FString url, TCefPtr<FCefBrowserClient> client)
{
	if(UrLifeSpanHandler.IsValid())
	{
		CEF_REDIRECT_EVENT_BODY(LifeSpan, OnBeforeCreated, _);
	}
	
	initialSize = FMath::Max(initialSize, FVector2D(128, 128));
	CefWindowInfo windowInfo;
	windowInfo.width = static_cast<int>(initialSize.X);
	windowInfo.height = static_cast<int>(initialSize.Y);

	CefBrowserSettings browserSettings;
	SetBrowserSettings(windowInfo, browserSettings);

	return CefBrowserHost::CreateBrowser(
		windowInfo, client.Get(), *url, browserSettings,
#if UR_CHROM_COMPAT_EXTRAINFO()
		nullptr,
#endif
		nullptr
	);
}

void IUrLifeSpanHandler::AssignBrowserToView(TCefPtr<CefBrowser> browser)
{
	auto view = AsUrHandler(this)->GetBrowserView();
	if(view.IsValid())
	{
		view->SetCefBrowser({}, browser);
	}
}

void IUrLifeSpanHandler::InvalidateBrowserAtView()
{
	auto view = AsUrHandler(this)->GetBrowserView();
	if(view.IsValid())
	{
		view->InvalidateBrowser({});
	}
}

UUrLifeSpanEvents* UUrLifeSpanEvents::GetEvents(TScriptInterface<IUrLifeSpanHandler> handler)
{
	auto subSys = GEngine->GetEngineSubsystem<UUrHandlerEvents>();
	return Cast<UUrLifeSpanEvents>(subSys->GetEvents(handler.GetObject(), StaticClass()));
}
