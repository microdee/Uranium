// 

#pragma once
#include "UrFocusHandler.h"

#include "UrDefaultFocusHandler.generated.h"

class URANIUM_API FCefDefaultFocusHandler : public ICefFocusHandler
{
	friend class UUrDefaultFocusHandler;
public:
	
	IMPLEMENT_REFCOUNTING(FCefDefaultFocusHandler);
};

UCLASS(Blueprintable, BlueprintType)
class URANIUM_API UUrDefaultFocusHandler
	: public UObject
	, public IUrFocusHandler
	, public IUrHandler
{
	GENERATED_BODY()
public:
	virtual void PostInitProperties() override;
};

UCLASS()
class URANIUM_API UUrDefaultFocusHandlerStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	public:

	UFUNCTION(BlueprintCallable, Category="Uranium")
	static void CreateDefaultCefFocusHandler(TScriptInterface<IUrFocusHandler> handler);
};