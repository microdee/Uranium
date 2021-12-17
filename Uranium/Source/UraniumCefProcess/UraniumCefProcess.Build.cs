/******************** (C) Copyright 2021 realworld one GmbH & Co. KG All Rights Reserved. ********************/

using System;
using System.IO;
using UnrealBuildTool;

public class UraniumCefProcess : ModuleRules
{
	public bool IsCefSandboxed = false;
	
	public UraniumCefProcess(ReadOnlyTargetRules Target) : base(Target)
	{
		// UraniumCefProcess is also compiled as a separate executable with CMake.
		// If IsCefSandboxed is set then it's also acting like a regular module.
		
		IsCefSandboxed = CheckGlobalFlag("force sandbox");

		if (Target.Type == TargetType.Editor)
		{
			IsCefSandboxed = false;
		}
		CppStandard = CppStandardVersion.Cpp17;
		
		PublicDependencyModuleNames.AddRange(new []
		{
			"ApplicationCore",
			"Core",
			"CoreUObject",
			"Engine",
			"CefCommon"
		});
		PublicIncludePaths.AddRange(new string[]
		{
			Path.Combine(PluginDirectory, "Source", "CefCommon", "Shared")
		});
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

