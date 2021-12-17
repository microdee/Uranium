// 

#pragma once
#include "UrDisplayHandler.h"

#include "UrDefaultDisplayHandler.generated.h"

class URANIUM_API FCefDefaultDisplayHandler : public ICefDisplayHandler
{
	friend class UUrDefaultDisplayHandler;
public:
	
	IMPLEMENT_REFCOUNTING(FCefDefaultDisplayHandler);
};

UCLASS(Blueprintable, BlueprintType)
class URANIUM_API UUrDefaultDisplayHandler
	: public UObject
	, public IUrDisplayHandler
	, public IUrHandler
{
	GENERATED_BODY()
protected:

	float CachedProgress = 0.0f;
	FString CachedTitle {};
	FString CachedAddress {};
	
public:
	virtual void PostInitProperties() override;

	virtual bool OnAutoResize_Implementation(UUrBrowserView* browser, FVector2D newSize) override;
	
	virtual void OnAddressChange_Implementation(UUrBrowserView* browser, const FString& url) override;
	virtual void OnLoadingProgressChange_Implementation(UUrBrowserView* browser, float progress) override;
	virtual void OnTitleChange_Implementation(UUrBrowserView* browser, const FString& title) override;
	virtual float GetCachedLoadingProgress_Implementation() override;
	virtual FString GetCachedTitle_Implementation() override;
	virtual FString GetCachedAddress_Implementation() override;
};

UCLASS()
class URANIUM_API UUrDefaultDisplayHandlerStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	public:

	UFUNCTION(BlueprintCallable, Category="Uranium")
	static void CreateDefaultCefDisplayHandler(TScriptInterface<IUrDisplayHandler> handler);
};