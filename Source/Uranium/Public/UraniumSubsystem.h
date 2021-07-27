// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UraniumSubsystem.generated.h"

class IUraniumContext;

DECLARE_DYNAMIC_DELEGATE(FOnCefInitializedDel);
DECLARE_DELEGATE_RetVal(UClass*, FOverrideContextClass)

UCLASS(BlueprintType)
class URANIUM_API UUraniumSubsystem : public UEngineSubsystem
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
	
public:
	
	/** Use |WaitForUraniumInitialized| in Blueprint. */
	FSimpleMulticastDelegate OnCefInitialized;
	FSimpleMulticastDelegate OnCefShutdown;

	virtual void Initialize(FSubsystemCollectionBase& collection) override;
	virtual void Deinitialize() override;

	void RegisterContextClass(UClass* overrideDelegate);
	
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

	static IUraniumContext* GetContext();
};
