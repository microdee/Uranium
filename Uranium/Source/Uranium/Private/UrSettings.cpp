/******************** (C) Copyright 2021 realworld one GmbH & Co. KG All Rights Reserved. ********************/

#include "UrSettings.h"

#include "Modules/ModuleManager.h"

#if WITH_EDITOR
#include "ISettingsModule.h"
#include "ISettingsSection.h"
#include "ISettingsContainer.h"
#endif

bool FUraniumSwitch::IsEmpty()
{
	return Switch.IsEmpty() && (Value.IsEmpty() || !HasValue);
}

#define LOCTEXT_NAMESPACE "UnrealityCoreSettings"

void UUrSettings::RegisterSettings()
{
#if WITH_EDITOR
	if (ISettingsModule* settingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		ISettingsContainerPtr settingsContainer = settingsModule->GetContainer("Project");

		auto specimen = GetMutableDefault<UUrSettings>();
		ISettingsSectionPtr settingsSection = settingsModule->RegisterSettings(
			"Project", "Plugins", "Uranium",
			LOCTEXT("RuntimeGeneralSettingsName", "Uranium"),
			LOCTEXT("RuntimeGeneralSettingsDescription", "These settings will only take effect when UE4 (Editor) is restarted."), specimen
		);
	}
#endif
}

void UUrSettings::UnregisterSettings()
{
#if WITH_EDITOR
	if (ISettingsModule* settingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		settingsModule->UnregisterSettings("Project", "Plugins", "Uranium");
	}
#endif
}

#undef LOCTEXT_NAMESPACE