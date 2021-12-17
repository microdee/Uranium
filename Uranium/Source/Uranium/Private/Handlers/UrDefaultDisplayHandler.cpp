// 

#include "Handlers//UrDefaultDisplayHandler.h"

void UUrDefaultDisplayHandler::PostInitProperties()
{
	Super::PostInitProperties();
	CefDisplayHandler = new FCefDefaultDisplayHandler();
	CefDisplayHandler->UrDisplayHandler = TScriptInterface<IUrDisplayHandler>(this);
}

bool UUrDefaultDisplayHandler::OnAutoResize_Implementation(UUrBrowserView* browser, FVector2D newSize)
{
	// TODO: handle more elegantly for widgets
	return true;
}

void UUrDefaultDisplayHandler::OnAddressChange_Implementation(UUrBrowserView* browser, const FString& url)
{
	CachedAddress = url;
}

void UUrDefaultDisplayHandler::OnLoadingProgressChange_Implementation(UUrBrowserView* browser, float progress)
{
	CachedProgress = progress;
}

void UUrDefaultDisplayHandler::OnTitleChange_Implementation(UUrBrowserView* browser, const FString& title)
{
	CachedTitle = title;
}

float UUrDefaultDisplayHandler::GetCachedLoadingProgress_Implementation()
{
	return CachedProgress;
}

FString UUrDefaultDisplayHandler::GetCachedTitle_Implementation()
{
	return CachedTitle;
}

FString UUrDefaultDisplayHandler::GetCachedAddress_Implementation()
{
	return CachedAddress;
}

void UUrDefaultDisplayHandlerStatics::CreateDefaultCefDisplayHandler(TScriptInterface<IUrDisplayHandler> handler)
{
	handler->CefDisplayHandler = new FCefDefaultDisplayHandler();
	handler->CefDisplayHandler->UrDisplayHandler = handler;
}
