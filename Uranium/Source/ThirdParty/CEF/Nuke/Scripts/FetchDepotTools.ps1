param (
    [string] $CefWorkDir,
    [string] $url
)

Import-Module BitsTransfer

if(-not (Test-Path "$CefWorkDir\depot_tools")) {
    "Fetching depot tools"
    Start-BitsTransfer -Source $url -Destination "$CefWorkDir\depot_tools.zip"
    Expand-Archive -Path "$CefWorkDir\depot_tools.zip" -DestinationPath "$CefWorkDir\depot_tools"
}

Push-Location "$CefWorkDir\depot_tools"
.\update_depot_tools.bat
Pop-Location