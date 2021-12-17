// 

#include "Handlers/UrWindowedDevToolsHandler.h"

#include "UrBrowserView.h"
#include "Handlers/UrDisplayHandler.h"

/** Empty client class for windowed devtools */
class URANIUM_API FCefWindowedDevToolsClient
	: public CefClient
{
	IMPLEMENT_REFCOUNTING(FCefWindowedDevToolsClient);
};

void UUrWindowedDevToolsHandler::ShowDevTools_Implementation(UUrBrowserView* browser, FVector2D inspectElementAt, int width, int height)
{
	IUrHandler* self = AsUrHandler(this);
	if(self->GetBrowser())
	{
		FString address = IUrDisplayHandler::Execute_GetCachedAddress(browser->DisplayHandler);
		FString windowTitle = FString::Printf(TEXT("Uranium Developer Tools: %s"), *address);
		CefWindowInfo windowInfo;
		windowInfo.SetAsPopup(nullptr, *windowTitle);
		windowInfo.style = WS_VISIBLE | WS_OVERLAPPEDWINDOW;
		windowInfo.x = 0;
		windowInfo.y = 0;
		windowInfo.width = 800;
		windowInfo.height = 600;
		self->GetBrowser()->GetHost()->ShowDevTools(
			windowInfo,
			new FCefWindowedDevToolsClient,
			CefBrowserSettings(),
			{
				FMath::FloorToInt(inspectElementAt.X),
				FMath::FloorToInt(inspectElementAt.Y)
			}
		);
	}
}