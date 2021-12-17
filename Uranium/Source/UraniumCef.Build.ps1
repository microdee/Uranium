# ******************** (C) Copyright 2021 realworld one GmbH & Co. KG All Rights Reserved. ********************

param (
	[string] $CefVersionIn = "85.0.4183.121.ST",
	[string] $BuildConfigIn = "Release",
	[string] $BuildPlatform = "Win64",
	[bool] $IsSandboxed = $false
)

$prevLoc = $(Get-Location)
$outDir = "$prevLoc\out\ue4\$BuildPlatform-$BuildConfigIn"

if(-not (Test-Path $outDir))
{
	New-Item -ItemType Directory $outDir
}

Push-Location $outDir

$cplatform = "x64"

cmake.exe `
	-G "Visual Studio 16 2019" -A $cplatform `
	-DCMAKE_BUILD_TYPE:STRING="$BuildConfigIn" `
	-DCMAKE_INSTALL_PREFIX:PATH="$outDir" `
	-DURCP_CEF_VERSION:STRING="$CefVersionIn" `
	-DURCP_PLATFORM:STRING="$BuildPlatform" `
	-DCEF_RUNTIME_LIBRARY_FLAG:STRING="/MD" `
	-DUSE_SANDBOX:BOOL="$IsSandboxed" `
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