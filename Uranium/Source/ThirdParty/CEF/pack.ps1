function Test-Platform {
    param ( [string] $name )

    [string[]] $platforms = "Win64"

    return $platforms.Where({$name.Contains($_)}, 'First').Count -gt 0
}

Get-ChildItem . -Directory `
    | Where-Object { Test-Platform $_.Name } `
    | ForEach-Object {
        Get-ChildItem $_ -File -Filter "*.7z.*" | Remove-Item -Force -Verbose
        Get-ChildItem $_ -Directory | ForEach-Object {
            .\7za.exe a -y -v100m -t7z -mx9 -r "$($_.FullName).7z" "$($_.FullName)\*"
    }
}