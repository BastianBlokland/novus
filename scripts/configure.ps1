<#
.SYNOPSIS
  Configure the project for building.
.DESCRIPTION
  Generates the build system using cmake.
  Dependencies: CMake atleast version 3.15: https://cmake.org
.PARAMETER Type
  Default: Release
  Build type. Possible values:
  - Debug
  - Release
.PARAMETER Gen
  Default: MinGW
  Build system to generate. Possible values:
  - MinGW
  - VS2019
  - VS2019-Clang
.PARAMETER Dir
  Default: build
  Build directory.
.PARAMETER Tests
  Include compiler and runtime tests.
.PARAMETER Fuzz
  Include fuzz targets
.PARAMETER Lint
  Enable source linter.
.PARAMETER Sanitize
  Should santiser instrumentation be included in targets.
.PARAMETER Coverage
  Should coverage instrumentation be included in targets.
#>
[cmdletbinding()]
param(
  [ValidateSet("Debug", "Release", IgnoreCase = $true)]
  [string]$Type = "Release",
  [ValidateSet("MinGW", "VS2019", "VS2019-Clang", IgnoreCase = $true)]
  [string]$Gen = "MinGW",
  [string]$Dir = "build",
  [switch]$Tests,
  [switch]$Fuzz,
  [switch]$Lint,
  [switch]$Sanitize,
  [switch]$Coverage
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

function MapToCMakeGen([string] $gen) {
  switch ($gen) {
    "MinGW" { "MinGW Makefiles"; break }
    "VS2019" { "Visual Studio 16 2019"; break }
    "VS2019-Clang" { "Visual Studio 16 2019"; break }
    default { Fail "Unsupported generator" }
  }
}

function GetToolsetArg([string] $gen) {
  switch ($gen) {
    "MinGW" { ""; break }
    "VS2019" { ""; break }
    "VS2019-Clang" { "-TClangCL"; break }
    default { Fail "Unsupported generator" }
  }
}

function ConfigureProj(
  [string] $type,
  [string] $gen,
  [string] $dir,
  [bool] $tests,
  [bool] $fuzz,
  [bool] $lint,
  [bool] $sanitize,
  [bool] $coverage) {

  if ([string]::IsNullOrEmpty($dir)) {
    Fail "No target directory provided"
  }

  # Create target directory if it doesn't exist yet.
  if (!(Test-Path $dir)) {
    New-Item -ItemType Directory -Path $dir
  }

  # Verify that cmake is present on path.
  if (!(Get-Command "cmake.exe" -ErrorAction SilentlyContinue)) {
    Fail "'cmake.exe' not found on path, it is required"
  }

  PInfo "Begin configuring build directory '$dir' using cmake"

  & cmake.exe -B "$dir" `
    -G "$(MapToCMakeGen $gen)" `
    -DCMAKE_BUILD_TYPE="$type" `
    -DBUILD_TESTING="$($tests ? "On" : "Off")" `
    -DBUILD_FUZZING="$($fuzz ? "On" : "Off")" `
    -DLINTING="$($lint ? "On" : "Off")" `
    -DSANITIZE="$($sanitize ? "On" : "Off")" `
    -DCOVERAGE="$($coverage ? "On" : "Off")" `
    -DCMAKE_POLICY_VERSION_MINIMUM="3.15" `
    "$(GetToolsetArg $gen)"

  if ($LASTEXITCODE -ne 0) {
    Fail "Configure failed"
  }

  # Save a file to remember what build-type is this.
  Set-Content -Path "$dir/build_type" -Value "$type"

  PInfo "Successfully configured build directory '$dir'"
}

ConfigureProj $Type $Gen $Dir $Tests $Fuzz $Lint $Sanitize $Coverage
exit 0
