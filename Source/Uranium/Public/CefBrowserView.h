// MESO Digital Interiors GmbH. (C) 2020-
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UObject/SoftObjectPtr.h"

#include "CefIncludesStart.h"

#include "include/cef_base.h"
#include "include/cef_app.h"
#include "include/cef_browser.h"
#include "include/cef_client.h"
#include "include/cef_version.h"

#include "CefIncludesEnd.h"
#include "Uranium.h"

#define SEND_EXTERNAL_BEGINFRAME 0

class UUrBrowserView;

class URANIUM_API FCefWindowedDevToolsClient
	: public CefClient
{
	IMPLEMENT_REFCOUNTING(FCefWindowedDevToolsClient);
};

class URANIUM_API FCefBrowserView
	: public CefClient

	// required
	, public CefRenderHandler
	, public CefLifeSpanHandler
	, public CefLoadHandler
	, public CefRequestHandler

	// info
	, public CefDisplayHandler

	// suppress
	, public CefContextMenuHandler
	, public CefJSDialogHandler
	, public CefDialogHandler
{
public:

	int MutexCounter = 0;

#pragma region Implementation

	static CefRefPtr<FCefBrowserView> CreateNew(FVector2D initialSize, FString url, UUrBrowserView* wrapper);

	bool bInitialized = false;

	TWeakObjectPtr<UUrBrowserView> UrWrapper;
	CefRefPtr<CefBrowser> Browser;
	CefRefPtr<CefBrowserHost> BrowserHost;

	FVector2D vMainSize;

	static void SetCommonBrowserSettings(CefWindowInfo& winInfo, CefBrowserSettings& settings);

	void ShowDevToolsWindow(FVector2D inspectElementAt);
	void ShowDevToolsEmbedded(FVector2D inspectElementAt, int width, int height);

	void Close();
	bool IsAllReady();
	bool IsBrowserReady();

#pragma endregion Implementation

#pragma region CefClient

	virtual CefRefPtr<CefContextMenuHandler> GetContextMenuHandler() override { return this; }
	virtual CefRefPtr<CefDialogHandler> GetDialogHandler() override { return this; }
	virtual CefRefPtr<CefDisplayHandler> GetDisplayHandler() override { return this; }
	virtual CefRefPtr<CefJSDialogHandler> GetJSDialogHandler() override { return this; }
	virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override { return this; }
	virtual CefRefPtr<CefLoadHandler> GetLoadHandler() override { return this; }
	virtual CefRefPtr<CefRenderHandler> GetRenderHandler() override { return this; }
	virtual CefRefPtr<CefRequestHandler> GetRequestHandler() override { return this; }

#pragma endregion CefClient

#pragma region CefRenderHandler

	virtual bool GetRootScreenRect(CefRefPtr<CefBrowser> browser, CefRect& rect) override;
	virtual bool GetScreenInfo(CefRefPtr<CefBrowser> browser, CefScreenInfo& screenInfo) override;
	virtual void GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) override;

	virtual void OnAcceleratedPaint(
		CefRefPtr<CefBrowser> browser,
		PaintElementType type,
		const RectList& dirtyRects,
		void* shared_handle
	) override;

	virtual void OnPaint(
		CefRefPtr<CefBrowser> browser,
		PaintElementType type,
		const RectList& dirtyRects,
		const void* buffer,
		int width, int height
	) override;

	virtual void OnPopupShow(CefRefPtr<CefBrowser> browser, bool show) override;
	virtual void OnPopupSize(CefRefPtr<CefBrowser> browser, const CefRect& rect) override;
	virtual void OnScrollOffsetChanged(CefRefPtr<CefBrowser> browser, double x, double y) override;

	virtual void OnTextSelectionChanged(
		CefRefPtr<CefBrowser> browser,
		const CefString& selectedText,
		const CefRange& selectedRange
	) override;

	virtual void OnVirtualKeyboardRequested(
		CefRefPtr<CefBrowser> browser, 
		TextInputMode inputMode
	) override;

#pragma endregion CefRenderHandler

#pragma region CefLifeSpanHandler

	virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) override;
	virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) override;

	virtual bool OnBeforePopup(
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
		bool* no_javascript_access
	) override;

#pragma endregion CefLifeSpanHandler

#pragma region CefLoadHandler

	virtual void OnLoadEnd(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		int httpStatusCode
	) override;

	virtual void OnLoadError(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		ErrorCode errorCode,
		const CefString& errorText,
		const CefString& failedUrl
	) override;

	virtual void OnLoadStart(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		TransitionType transitionType
	) override;

	virtual void OnLoadingStateChange(
		CefRefPtr<CefBrowser> browser,
		bool isLoading,
		bool canGoBack,
		bool canGoForward
	) override;

#pragma endregion CefLoadHandler

#pragma region CefRequestHandler

	//bool CanGetCookies(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request) override;
	//bool CanSetCookie(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, const CefCookie& cookie) override;

	virtual bool OnBeforeBrowse(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefRequest> request,
		bool userGesture,
		bool isRedirect
	) override;

	virtual void OnPluginCrashed(CefRefPtr<CefBrowser> browser, const CefString& pluginPath) override;
	virtual void OnRenderProcessTerminated(CefRefPtr<CefBrowser> browser, TerminationStatus status) override;
	virtual void OnRenderViewReady(CefRefPtr<CefBrowser> browser) override;

	// Used for LoadHtml
	virtual CefRefPtr<CefResourceRequestHandler> GetResourceRequestHandler(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefRequest> request,
		bool isNavigation,
		bool isDownload,
		const CefString& requestInitiator,
		bool& disableDefaultHandling
	) override;

#pragma endregion CefRequestHandler

#pragma region CefDisplayHandler

	virtual void OnAddressChange(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		const CefString& url
	) override;

	virtual bool OnAutoResize(CefRefPtr<CefBrowser> browser, const CefSize& newSize) override;

	virtual bool OnConsoleMessage(
		CefRefPtr<CefBrowser> browser,
		cef_log_severity_t level,
		const CefString& message,
		const CefString& source,
		int line
	) override;


	virtual void OnFullscreenModeChange(CefRefPtr<CefBrowser> browser, bool fullscreen) override;
	virtual void OnLoadingProgressChange(CefRefPtr<CefBrowser> browser, double progress) override;
	virtual void OnStatusMessage(CefRefPtr<CefBrowser> browser, const CefString& value) override;

#pragma endregion CefDisplayHandler

	virtual void OnBeforeContextMenu(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefContextMenuParams> params,
		CefRefPtr<CefMenuModel> model
	) override;

	virtual bool OnJSDialog(
		CefRefPtr<CefBrowser> browser,
		const CefString& originUrl,
		JSDialogType dialogType,
		const CefString& messageText,
		const CefString& defaultPromptText,
		CefRefPtr<CefJSDialogCallback> callback,
		bool& suppressMessage
	) override;

	virtual bool OnFileDialog(
		CefRefPtr<CefBrowser> browser,
		FileDialogMode mode,
		const CefString& title,
		const CefString& defaultFilePath,
		const std::vector<CefString>& acceptFilters,
		int selectedAcceptFilter,
		CefRefPtr<CefFileDialogCallback> callback
	) override;

	IMPLEMENT_REFCOUNTING(FCefBrowserView);
};