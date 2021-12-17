/******************** (C) Copyright 2021 realworld one GmbH & Co. KG All Rights Reserved. ********************/

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UrBrowserCompositor.h"
#include "UraniumSubsystem.generated.h"

class IUraniumContext;
class UUrBrowserComposition;

DECLARE_DYNAMIC_DELEGATE(FOnCefInitializedDel);


UCLASS(BlueprintType)
class URANIUM_API UUraniumSubsystem : public UEngineSubsystem, public IUrBrowserCompositor
{
	GENERATED_BODY()
private:

	UPROPERTY()
	TScriptInterface<IUraniumContext> Context;

	UFUNCTION()
	void BeginUraniumInit();

	UFUNCTION()
	void EndUraniumInit();

	UFUNCTION()
	void HandleContextDeinit();

	UPROPERTY()
	UClass* OverrideContextClass;

	UPROPERTY()
	TSubclassOf<UUrBrowserComposition> DefaultHandlerCompositionClass;
	
public:
	
	/** Use |WaitForUraniumInitialized| in Blueprint. */
	FSimpleMulticastDelegate OnCefInitialized;
	FSimpleMulticastDelegate OnCefShutdown;

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnBeforeInitialize, UUraniumSubsystem*)
	static FOnBeforeInitialize OnBeforeInitialize;

	virtual void Initialize(FSubsystemCollectionBase& collection) override;
	virtual void Deinitialize() override;

	void RegisterContextClass(UClass* contextClass);
	void RegisterDefaultHandlerComposition(TSubclassOf<UUrBrowserComposition> handlerCompClass);
	
	UFUNCTION(BlueprintCallable, Category = "Uranium")
	void WaitForUraniumInitialized(FOnCefInitializedDel onInitialized);

	void Shutdown();

	UFUNCTION(BlueprintPure, Category = "Uranium")
	FString GetUserAgent();

	UFUNCTION(BlueprintCallable, Category = "Uranium")
	void SetUserAgent(FString userAgent);

	UFUNCTION(BlueprintPure, Category = "Uranium")
	FString GetCefVersion();

	UFUNCTION(BlueprintPure, Category = "Uranium")
	FString GetChromiumVersion();

	UFUNCTION(BlueprintPure, Category = "Uranium")
	bool IsCefRunning();

	UFUNCTION(BlueprintPure, Category = "Uranium")
	UUrBrowserComposition* GetDefaultBrowserComposition();

	virtual UUrBrowserComposition* GetComposition_Implementation() override;
	
	static UUraniumSubsystem* Get();
	static IUraniumContext* GetContext();
};
