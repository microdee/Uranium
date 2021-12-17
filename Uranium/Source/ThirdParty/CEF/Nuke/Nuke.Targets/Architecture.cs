using System;
using System.ComponentModel;
using System.Linq;
using Nuke.Common.Tooling;

[TypeConverter(typeof(TypeConverter<ArchitectureConfig>))]
public class ArchitectureConfig : Enumeration
{
    public static ArchitectureConfig x64 = new() { Value = nameof(x64) };
    public static ArchitectureConfig arm64 = new() { Value = nameof(arm64) };

    public static implicit operator string(ArchitectureConfig configuration)
    {
        return configuration.Value;
    }
}