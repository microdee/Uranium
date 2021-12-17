using System;
using System.IO;
using System.Net;
using System.Linq;
using System.Text.RegularExpressions;
using System.Collections.Generic;
using System.Collections.Concurrent;
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

partial class Build : NukeBuild
{
    [LocalExecutable(@"C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Tools\MSVC\14.29.30133\bin\HostX86\x86\dumpbin.exe")]
    public Tool DumpBin;

    public AbsolutePath System32 => (AbsolutePath) @"C:\Windows\System32";
    public AbsolutePath SeekRoot => ChromiumSrcDir;

    public AbsolutePath ReferenceBinary => RootDirectory
        / "distrib-output"
        / "cef_binary_94.0.0-Unknown.0+gUnknown+chromium-94.0.4606.12_windowsarm64"
        / "Release" / "chrome_elf.dll";

    private IEnumerable<string> GetExportsRaw(string systemDll)
    {
        var dllPath = System32 / systemDll;
        if(!FileExists(dllPath))
        {
            Warn($"{dllPath} doesn't exist");
            yield break;
        }

        bool isExportsSection = false;
        bool expectExportsSection = true;
        int namePosition = 0;
        foreach(var line in DumpBin.Invoke(arguments: $"/EXPORTS /NOLOGO \"{dllPath}\""))
        {
            if(isExportsSection)
            {
                if(string.IsNullOrWhiteSpace(line.Text)) continue;
                if(line.Text.Contains("    "))
                {
                    var export = line.Text.Substring(namePosition).Trim();
                    yield return export;
                    if(export.EndsWith("W") || export.EndsWith("A"))
                    {
                        yield return export.Substring(0, export.Length - 1);
                    }
                }
                else
                {
                    isExportsSection = false;
                }
            }
            if (expectExportsSection
                && line.Text.Contains("ordinal")
                && line.Text.Contains("hint")
                && line.Text.Contains("RVA")
                && line.Text.Contains("name")
            ) {
                isExportsSection = true;
                expectExportsSection = false;
                namePosition = line.Text.IndexOf("name");
            }
        }
    }

    private IEnumerable<string> GetExports(string systemDll) => GetExportsRaw(systemDll)
        .Where(e => !e.Contains("[NONAME]"))
        .Select(e => Regex.Match(e, @"(?<name>\w+)(?:\s\(forwarded|$)").Groups["name"].Value)
        .Where(e => !e.EndsWith("64"))
        .Distinct()
        //.Where(e => e.Length > 5)
        ;

    struct ResultLine
    {
        public string LineText;
        public int LineNumber;
        public int Index;
    }

    private string CurrentLine(string text, int index)
    {
        var end = text.IndexOf('\n', index);
        if(end <= 0) end = text.Length;
        while(text[index] != '\n' && index > 0)
        {
            index--;
        }
        var lineLength = end - index;
        return text.Substring(index, lineLength).Trim();
    }

    private ConcurrentDictionary<AbsolutePath, List<ResultLine>> Results = new();

    private void SearchSources(AbsolutePath path, IEnumerable<string> exports)
    {
        Normal($"folder: {path}");
        
        void EnumSources(string glob)
        {
            Directory.EnumerateFiles(path, glob)
                .AsParallel()
                .ForAll(src => {
                    try
                    {
                        if(new FileInfo(src).Length > (8 * 1024 * 1024))
                        {
                            return;
                        }
                        var text = File.ReadAllText(src);
                        var lines = text.Split("\n").Select(l => l.Trim()).ToArray();
                        foreach(var f in exports)
                        {
                            var matches = Regex.Matches(
                                text, $@"(?:\W::{f}\(|[^:.>a-zA-Z0-9_]{f}\()",
                                RegexOptions.Multiline
                            );
                            for(int i=0; i<matches.Count; i++)
                            {
                                var match = matches[i];
                                var lineNum = text.Substring(0, match.Index + 2).Count(c => c == '\n');
                                
                                Results.GetOrAdd((AbsolutePath)src, k => new()).Add(new() {
                                    //LineText = text.Substring(match.Index, match.Length),
                                    //LineText = CurrentLine(text, match.Index),
                                    LineText = lines[lineNum],
                                    LineNumber = lineNum + 1,
                                    Index = match.Index
                                });
                            }
                        }
                    }
                    catch(Exception e)
                    {
                        Warn("Something went wrong while processing a source file:");
                        Warn(e.Message);
                    }
                });
        }

        EnumSources("*.cc");
        EnumSources("*.h");
        
        Directory.EnumerateDirectories(path)
            .Where(d => !Path.GetFileName(d).StartsWith('.'))
            .AsParallel()
            .ForAll(subDir => SearchSources((AbsolutePath)subDir, exports));
    }

    public Target SeekWinapi => _ => _
        .Executes(() =>
        {
            bool isDelayLoadedDllSection = false;
            List<string> delayLoadedDlls = new();
            
            foreach(var line in DumpBin.Invoke(arguments: $"/DEPENDENTS /NOLOGO \"{ReferenceBinary}\""))
            {
                if(isDelayLoadedDllSection)
                {
                    if(string.IsNullOrWhiteSpace(line.Text)) continue;
                    if(line.Text.Contains("    "))
                    {
                        var delayLoadedDep = line.Text.Trim();
                        Info($"got dependency {delayLoadedDep}");
                        delayLoadedDlls.Add(delayLoadedDep);
                    }
                    else
                    {
                        isDelayLoadedDllSection = false;
                    }
                }
                if(line.Text.Contains("Image has the following delay load dependencies"))
                {
                    isDelayLoadedDllSection = true;
                }
            }

            var exports = (
                from dldll in delayLoadedDlls
                    from exp in GetExports(dldll)
                    select exp
            ).ToArray();

            foreach(var export in exports)
            {
                Info($"export: {export}");
            }

            Success("Sources containing designated functions:");

            SearchSources(SeekRoot, exports);
            if(Results.Count == 0)
            {
                Success("None of the seeked functions were found in target source code.");
            }
            foreach(var kvp in Results)
            {
                Info($"{kvp.Key}:");
                foreach(var line in kvp.Value)
                {
                    Normal($"    {line.LineNumber}:  {line.LineText}");
                }
            }
        });
}