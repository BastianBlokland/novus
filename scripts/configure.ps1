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
.PARAMETER Dir
  Default: build
  Build directory.
.PARAMETER Tests
  Include compiler and runtime tests.
.PARAMETER Lint
  Enable source linter.
#>
[cmdletbinding()]
param(
  [ValidateSet("Debug", "Release", IgnoreCase = $true)]
  [string]$Type = "Release",
  [ValidateSet("MinGW", "VS2019", IgnoreCase = $true)]
  [string]$Gen = "MinGW",
  [string]$Dir = "build",
  [switch]$Tests,
  [switch]$Lint
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

function MapToCMakeGen([string] $Gen) {
  switch ($Gen) {
    "MinGW" { "MinGW Makefiles"; break }
    "VS2019" { "Visual Studio 16 2019"; break }
    default { Fail "Unsupported generator" }
  }
}

function ConfigureProj([string] $Type, [string] $Gen, [string] $Dir, [bool] $Tests, [bool] $Lint) {
  if ([string]::IsNullOrEmpty($Dir)) {
    Fail "No target directory provided"
  }

  # Create target directory if it doesn't exist yet.
  if (!(Test-Path $Dir)) {
    New-Item -ItemType Directory -Path $Dir
  }

  # Verify that cmake is present on path.
  if (!(Get-Command "cmake.exe" -ErrorAction SilentlyContinue)) {
    Fail "'cmake.exe' not found on path, it is required"
  }

  PInfo "Begin configuring build directory '$Dir' using cmake"

  cmake.exe -B "$Dir" `
    -G "$(MapToCMakeGen $Gen)" `
    -DCMAKE_BUILD_TYPE="$Type" `
    -DBUILD_TESTING="$($Tests ? "On" : "Off")" `
    -DLINTING="$($Lint ? "On" : "Off")"

  PInfo "Succesfully configured build directory '$Dir'"
}

# Run configuration.
ConfigureProj $Type $Gen $Dir $Tests $Lint
exit 0
