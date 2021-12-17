// 

#pragma once
#include "UrFindHandler.h"

#include "UrDefaultFindHandler.generated.h"

class URANIUM_API FCefDefaultFindHandler : public ICefFindHandler
{
	friend class UUrDefaultFindHandler;
public:
	
	IMPLEMENT_REFCOUNTING(FCefDefaultFindHandler);
};

UCLASS(Blueprintable, BlueprintType)
class URANIUM_API UUrDefaultFindHandler
	: public UObject
	, public IUrFindHandler
	, public IUrHandler
{
	GENERATED_BODY()
public:
	virtual void PostInitProperties() override;
};

UCLASS()
class URANIUM_API UUrDefaultFindHandlerStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable, Category="Uranium")
	static void CreateDefaultCefFindHandler(TScriptInterface<IUrFindHandler> handler);
};