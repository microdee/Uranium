// 

#pragma once
#include "UrKeyboardHandler.h"

#include "UrDefaultKeyboardHandler.generated.h"

class URANIUM_API FCefDefaultKeyboardHandler : public ICefKeyboardHandler
{
	friend class UUrDefaultKeyboardHandler;
public:
	
	IMPLEMENT_REFCOUNTING(FCefDefaultKeyboardHandler);
};

UCLASS(Blueprintable, BlueprintType)
class URANIUM_API UUrDefaultKeyboardHandler
	: public UObject
	, public IUrKeyboardHandler
	, public IUrHandler
{
	GENERATED_BODY()
public:
	virtual void PostInitProperties() override;
};

UCLASS()
class URANIUM_API UUrDefaultKeyboardHandlerStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	public:

	UFUNCTION(BlueprintCallable, Category="Uranium")
	static void CreateDefaultCefKeyboardHandler(TScriptInterface<IUrKeyboardHandler> handler);
};