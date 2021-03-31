
param (
    [string] $CefVersionIn = "85.0.4183.121.ST",
    [string] $BuildConfigIn = "Debug"
)

$prevLoc = $(Get-Location)
$outDir = "$prevLoc\out\ue4\x64-$BuildConfigIn"

if(-not (Test-Path $outDir))
{
    New-Item -ItemType Directory $outDir
}

Push-Location $outDir

cmake.exe `
    -G "Visual Studio 16 2019" -A x64 `
    -DCMAKE_BUILD_TYPE:STRING="$BuildConfigIn" `
    -DCMAKE_INSTALL_PREFIX:PATH="$prevLoc\out\ue4\x64-$BuildConfigIn" `
    -DURCP_CEF_VERSION:STRING="$CefVersionIn" `
    -DCEF_RUNTIME_LIBRARY_FLAG:STRING="/MD" `
    -DUSE_SANDBOX:BOOL="False" `
    "$prevLoc"

cmake.exe `
    --build "$(Get-Location)" `
    --target UraniumCefProcess `
    --config $BuildConfigIn

if(Test-Path ".\$BuildConfigIn")
{
    Get-ChildItem ".\$BuildConfigIn" -File | ForEach-Object {
        Copy-Item $_.FullName .\
    }
}

if(Test-Path ".\libcef_dll_wrapper\$BuildConfigIn")
{
    Get-ChildItem ".\libcef_dll_wrapper\$BuildConfigIn" -File | ForEach-Object {
        Copy-Item $_.FullName .\
    }
}

Pop-Location