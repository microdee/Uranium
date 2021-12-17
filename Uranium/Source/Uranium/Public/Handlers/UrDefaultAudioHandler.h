// 

#pragma once
#include "UrAudioHandler.h"

#include "UrDefaultAudioHandler.generated.h"

class URANIUM_API FCefDefaultAudioHandler : public ICefAudioHandler
{
	friend class UUrDefaultAudioHandler;
public:
	virtual void OnAudioStreamStarted(CefRefPtr<CefBrowser> browser, const CefAudioParameters& params, int channels) override {}
	virtual void OnAudioStreamPacket(CefRefPtr<CefBrowser> browser, const float** data, int frames, int64 pts) override {}
	virtual void OnAudioStreamStopped(CefRefPtr<CefBrowser> browser) override {}
	virtual void OnAudioStreamError(CefRefPtr<CefBrowser> browser, const CefString& message) override {}
	
	IMPLEMENT_REFCOUNTING(FCefDefaultAudioHandler);
};

UCLASS(BlueprintType, Blueprintable)
class URANIUM_API UUrDefaultAudioHandler
	: public UObject
	, public IUrAudioHandler
	, public IUrHandler
{
	GENERATED_BODY()
public:
	virtual void PostInitProperties() override;
};

UCLASS()
class URANIUM_API UUrDefaultAudioHandlerStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable, Category="Uranium")
	static void CreateDefaultCefAudioHandler(TScriptInterface<IUrAudioHandler> handler);
};