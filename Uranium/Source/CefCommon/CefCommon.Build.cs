// 

using System;
using System.Diagnostics;
using System.IO;
using Tools.DotNETCommon;
using UnrealBuildTool;

public class CefCommon : ModuleRules
{
	public bool IsCefSandboxed = false;
	
	public CefCommon(ReadOnlyTargetRules Target) : base(Target)
	{
		/*
			CMake is required to be installed to build CEF wrapper and UraniumCefProcess
		*/

		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		var cefVersion = "85.0.4183.121.ST";
		//var cefVersion = "85.0.4183.121.ST";
		//var cefVersion = "73.0.3683.75.ST";

		var isSupportedPlatform = Target.Platform == UnrealTargetPlatform.Win64;

		if (!isSupportedPlatform)
		{
			throw new InvalidOperationException("Building Uranium is not supported on platform " + Target.Platform.ToString());
		}
		
		IsCefSandboxed = CheckGlobalFlag("force sandbox");
		
		if (Target.Type == TargetType.Editor)
		{
			IsCefSandboxed = false;
		}
		
		PublicDefinitions.Add(
			IsCefSandboxed
				? "UR_CEF_SANDBOXED=1"
				: "UR_CEF_SANDBOXED=0"
		);
		
		var withSharedTexture = cefVersion.Contains(".ST")
			&& !CheckGlobalFlag("force onpaint");
		
		PublicDefinitions.Add(
			withSharedTexture
				? "UR_WITH_SHARED_TEXTURE=1"
				: "UR_WITH_SHARED_TEXTURE=0"
		);
		
		ManageCef(Target, cefVersion, cefConfig);

		PrivateIncludePathModuleNames.AddRange(new string[]
		{
			"TargetPlatform"
		});

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"ApplicationCore",
			"Core",
			"CoreUObject",
			"Engine"
		});

		PublicIncludePaths.AddRange(new string[]
		{
			Path.Combine(ModuleDirectory, "Shared")
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
	
	private void ManageCef(ReadOnlyTargetRules Target, string cefVersion, string cefConfig)
	{
		Log.WriteLine(LogEventType.Console, "Processing CEF");

		var platform = Target.Platform.ToString();

		// Configure and determine folders
		var targetDir = Path.GetDirectoryName(Target.ProjectFile.FullName);
		var cefMainDir = Path.GetFullPath(Path.Combine(PluginDirectory, "Source", "ThirdParty", "CEF"));
		var cefVersionsDir = Path.Combine(cefMainDir, platform);
		var cefInputDir = Path.Combine(cefVersionsDir, cefVersion);

		var cefOutputDir = Path.Combine(targetDir, "Binaries", platform);
		var cefBuiltDir = Path.GetFullPath(Path.Combine(PluginDirectory, "Source", "out", "ue4", platform + "-" + cefConfig));
		var cefResourceDir = Path.Combine(cefInputDir, "Resources");
		var cefBinDir = Path.Combine(cefInputDir, cefConfig);
		var cefIncludeDir = Path.Combine(cefInputDir, "include");
		var icudtlDatPath = Path.Combine(cefResourceDir, "icudtl.dat");

		var cefSubProcSrcPath = Path.Combine(PluginDirectory, "Source", "UraniumCefProcess");
		var cefSubProcOutputPath = Path.Combine(cefOutputDir, "UraniumCefProcess.exe");
		var cefSubProcBuiltPath = Path.Combine(cefBuiltDir, "UraniumCefProcess.exe");

		Log.WriteLine(LogEventType.Console, "CEF Input folder: {0}", cefInputDir);
		Log.WriteLine(LogEventType.Console, "CEF Built folder: {0}", cefBuiltDir);
		Log.WriteLine(LogEventType.Console, "CEF Output folder: {0}", cefOutputDir);

		// Invalidate make files when CEF subprocess source changes
		foreach (var file in Directory.EnumerateFiles(cefSubProcSrcPath))
		{
			ExternalDependencies.Add(file);
		}

		if (!Directory.Exists(cefInputDir))
		{
			Log.WriteLine(LogEventType.Console, "CEF Input folder: {0}", cefInputDir);

			// Archives are broke up into multiple volumes to work around Azure devops bug in LFS
			// We're checking here the first volume of the CEF archive 
			if (!File.Exists(cefInputDir.TrimEnd('\\').TrimEnd('/') + ".7z.001"))
			{
				throw new FileNotFoundException("Selected CEF version was not found.");
			}

			Log.WriteLine(LogEventType.Console, "Extracting CEF distributable archives", cefOutputDir);

			var psExtract = Process.Start(new ProcessStartInfo
			{
				FileName = "powershell.exe",
				Arguments = "-ExecutionPolicy Bypass -Command \"" + cefMainDir + "\\unpack.ps1\" ",
				WorkingDirectory = cefMainDir,
				UseShellExecute = false,
			});
			psExtract.WaitForExit();

			if(psExtract.ExitCode > 0)
			{
				throw new Exception("Failed to extract CEF distributable archives");
			}
		}

		if (!Directory.Exists(cefBinDir))
		{
			throw new FileNotFoundException("Correct binary folder of CEF was not found.");
		}

		if(!Directory.Exists(cefIncludeDir))
		{
			throw new FileNotFoundException("Include folder of CEF was not found.");
		}

		if(!Directory.Exists(cefResourceDir))
		{
			throw new FileNotFoundException("Resources folder of CEF was not found.");
		}

		if(!Directory.Exists(cefInputDir))
		{
			throw new FileNotFoundException("Specified CEF folder was not found.");
		}

		// Build CEF and UraniumCefProcess
		Console.WriteLine("Running script to build CEF wrapper and the CEF subprocess");

		var psbuild = Process.Start(new ProcessStartInfo
		{
			FileName = "powershell.exe",
			Arguments = "-ExecutionPolicy Bypass -Command \"" + PluginDirectory + "\\Source\\UraniumCef.Build.ps1\" " +
				cefVersion + " " + cefConfig + " " + platform + " $" + IsCefSandboxed.ToString(),
			WorkingDirectory = PluginDirectory + "\\Source",
			UseShellExecute = false,
		});
		psbuild.WaitForExit();

		if(!File.Exists(cefSubProcBuiltPath))
		{
			throw new Exception("Failed to build CEF wrapper or the CEF subprocess");
		}

		// Copy results and include library files

		if(!Directory.Exists(cefOutputDir))
		{
			Directory.CreateDirectory(cefOutputDir);
		}

		PublicIncludePaths.Add(cefInputDir);
		PublicAdditionalLibraries.AddRange(new string[]
		{
			Path.Combine(cefBinDir, "libcef.lib"),
			Path.Combine(cefBuiltDir, "libcef_dll_wrapper.lib")
		});

		if (IsCefSandboxed)
		{
			PublicAdditionalLibraries.Add(Path.Combine(cefBinDir, "cef_sandbox.lib"));
		}

		Log.WriteLine(LogEventType.Console, "Copying CEF Binaries");
		CopyDir(cefBinDir, cefOutputDir, true);
		//File.Copy(icudtlDatPath, Path.Combine(cefOutputDir, "Binaries", "icudtl.dat"), true);

		Log.WriteLine(LogEventType.Console, "Copying UraniumCefProcess");
		File.Copy(cefSubProcBuiltPath, cefSubProcOutputPath, true);
		RuntimeDependencies.Add(cefSubProcOutputPath);

		Log.WriteLine(LogEventType.Console, "Copying CEF Resources");
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
					Log.WriteLine(
						LogEventType.Warning,
						"Copying CEF dependency {0} failed with the following exception:",
						Path.GetFileName(temppath)
					);
					Log.WriteException(e, null);
				}
			}
			else
			{
				file.CopyTo(temppath, true);
			}
			
			if (addRtDeps)
			{
				RuntimeDependencies.Add(temppath);
			}
		}

		if (someFilesFailed)
		{
			Log.WriteLine(
				LogEventType.Warning,
				"Some existing dependencies could not be updated. Try building the project manually if that causes problems."
			);
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