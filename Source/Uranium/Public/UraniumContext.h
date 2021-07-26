// MESO Digital Interiors GmbH. (C) 2020-
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UraniumApp.h"

#include "CefIncludesStart.h"

#include "include/cef_app.h"
#include "include/cef_browser.h"
#include "include/cef_client.h"
#include "include/cef_version.h"
#include <thread>

#include "CefIncludesEnd.h"

#include "UraniumContext.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUraniumVoidDel);
DECLARE_DYNAMIC_DELEGATE(FOnCefInitializedDel);

class FEvent;

/**
 * Context object for all functionalities usually handled in global scope
 * for regular CEF applications. In Uranium starting up and shutting down
 * the CEF backend designed to be controllable by the project.
 * 
 * The singleton pattern is used here making sure only one instance of
 * CEF is present any time during runtime.
 */
class URANIUM_API FUraniumContext
{
private:

	class FQuitTask : public CefTask
	{
	public:
		FQuitTask() { }
		void Execute() override {
			CefQuitMessageLoop();
		}

		IMPLEMENT_REFCOUNTING(FQuitTask);
	};

	bool bIsCefRunning = false;
	bool bIsCefInitializing = false;
	bool bCefModulesLoaded = false;
	int FrameRate = 60;
	FString UserAgent;

	TSharedPtr<std::thread, ESPMode::ThreadSafe> MessageLoopThread;
	void MessageLoop();

public:
	static TSharedPtr<FUraniumContext> Singleton;
	
	CefRefPtr<FUraniumApp> App;

	CefSettings GetCefSettings();

	FString GetUserAgent();
	void SetUserAgent(FString userAgent);

	FString GetCefVersion() { return TEXT(CEF_VERSION); }
	FString GetChromiumVersion();

	void Initialize();
	void Shutdown();
	bool IsCefRunning();
	FUraniumVoidDel OnCefInitialized;
};

URANIUM_API TSharedPtr<FUraniumContext> GetSingletonUraniumContext();

UCLASS(BlueprintType)
class URANIUM_API UUraniumContext : public UObject
{
	GENERATED_BODY()
public:

	static void GlobalTick();

	UFUNCTION(BlueprintCallable, Category = "Uranium", meta = (WorldContext="worldContext"))
	static void InitializeUranium(FOnCefInitializedDel onInitialized, UObject* worldContext);

	//UFUNCTION(BlueprintCallable, Category = "Uranium")
	static void ShutdownUranium();

	UFUNCTION(BlueprintPure, Category = "Uranium")
	static FString GetUserAgent();

	UFUNCTION(BlueprintCallable, Category = "Uranium")
	static void SetUserAgent(FString userAgent);

	UFUNCTION(BlueprintPure, Category = "Uranium")
	static FString GetCefVersion();

	UFUNCTION(BlueprintPure, Category = "Uranium")
	static FString GetChromiumVersion();

	UFUNCTION(BlueprintPure, Category = "Uranium")
	static bool IsCefRunning();
};