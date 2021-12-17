// 

#pragma once
#include "UrLoadHandler.h"

#include "UrDefaultLoadHandler.generated.h"

class URANIUM_API FCefDefaultLoadHandler : public ICefLoadHandler
{
	friend class UUrDefaultLoadHandler;
public:
	
	IMPLEMENT_REFCOUNTING(FCefDefaultLoadHandler);
};

UCLASS(Blueprintable, BlueprintType)
class URANIUM_API UUrDefaultLoadHandler
	: public UObject
	, public IUrLoadHandler
	, public IUrHandler
{
	GENERATED_BODY()
protected:
	bool bIsLoading = false;
	
public:
	virtual void PostInitProperties() override;

	virtual bool IsLoading_Implementation() override
	{
		return bIsLoading;
	}
};

UCLASS()
class URANIUM_API UUrDefaultLoadHandlerStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable, Category="Uranium")
	static void CreateDefaultCefLoadHandler(TScriptInterface<IUrLoadHandler> handler);
};