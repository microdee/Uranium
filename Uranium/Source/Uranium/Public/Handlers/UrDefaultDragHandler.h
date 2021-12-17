// 

#pragma once
#include "UrDragHandler.h"

#include "UrDefaultDragHandler.generated.h"

class URANIUM_API FCefDefaultDragHandler : public ICefDragHandler
{
	friend class UUrDefaultDragHandler;
public:
	
	IMPLEMENT_REFCOUNTING(FCefDefaultDragHandler);
};

UCLASS(Blueprintable, BlueprintType)
class URANIUM_API UUrDefaultDragHandler
	: public UObject
	, public IUrDragHandler
	, public IUrHandler
{
	GENERATED_BODY()
public:
	virtual void PostInitProperties() override;
};

UCLASS()
class URANIUM_API UUrDefaultDragHandlerStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	public:

	UFUNCTION(BlueprintCallable, Category="Uranium")
	static void CreateDefaultCefDragHandler(TScriptInterface<IUrDragHandler> handler);
};