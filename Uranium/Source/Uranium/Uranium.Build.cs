/******************** (C) Copyright 2021 realworld one GmbH & Co. KG All Rights Reserved. ********************/

// Find documentation and class members at this links:
// https://github.com/EpicGames/UnrealEngine/blob/release/Engine/Source/Programs/UnrealBuildTool/Configuration/ModuleRules.cs
// https://github.com/EpicGames/UnrealEngine/blob/release/Engine/Source/Programs/UnrealBuildTool/Configuration/ModuleRulesContext.cs
// https://github.com/EpicGames/UnrealEngine/blob/release/Engine/Source/Programs/UnrealBuildTool/Configuration/TargetRules.cs

using UnrealBuildTool;
using System;
using System.Linq;
using System.Diagnostics;
using System.IO;
using Tools.DotNETCommon;

public class Uranium : ModuleRules
{
	public bool IsCefSandboxed = false;
	
	public Uranium(ReadOnlyTargetRules Target) : base(Target)
	{

		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		IsCefSandboxed = CheckGlobalFlag("force sandbox");

		if (Target.Type == TargetType.Editor)
		{
			IsCefSandboxed = false;
		}

		PublicIncludePaths.AddRange(new string[]
		{
			Path.Combine(PluginDirectory, "Source", "CefCommon", "Shared")
		});

		PrivateIncludePaths.AddRange(new string[]
		{
			// ... add other private include paths required here ...
		});

		PrivateIncludePathModuleNames.AddRange(new string[]
		{
			"TargetPlatform"
		});

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"ApplicationCore",
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"UMG",
			"Slate",
			"SlateCore",
			"RenderCore",
			"RHI",
			
			"CefCommon"
		});
		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"D3D11RHI",
			"D3D12RHI",
		});
		PublicSystemLibraries.AddRange(new string[]
		{
			"dxgi.lib",
			"d3d11.lib",
			"d3d12.lib"
		});
		if (Target.Platform == UnrealTargetPlatform.Win64 || Target.Platform == UnrealTargetPlatform.Win32)
		{
			PublicSystemLibraries.AddRange(new string[]
			{
				"comsuppw.lib"
			});
		}

		AddEngineThirdPartyPrivateStaticDependencies(Target, "DX12");
		
		PrivateDependencyModuleNames.AddRange(new string[] { });

		if (Target.Type != TargetType.Server)
		{
			PrivateIncludePathModuleNames.AddRange(new string[]
			{
				"SlateRHIRenderer",
			});

			DynamicallyLoadedModuleNames.AddRange(new string[]
			{
				"ImageWrapper",
				"SlateRHIRenderer",
			});
		}

		DynamicallyLoadedModuleNames.AddRange(new string[] { });
	}
	
	bool CheckGlobalFlag(string flag)
	{
		var globalFlagsPath = Path.Combine(PluginDirectory, "Source", "global-flags.txt");
		if (File.Exists(globalFlagsPath))
		{
			foreach (var flagLine in File.ReadAllLines(globalFlagsPath))
			{
				if (flagLine.Equals(flag, StringComparison.InvariantCultureIgnoreCase))
				{
					return true;
				}
			}
		}
		return false;
	}
}