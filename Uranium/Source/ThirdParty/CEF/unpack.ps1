Push-Location $PSScriptRoot
Get-ChildItem . -Directory | ForEach-Object {
    Push-Location $_
    ..\7za.exe x -aoa -r "*.7z.001" -o*
    Pop-Location
}
Pop-Location