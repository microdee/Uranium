// 

#pragma once
#include "UrDownloadHandler.h"

#include "UrDefaultDownloadHandler.generated.h"

class URANIUM_API FCefDefaultDownloadHandler : public ICefDownloadHandler
{
	friend class UUrDefaultDownloadHandler;
public:
	
	virtual void OnBeforeDownload(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefDownloadItem> downloadItem,
		const CefString& suggestedName,
		CefRefPtr<CefBeforeDownloadCallback> callback
	) override;

	virtual void OnDownloadUpdated(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefDownloadItem> downloadItem,
		CefRefPtr<CefDownloadItemCallback> callback
	) override;
	
	IMPLEMENT_REFCOUNTING(FCefDefaultDownloadHandler);
};

UCLASS(Blueprintable, BlueprintType)
class URANIUM_API UUrDefaultDownloadHandler
	: public UObject
	, public IUrDownloadHandler
	, public IUrHandler
{
	GENERATED_BODY()
public:
	virtual void PostInitProperties() override;
};

UCLASS()
class URANIUM_API UUrDefaultDownloadHandlerStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	public:

	UFUNCTION(BlueprintCallable, Category="Uranium")
	static void CreateDefaultCefDownloadHandler(TScriptInterface<IUrDownloadHandler> handler);
};