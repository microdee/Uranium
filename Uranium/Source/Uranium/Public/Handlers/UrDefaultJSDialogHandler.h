// 

#pragma once
#include "UrJSDialogHandler.h"

#include "UrDefaultJSDialogHandler.generated.h"

class URANIUM_API FCefDefaultJSDialogHandler : public ICefJSDialogHandler
{
	friend class UUrDefaultJSDialogHandler;
public:

	virtual bool OnJSDialog(
		CefRefPtr<CefBrowser> browser,
		const CefString& originUrl,
		JSDialogType dialogType,
		const CefString& messageText,
		const CefString& defaultPromptText,
		CefRefPtr<CefJSDialogCallback> callback,
		bool& suppressMessage
	) override;
	
	IMPLEMENT_REFCOUNTING(FCefDefaultJSDialogHandler);
};

UCLASS(Blueprintable, BlueprintType)
class URANIUM_API UUrDefaultJSDialogHandler
	: public UObject
	, public IUrJSDialogHandler
	, public IUrHandler
{
	GENERATED_BODY()
public:
	virtual void PostInitProperties() override;
};

UCLASS()
class URANIUM_API UUrDefaultJSDialogHandlerStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	public:

	UFUNCTION(BlueprintCallable, Category="Uranium")
	static void CreateDefaultCefJSDialogHandler(TScriptInterface<IUrJSDialogHandler> handler);
};