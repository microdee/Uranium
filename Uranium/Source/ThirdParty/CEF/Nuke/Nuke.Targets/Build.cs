using System;
using System.IO;
using System.Net;
using System.Linq;
using Nuke.Common;
using Nuke.Common.CI;
using Nuke.Common.Execution;
using Nuke.Common.IO;
using Nuke.Common.ProjectModel;
using Nuke.Common.Tooling;
using Nuke.Common.Utilities.Collections;
using static Nuke.Common.EnvironmentInfo;
using static Nuke.Common.IO.FileSystemTasks;
using static Nuke.Common.IO.PathConstruction;
using static Nuke.Common.Logger;
using System.Text.RegularExpressions;

[CheckBuildProjectConfigurations]
partial class Build : NukeBuild
{
    /// Support plugins are available for:
    ///   - JetBrains ReSharper        https://nuke.build/resharper
    ///   - JetBrains Rider            https://nuke.build/rider
    ///   - Microsoft VisualStudio     https://nuke.build/visualstudio
    ///   - Microsoft VSCode           https://nuke.build/vscode

    public static int Main() => Execute<Build>(x => x.Empty);

    [Parameter("A publicly available git repository URL of CEF. Unfortunately our devops repo doesn't work here. When changed the whole chromium setup needs to be done again from scratch.")]
    //public string RemoteRepo { get; set; } = "https://bitbucket.org/rrjksn/cefsharedtextures.git";
    public string RemoteRepo { get; set; } = "https://bitbucket.org/chromiumembedded/cef.git";

    [Parameter("The target release branch number of CEF")]
    //public int Revision { get; set; } = 4183;
    public int Revision { get; set; } = 4606;

    [Parameter("Target architectures to generate or build. Multiple inputs are supported.")]
    public ArchitectureConfig[] Architecture { get; set; } = new [] { ArchitectureConfig.x64 };


    [Parameter("Specify the CEF working directory. It's C:\\code by default and it is recommended to work from there. It can be a symbolic link too accross hard-drives.")]
    public AbsolutePath CefWorkDir { get; set; } = (AbsolutePath) @"C:\code";

    public AbsolutePath DepotToolsPath => CefWorkDir / "depot_tools";
    public AbsolutePath AutomateGitPy => CefWorkDir / "automate" / "automate-git.py";
    public AbsolutePath ChromiumSrcDir => CefWorkDir / "chromium_git" / "chromium" / "src";

    [Parameter("99.99% of times it should be true.")]
    public bool IsOfficialBuild { get; set; } = true;
    
    [Parameter("Wether to include proprietary codecs in the resulting package.")]
    public bool ProprietaryCodecs { get; set; } = true;

    [Parameter("Execute the Update target even when Chromium is already cloned")]
    public bool ForceUpdate { get; set; }
    
    [Parameter]
    public string FfmpegBranding  { get; set; } = "Chrome";

    [Parameter("Which Visual Studio build tools should be used")]
    public string VsVersion { get; set; } = "2019";

    [Parameter("URL to the Depot Tools which are downloaded during update")]
    public string DepotToolsUrl { get; set; } = "https://storage.googleapis.com/chrome-infra/depot_tools.zip";

    [Parameter("URL to the automate-git.py which are downloaded during update")]
    public string AutomateGitPyUrl { get; set; } = "https://bitbucket.org/chromiumembedded/cef/raw/master/tools/automate/automate-git.py";

    [Parameter("Path to the Python 2 installation. Chromium might require it in PATH during building, but only at specific times, which targets are managing here.")]
    public AbsolutePath Python2Path { get; set; } = (AbsolutePath) @"C:\Python27";

    [Parameter("0 = continue on errors (unless dependent tasks have failed) | N = stop on Nth error")]
    public int MaxBuildErrors { get; set; } = 1;

    [PathExecutable]
    readonly Tool Git;

    public Target Empty => _ => _
        .Unlisted()
        .Executes(() =>
        {
            Logger.Info("Please specify a target.");
        });

    public Target FetchDepotTools => _ => _
        .OnlyWhenDynamic(() => !Directory.Exists(DepotToolsPath))
        .Executes(() =>
        {
            var fetchScript = RootDirectory / "Scripts" / "FetchDepotTools.ps1";
            var fetchProc = ProcessTasks.StartShell(
                $@"powershell -ExecutionPolicy Bypass -Command {fetchScript} ""{CefWorkDir}"" ""{DepotToolsUrl}""",
                RootDirectory
            );
            fetchProc.WaitForExit();
            if(fetchProc.ExitCode > 0)
            {
                ControlFlow.Fail("Fetching depot tools has failed");
            }
        });

    public Target FetchAutomateGit => _ => _
        .OnlyWhenDynamic(() => !File.Exists(AutomateGitPy))
        .Executes(() =>
        {
            var wc = new WebClient();
            try
            {
                if(!Directory.Exists(AutomateGitPy.Parent))
                {
                    Directory.CreateDirectory(AutomateGitPy.Parent);
                }
                wc.DownloadFile(AutomateGitPyUrl, AutomateGitPy);
            }
            catch (Exception e)
            {
                Logger.Error(e);
                Logger.Error(e.InnerException);
                ControlFlow.Fail("Downloading automate-git.py has failed.");
            }
        });

    public Target Update => _ => _
        .Description("It clones both Chromium and CEF with the help of the Depot Tools. It can take a long time.")
        .DependsOn(FetchDepotTools, FetchAutomateGit)
        .OnlyWhenDynamic(() => ForceUpdate || !Directory.Exists(ChromiumSrcDir))
        .Executes(() =>
        {
            if(!Directory.Exists(CefWorkDir / "chromium_git"))
            {
                Directory.CreateDirectory(CefWorkDir / "chromium_git");
            }
            if(Directory.Exists(CefWorkDir / "chromium_git" / "cef"))
            {
                Git(
                    $"remote set-url origin {RemoteRepo}",
                    workingDirectory: CefWorkDir / "chromium_git" / "cef"
                );
            }
            if(Directory.Exists(ChromiumSrcDir / "cef"))
            {
                Git(
                    $"remote set-url origin {RemoteRepo}",
                    workingDirectory: ChromiumSrcDir / "cef"
                );
            }

            SetOfficialBuildEnvironment();
            var cmd = ProcessTasks.StartShell(
                $"python {AutomateGitPy}"
                + $" --download-dir={CefWorkDir / "chromium_git"}"
                + $" --depot-tools-dir={DepotToolsPath}"
                + $" --url={RemoteRepo}"
                + $" --branch={Revision}"
                + $" --no-distrib"
                + $" --no-build"
            );
            cmd.WaitForExit();
            if(cmd.ExitCode > 0)
            {
                ControlFlow.Fail("Updating the repositories has failed");
            }
        });

    public Target CreateProject => _ => _
        .Description($"It applies patches from CEF and generates the project files.\n    Output is {ChromiumSrcDir}\\out")
        .DependsOn(Update)
        .Executes(() =>
        {
            SetOfficialBuildEnvironment();
            SetFolderInPath(RootDirectory / "pythonbat");
            SetFolderInPath(DepotToolsPath);

            var cmdLine = @".\cef_create_projects.bat --force-clean";
            void runCmd(string cl)
            {
                var cmd = ProcessTasks.StartShell(cl, workingDirectory: ChromiumSrcDir / "cef");
                cmd.WaitForExit();
                if(cmd.ExitCode > 0)
                {
                    ControlFlow.Fail("Making CEF distributable");
                }
            }

            runCmd(cmdLine + " --sandbox");
            RestorePath();
        });

    public Target NinjaBuild => _ => _
        .Description($"Actually compiles Chromium and CEF.\n    Output is {ChromiumSrcDir}\\out")
        .DependsOn(Update, CreateProject)
        .Executes(() => {
            SetOfficialBuildEnvironment();
            SetFolderInPath(Python2Path);
            Architecture.ForEach(a =>
            {
                void runNinja(string config, bool isSandboxed)
                {
                    var sandboxedDir = isSandboxed ? "_sandbox" : "";
                    Logger.Info($"Building CEF {config} for {a}{sandboxedDir}");
                    var ninja = ProcessTasks.StartShell(
                        $@"ninja -C .\out\{config}_GN_{a}{sandboxedDir} cef{sandboxedDir} -k {MaxBuildErrors}",
                        ChromiumSrcDir
                    );
                    ninja.WaitForExit();
                    if(ninja.ExitCode > 0)
                    {
                        File.WriteAllText(
                            RootDirectory / "ninja-build.log",
                            string.Join(
                                Environment.NewLine,
                                ninja.Output.Select(lo => lo.Text)
                            )
                        );
                        ControlFlow.Fail($"Building CEF {config} has failed");
                    }
                }
                void runNinjaConfigs(bool isSandboxed)
                {
                    runNinja("Debug", isSandboxed);
                    runNinja("Release", isSandboxed);
                }

                runNinjaConfigs(false);
                runNinjaConfigs(true);
            });
            RestorePath();
        });
    
    public Target MakeDistrib => _ => _
        .Description($"Create a distributable package of CEF which Uranium can consume.\n    Output is {RootDirectory}\\distrib-output")
        .DependsOn(NinjaBuild)
        .Executes(() =>
        {
            SetOfficialBuildEnvironment();
            SetFolderInPath(Python2Path);
            SetFolderInPath(RootDirectory / "pythonbat");

            if(!Directory.Exists(RootDirectory / "distrib-output"))
            {
                Directory.CreateDirectory(RootDirectory / "distrib-output");
            }

            void runCmd(bool isSandboxed)
            {
                var sandboxedDir = isSandboxed ? "_sandbox" : "";
                var sandboxedArgs = isSandboxed ? " --sandbox" : "";
                Architecture.ForEach(a =>
                {
                    var makeDistrib = ProcessTasks.StartShell(
                        $@".\make_distrib.bat"
                        + $" --ninja-build"
                        + $" --output-dir={RootDirectory / "distrib-output"}"
                        + $" --{a}-build"
                        + sandboxedArgs,
                        ChromiumSrcDir / "cef" / "tools"
                    );
                    makeDistrib.WaitForExit();
                    if(makeDistrib.ExitCode > 0)
                    {
                        ControlFlow.Fail($"Failed making CEF distributables on {a}{sandboxedArgs}");
                    }
                });
            }

            runCmd(false);
            runCmd(true);
            RestorePath();
        });

    public void SetOfficialBuildEnvironment()
    {
        if(Architecture.Contains(ArchitectureConfig.arm64))
        {
            Environment.SetEnvironmentVariable("CEF_ENABLE_ARM64", "1");
        }
        Environment.SetEnvironmentVariable("GN_DEFINES",
            $"is_official_build={IsOfficialBuild}"
            + $" proprietary_codecs={ProprietaryCodecs}"
            + $" ffmpeg_branding={FfmpegBranding}"
            +  " use_allocator=none"
            +  " use_allocator_shim=false"
            +  " use_lld=false"
            +  " use_thin_lto=false" // https://magpcss.org/ceforum/viewtopic.php?f=6&t=18309&p=48575&hilit=sandbox#p48575
            + (Architecture.Contains(ArchitectureConfig.arm64)
                ?  " target_winuwp_family=hololens"
                +  " target_os=winuwp"
                +  " is_clang=false"
                +  " treat_warnings_as_errors=false"
                :  ""
            )
        );
        Environment.SetEnvironmentVariable("GYP_DEFINES",
            $"proprietary_codecs={(ProprietaryCodecs ? 1 : 0)}"
            + $" ffmpeg_branding={FfmpegBranding}"
        );
        Environment.SetEnvironmentVariable("GYP_GENERATORS", "ninja,msvs-ninja");
        Environment.SetEnvironmentVariable("GN_ARGUMENTS",
            $"--ide=vs{VsVersion}"
            + " --sln=cef"
            + " --filters=//cef/*"
        );
        Environment.SetEnvironmentVariable("GYP_MSVS_VERSION", VsVersion);
    }

    private string _pathEnv = null;

    private void SetFolderInPath(AbsolutePath path)
    {
        if(_pathEnv == null)
            _pathEnv = Environment.GetEnvironmentVariable("PATH");
        Environment.SetEnvironmentVariable("PATH", $"{path};" + _pathEnv);
    }
    private void RestorePath()
    {
        if(_pathEnv == null) return;
        Environment.SetEnvironmentVariable("PATH", _pathEnv);
        _pathEnv = null;
    }

    private void RunGnArgs()
    {
        var py = ProcessTasks.StartShell(
            @"python3 .\tools\gn_args.py",
            ChromiumSrcDir / "cef"
        );
        py.WaitForExit();
        if(py.ExitCode > 0)
        {
            ControlFlow.Fail("gn_args.py has failed");
        }
    }
}