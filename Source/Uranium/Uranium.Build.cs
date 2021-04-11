// Some copyright should be here...

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
    public Uranium(ReadOnlyTargetRules Target) : base(Target)
    {
        /*
            CMake is required to be installed to build CEF wrapper and UraniumCefProcess
        */

        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        
        var cefVersion = "85.0.4183.121.ST";
        //var cefVersion = "73.0.3683.75.ST";
        var cefConfig =
            Target.Configuration == UnrealTargetConfiguration.Debug ?
            // The debug binaries of 73.0.3683.75.ST are not working properly
            // /*"Debug"*/ "Release" : "Release";
            "Debug" : "Release";

        if (Target.Platform != UnrealTargetPlatform.Win64)
        {
            Console.WriteLine("This project only supports building for Win64 platform");
            return;
        }

        ManageCef(Target, cefVersion, cefConfig);

        PublicDefinitions.AddRange(new string[] { });

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
            "d3d12.lib",
            "comsuppw.lib"
        });

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
    
    private void ManageCef(ReadOnlyTargetRules Target, string cefVersion, string cefConfig)
    {
        Console.WriteLine("Processing CEF");

        // Configure and determine folders
        var targetDir = Path.GetDirectoryName(Target.ProjectFile.FullName);
        var cefInputDir = Path.GetFullPath(Path.Combine(PluginDirectory, "Source", "ThirdParty", "CEF", cefVersion));
        var cefOutputDir = Path.Combine(targetDir, "Binaries", Target.Platform.ToString());
        var cefBuiltDir = Path.GetFullPath(Path.Combine(PluginDirectory, "Source", "out", "ue4", "x64-" + cefConfig));
        var cefResourceDir = Path.Combine(cefInputDir, "Resources");
        var cefBinDir = Path.Combine(cefInputDir, cefConfig);
        var cefIncludeDir = Path.Combine(cefInputDir, "include");
        var icudtlDatPath = Path.Combine(cefResourceDir, "icudtl.dat");

        var cefSubProcSrcPath = Path.Combine(PluginDirectory, "Source", "UraniumCefProcess");
        var cefSubProcOutputPath = Path.Combine(cefOutputDir, "UraniumCefProcess.exe");
        var cefSubProcBuiltPath = Path.Combine(cefBuiltDir, "UraniumCefProcess.exe");

        // Invalidate make files when CEF subprocess source changes
        foreach (var file in Directory.EnumerateFiles(cefSubProcSrcPath))
        {
            ExternalDependencies.Add(file);
        }

        if (!Directory.Exists(cefBinDir))
            throw new FileNotFoundException("Correct binary folder of CEF was not found.");

        if(!Directory.Exists(cefIncludeDir))
            throw new FileNotFoundException("Include folder of CEF was not found.");

        if(!Directory.Exists(cefResourceDir))
            throw new FileNotFoundException("Resources folder of CEF was not found.");

        Console.WriteLine("CEF Input folder: " + cefInputDir);
        Console.WriteLine("CEF Built folder: " + cefBuiltDir);
        Console.WriteLine("CEF Output folder: " + cefOutputDir);

        if(!Directory.Exists(cefInputDir))
            throw new FileNotFoundException("Specified CEF folder was not found.");

        // Build CEF and UraniumCefProcess
        Console.WriteLine("Running script to build CEF wrapper and the CEF subprocess");

        var psbuild = Process.Start(new ProcessStartInfo {
            FileName = "powershell.exe",
            Arguments = "\"" + PluginDirectory + "\\Source\\UraniumCef.Build.ps1\" " +
                cefVersion + " " + cefConfig,
            WorkingDirectory = PluginDirectory + "\\Source",
            UseShellExecute = false,
        });
        psbuild.WaitForExit();

        if(!File.Exists(cefSubProcBuiltPath))
            throw new Exception("Failed to build CEF wrapper or the CEF subprocess");

        // Copy results and include library files

        if(!Directory.Exists(cefOutputDir))
            Directory.CreateDirectory(cefOutputDir);

        PublicIncludePaths.Add(cefInputDir);
        PublicAdditionalLibraries.AddRange(new string[]
        {
            Path.Combine(cefBinDir, "libcef.lib"),
            Path.Combine(cefBuiltDir, "libcef_dll_wrapper.lib")
        });

        Console.WriteLine("Copying CEF Binaries");
        CopyDir(cefBinDir, cefOutputDir, true);
        //File.Copy(icudtlDatPath, Path.Combine(cefOutputDir, "Binaries", "icudtl.dat"), true);

        Console.WriteLine("Copying UraniumCefProcess");
        File.Copy(cefSubProcBuiltPath, cefSubProcOutputPath, true);
        RuntimeDependencies.Add(cefSubProcOutputPath);

        Console.WriteLine("Copying CEF Resources");
        CopyDir(cefResourceDir, cefOutputDir, true);
    }

    private void CopyDir(string sourceDirName, string destDirName, bool addRtDeps = false, bool copySubDirs = true)
    {
        DirectoryInfo dir = new DirectoryInfo(sourceDirName);

        if (!dir.Exists)
        {
            throw new DirectoryNotFoundException(
                "Source directory does not exist or could not be found: "
                + sourceDirName);
        }

        DirectoryInfo[] dirs = dir.GetDirectories();
        if (!Directory.Exists(destDirName))
        {
            Directory.CreateDirectory(destDirName);
        }
        bool someFilesFailed = false;
        FileInfo[] files = dir.GetFiles();
        foreach (FileInfo file in files)
        {
            string temppath = Path.Combine(destDirName, file.Name);
            if (File.Exists(temppath))
            {
                try
                {
                    file.CopyTo(temppath, true);
                }
                catch (Exception e)
                {
                    someFilesFailed = true;
                    Console.WriteLine(
                        "Warning: Copying CEF dependency "
                        + Path.GetFileName(temppath)
                        + " failed with exception: "
                        + e.Message
                    );
                }
            }
            else file.CopyTo(temppath, true);
            if (addRtDeps)
                RuntimeDependencies.Add(temppath);
        }

        if (someFilesFailed)
        {
            Console.WriteLine("Warning: Some existing dependencies could not be updated. Try building the project manually if that causes problems.");
        }

        if (copySubDirs)
        {
            foreach (DirectoryInfo subdir in dirs)
            {
                string temppath = Path.Combine(destDirName, subdir.Name);
                CopyDir(subdir.FullName, temppath, addRtDeps, true);
            }
        }
    }
}
