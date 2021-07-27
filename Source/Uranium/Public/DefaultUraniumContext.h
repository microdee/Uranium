// MESO Digital Interiors GmbH. (C) 2020-
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UraniumContext.h"

#include <thread>

#include "DefaultUraniumContext.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUraniumVoidDel);

class FEvent;

/**
* Context object for all functionalities usually handled in global scope
* for regular CEF applications. This object might serve as a base class
* to extend functionality of Uranium
*/
UCLASS()
class URANIUM_API UDefaultUraniumContext : public UObject, public IUraniumContext
{
	GENERATED_BODY()
private:
	TSharedPtr<std::thread, ESPMode::ThreadSafe> MessageLoopThread;

protected:
	CefRefPtr<FUraniumApp> App;
	
	bool bMessageLoopOnEngineTick = false;
	bool bIsCefRunning = false;
	bool bIsCefInitializing = false;
	bool bCefModulesLoaded = false;
	int FrameRate = 60;
	FString UserAgent {};
	
	virtual void MessageLoop();

public:
	virtual CefRefPtr<FUraniumApp> GetApp() override { return App; }
	virtual CefSettings GetCefSettings() override;
	virtual FString GetUserAgent() override;
	virtual void SetUserAgent(FString userAgent) override;
	virtual bool IsCefRunning() override { return bIsCefRunning; }
	virtual bool IsInitializing() override { return bIsCefInitializing; }
	virtual void Initialize(UUraniumSubsystem* owner) override;
	virtual void Deinitialize() override;

	UFUNCTION()
	virtual void Tick(float DeltaTime);

	virtual void BeginDestroy() override;
};
