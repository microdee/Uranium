// 

#pragma once
#include "UrDialogHandler.h"

#include "UrDefaultDialogHandler.generated.h"

class URANIUM_API FCefDefaultDialogHandler : public ICefDialogHandler
{
	friend class UUrDefaultDialogHandler;
public:
	
	virtual bool OnFileDialog(
		CefRefPtr<CefBrowser> browser,
		FileDialogMode mode,
		const CefString& title,
		const CefString& defaultFilePath,
		const std::vector<CefString>& acceptFilters,
		int selectedAcceptFilter,
		CefRefPtr<CefFileDialogCallback> callback
	) override;
	
	IMPLEMENT_REFCOUNTING(FCefDefaultDialogHandler);
};

UCLASS(Blueprintable, BlueprintType)
class URANIUM_API UUrDefaultDialogHandler
	: public UObject
	, public IUrDialogHandler
	, public IUrHandler
{
	GENERATED_BODY()
public:
	virtual void PostInitProperties() override;
};

UCLASS()
class URANIUM_API UUrDefaultDialogHandlerStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	public:

	UFUNCTION(BlueprintCallable, Category="Uranium")
	static void CreateDefaultCefDialogHandler(TScriptInterface<IUrDialogHandler> handler);
};