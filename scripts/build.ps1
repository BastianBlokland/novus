<#
.SYNOPSIS
  Build the project as configured.
.DESCRIPTION
  Build the project as configured using MinGW make.
  Dependencies: MinGW (or MinGW-w64).
  Note: either 7.x or 9.x as MinGW 8.x ships with a broken c++ std::filesystem api.
.PARAMETER Threads
  Default: -1 (As many as there are cores in the machine)
  Number of threads to use for building.
.PARAMETER Dir
  Default: build
  Build directory.
#>
[cmdletbinding()]
param(
  [int]$Threads = -1,
  [string]$Dir = "build"
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

function PInfo($str) {
  Write-Host "$str"
}

function PError($str) {
  Write-Error "$str"
}

function Fail($str) {
  PError $str
  exit 1
}

function BuildProjMake([int] $threads, [string] $dir) {
  # Verify that make from MinGW is present on path.
  if (!(Get-Command "mingw32-make.exe" -ErrorAction SilentlyContinue)) {
    Fail "'mingw32-make.exe' not found on path, please install MinGW and add its bin dir to path"
  }

  PInfo "Begin building using make on $threads threads"

  Push-Location "$dir"
  mingw32-make.exe -j "$threads"
  $makeResult = $LASTEXITCODE
  Pop-Location

  if ($makeResult -ne 0) {
    Fail "Build failed"
  }

  PInfo "Succesfully finished build"
}

function BuildProjMSBuild([int] $threads, [string] $dir) {
  # Verify that vswhere is present on path.
  if (!(Get-Command "vswhere.exe" -ErrorAction SilentlyContinue)) {
    Fail "'vswhere.exe' not found on path, please install it and add its bin dir to path"
  }

  # Find msbuild using vswhere.
  $msbuildPath = vswhere.exe `
    -latest `
    -requires Microsoft.Component.MSBuild `
    -find MSBuild\**\Bin\MSBuild.exe | select-object -first 1

  PInfo "Found MSBuild.exe: $msbuildPath"
  PInfo "Begin building using MSBuild on $Threads threads"

  Push-Location "$dir"
  & "$msbuildPath" -noLogo -maxCpuCount:$threads Novus.sln -p:Configuration=Release
  $msbuildResult = $LASTEXITCODE
  Pop-Location

  # Msbuild creates extra empty directores in the output directory, we remove those as it
  # just clutters up the output.
  Remove-Item "$dir\..\bin\Debug" -Force -ErrorAction Ignore
  Remove-Item "$dir\..\bin\Release" -Force -ErrorAction Ignore

  if ($msbuildResult -ne 0) {
    Fail "Build failed"
  }

  PInfo "Succesfully finished build"
}

function BuildProj([int] $threads, [string] $dir) {
  if ([string]::IsNullOrEmpty($dir)) {
    Fail "No build directory provided"
  }

  # If no threads number is provided default to number of cores.
  if ($threads -le 0) {
    $threads = $(Get-CimInstance Win32_ComputerSystem).NumberOfLogicalProcessors
  }

  # Verify the build directory exists.
  if (!(Test-Path $dir)) {
    Fail "Build directory is missing, please run 'configure.ps1' first"
  }

  if (Test-Path "$dir\Makefile") {
    BuildProjMake $threads $dir
  }
  elseif (Test-Path "$dir\Novus.sln") {
    BuildProjMSBuild $threads $dir
  }
  else {
    Fail "Invalid build directory, please run 'configure.ps1' first"
  }
}

# Run build.
BuildProj $Threads $Dir
exit 0
