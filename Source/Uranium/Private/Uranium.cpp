// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "Uranium.h"
#include "CefUtils.h"
#include "UraniumContext.h"
#include "UrSettings.h"

DEFINE_LOG_CATEGORY(LogUranium);
DEFINE_LOG_CATEGORY(LogUraniumConsole);

#define LOCTEXT_NAMESPACE "FUraniumModule"

void FUraniumModule::StartupModule()
{
    // This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
    //GetSingletonUraniumContext()->Initialize();
    UUrSettings::RegisterSettings();
}

void FUraniumModule::ShutdownModule()
{
    // This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
    // we call this function before unloading the module.
    GetSingletonUraniumContext()->Shutdown();
    UUrSettings::UnregisterSettings();
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FUraniumModule, Uranium)