/******************** (C) Copyright 2021 realworld one GmbH & Co. KG All Rights Reserved. ********************/


#include "UraniumSubsystem.h"
#include "Uranium.h"
#include "UraniumContext.h"
#include "UrBrowserComposition.h"
#include "DefaultUraniumContext.h"

UUraniumSubsystem::FOnBeforeInitialize UUraniumSubsystem::OnBeforeInitialize;

void UUraniumSubsystem::BeginUraniumInit()
{
	UE_LOG(LogUranium, Display, TEXT("Beginning to initialize Uranium"));
	
	UClass* contextClass = UDefaultUraniumContext::StaticClass();
	OnBeforeInitialize.Broadcast(this);
	
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

void UUraniumSubsystem::RegisterContextClass(UClass* contextClass)
{
	if(ensure(contextClass->ImplementsInterface(UUraniumContext::StaticClass())))
	{
		ensureMsgf(OverrideContextClass == nullptr,
			TEXT("Someone else has already overriden the Uranium context class.")
			TEXT("\n    Previously registered class will be overriden by current one.")
			TEXT("\n    For safety make sure only one entity is overriding the Uranium context class!")
		);
		OverrideContextClass = contextClass;
	}
}

void UUraniumSubsystem::RegisterDefaultHandlerComposition(TSubclassOf<UUrBrowserComposition> handlerCompClass)
{
	ensureMsgf(DefaultHandlerCompositionClass == nullptr,
		TEXT("Someone else has already overriden the Uranium context class.")
		TEXT("\n    Previously registered class will be overriden by current one.")
		TEXT("\n    For safety make sure only one entity is overriding the Uranium context class!")
	);
	DefaultHandlerCompositionClass = handlerCompClass;
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

UUrBrowserComposition* UUraniumSubsystem::GetDefaultBrowserComposition()
{
	if(!DefaultHandlerCompositionClass)
	{
		DefaultHandlerCompositionClass = UUrBrowserComposition::StaticClass();
	}
	return GetMutableDefault<UUrBrowserComposition>(DefaultHandlerCompositionClass);
}

UUrBrowserComposition* UUraniumSubsystem::GetComposition_Implementation()
{
	return GetDefaultBrowserComposition();
}

UUraniumSubsystem* UUraniumSubsystem::Get()
{
	return GEngine->GetEngineSubsystem<UUraniumSubsystem>();
}

IUraniumContext* UUraniumSubsystem::GetContext()
{
	return Cast<IUraniumContext>(Get()->Context.GetObject());
}
