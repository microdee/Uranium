// 

#pragma once
#include "UrDevToolsHandler.h"

#include "UrWindowedDevToolsHandler.generated.h"

/**
 * Display the devtools as a separate window outside of UE rendering 
 */
UCLASS()
class URANIUM_API UUrWindowedDevToolsHandler
	: public UObject
	, public IUrDevToolsHandler
	, public IUrHandler
{
	GENERATED_BODY()
public:
	virtual void ShowDevTools_Implementation(UUrBrowserView* browser, FVector2D inspectElementAt, int width, int height) override;
};
