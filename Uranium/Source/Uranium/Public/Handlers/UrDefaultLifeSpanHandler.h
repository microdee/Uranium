// 

#pragma once
#include "UrLifeSpanHandler.h"

#include "UrDefaultLifeSpanHandler.generated.h"

class URANIUM_API FCefDefaultLifeSpanHandler : public ICefLifeSpanHandler
{
	friend class UUrDefaultLifeSpanHandler;
public:
	
	IMPLEMENT_REFCOUNTING(FCefDefaultLifeSpanHandler);
};

UCLASS(Blueprintable, BlueprintType)
class URANIUM_API UUrDefaultLifeSpanHandler
	: public UObject
	, public IUrLifeSpanHandler
	, public IUrHandler
{
	GENERATED_BODY()
public:
	virtual void PostInitProperties() override;

	/**
	 * The preferred behavior when traditionally a new window would be opened in a web-browser.
	 * By default it is set to |EUrNewWindowBehavior::OpenInSameBrowser| which just navigates
	 * the current browser to the target URL, without creating a new browser instance.
	 */
	UPROPERTY(EditAnywhere, Category = "Uranium")
	EUrNewWindowBehavior NewWindowBehavior = EUrNewWindowBehavior::OpenInSameBrowser;

	virtual EUrNewWindowBehavior GetNewWindowBehavior_Implementation(UUrBrowserView* browser, FNewBrowserMetadata newWindowArguments) override;
};

UCLASS()
class URANIUM_API UUrDefaultLifeSpanHandlerStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	public:

	UFUNCTION(BlueprintCallable, Category="Uranium")
	static void CreateDefaultCefLifeSpanHandler(TScriptInterface<IUrLifeSpanHandler> handler);
};