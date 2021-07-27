// Fill out your copyright notice in the Description page of Project Settings.


#include "UraniumSubsystem.h"
#include "Uranium.h"
#include "UraniumContext.h"
#include "DefaultUraniumContext.h"

void UUraniumSubsystem::BeginUraniumInit()
{
	UE_LOG(LogUranium, Display, TEXT("Beginning to initialize Uranium"));
	
	UClass* contextClass = UDefaultUraniumContext::StaticClass();
	if(OverrideContextClass)
	{
		contextClass = OverrideContextClass;
	}
	
	UE_LOG(LogUranium, Display, TEXT("Uranium context type is: %s"), *contextClass->GetName());
	Context = NewObject<UObject>(this, contextClass);
	Context->OnCefInitialized.AddUObject(this, &UUraniumSubsystem::EndUraniumInit);
	Context->OnCefShutdown.AddUObject(this, &UUraniumSubsystem::HandleContextDeinit);
	Context->Initialize(this);
}

void UUraniumSubsystem::EndUraniumInit()
{
	OnCefInitialized.Broadcast();
}

void UUraniumSubsystem::HandleContextDeinit()
{
	Context = nullptr;
}

void UUraniumSubsystem::Initialize(FSubsystemCollectionBase& collection)
{
	FCoreDelegates::OnFEngineLoopInitComplete.AddUObject(this, &UUraniumSubsystem::BeginUraniumInit);
}

void UUraniumSubsystem::Deinitialize()
{
	Shutdown();
}

void UUraniumSubsystem::RegisterContextClass(UClass* overrideDelegate)
{
	if(ensure(overrideDelegate->ImplementsInterface(UUraniumContext::StaticClass())))
	{
		ensureMsgf(OverrideContextClass == nullptr,
			TEXT("Someone else has already overriden the Uranium context class.")
			TEXT("\n    Previously registered class will be overriden by current one.")
			TEXT("\n    For safety make sure only one entity is overriding the Uranium context class!")
		);
		OverrideContextClass = overrideDelegate;
	}
}

void UUraniumSubsystem::WaitForUraniumInitialized(FOnCefInitializedDel onInitialized)
{
	if(Context && Context->IsCefRunning())
	{
		// if CEF is already running just call the input delegate
		onInitialized.ExecuteIfBound();
	}
	else
	{
		// else wait until context finishes
		OnCefInitialized.AddLambda([onInitialized]()
		{
			onInitialized.ExecuteIfBound();
		});
	}
}

void UUraniumSubsystem::Shutdown()
{
	if(Context)
	{
		Context->Deinitialize();
	}
	else
	{
		UE_LOG(LogUranium, Warning, TEXT("CEF has been already shut down."));
	}
}

FString UUraniumSubsystem::GetUserAgent()
{
	if(Context)
	{
		return Context->GetUserAgent();
	}
	return {};
}

void UUraniumSubsystem::SetUserAgent(FString userAgent)
{
	if(Context)
	{
		Context->SetUserAgent(userAgent);
	}
}

FString UUraniumSubsystem::GetCefVersion()
{
	if(Context)
	{
		return Context->GetCefVersion();
	}
	return {};
}

FString UUraniumSubsystem::GetChromiumVersion()
{
	if(Context)
	{
		return Context->GetChromiumVersion();
	}
	return {};
}

bool UUraniumSubsystem::IsCefRunning()
{
	if(Context)
	{
		return Context->IsCefRunning();
	}
	return false;
}

IUraniumContext* UUraniumSubsystem::GetContext()
{
	auto subSys = GEngine->GetEngineSubsystem<UUraniumSubsystem>();
	return Cast<IUraniumContext>(subSys->Context.GetObject());
}
