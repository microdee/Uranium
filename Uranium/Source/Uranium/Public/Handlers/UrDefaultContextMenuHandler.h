// 

#pragma once
#include "UrContextMenuHandler.h"

#include "UrDefaultContextMenuHandler.generated.h"

class URANIUM_API FCefDefaultContextMenuHandler : public ICefContextMenuHandler
{
	friend class UUrDefaultContextMenuHandler;
public:

	virtual void OnBeforeContextMenu(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefContextMenuParams> params,
		CefRefPtr<CefMenuModel> model
	) override;
	
	IMPLEMENT_REFCOUNTING(FCefDefaultContextMenuHandler);
};

UCLASS(Blueprintable, BlueprintType)
class URANIUM_API UUrDefaultContextMenuHandler
	: public UObject
	, public IUrContextMenuHandler
	, public IUrHandler
{
	GENERATED_BODY()
public:
	virtual void PostInitProperties() override;
};

UCLASS()
class URANIUM_API UUrDefaultContextMenuHandlerStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	public:

	UFUNCTION(BlueprintCallable, Category="Uranium")
	static void CreateDefaultCefContextMenuHandler(TScriptInterface<IUrContextMenuHandler> handler);
};