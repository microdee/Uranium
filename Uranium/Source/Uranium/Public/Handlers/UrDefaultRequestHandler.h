// 

#pragma once
#include "UrRequestHandler.h"

#include "UrDefaultRequestHandler.generated.h"

class URANIUM_API FCefDefaultRequestHandler : public ICefRequestHandler
{
	friend class UUrDefaultRequestHandler;
public:
	
	IMPLEMENT_REFCOUNTING(FCefDefaultRequestHandler);
};

UCLASS(Blueprintable, BlueprintType)
class URANIUM_API UUrDefaultRequestHandler
	: public UObject
	, public IUrRequestHandler
	, public IUrHandler
{
	GENERATED_BODY()
public:
	virtual void PostInitProperties() override;
};

UCLASS()
class URANIUM_API UUrDefaultRequestHandlerStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	public:

	UFUNCTION(BlueprintCallable, Category="Uranium")
	static void CreateDefaultCefRequestHandler(TScriptInterface<IUrRequestHandler> handler);
};