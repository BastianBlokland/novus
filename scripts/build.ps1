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

function BuildProj([int] $Threads, [string] $Dir) {
  if ([string]::IsNullOrEmpty($Dir)) {
    Fail "No build directory provided"
  }

  # If no threads number is provided default to number of cores.
  if ($Threads -le 0) {
    $Threads = $(Get-CimInstance Win32_ComputerSystem).NumberOfLogicalProcessors
  }

  # Verify the build directory exists.
  if (!(Test-Path $Dir)) {
    Fail "Build directory is missing, please run 'configure.ps1' first"
  }

  # Verify Makefile in build directory exists.
  if (!(Test-Path $Dir)) {
    Fail "No 'Makefile' found in build directory, please run 'configure.ps1' first"
  }

  # Verify that make from MinGW is present on path.
  if (!(Get-Command "mingw32-make.exe" -ErrorAction SilentlyContinue)) {
    Fail "'mingw32-make.exe' not found on path, please install MinGW and add its bin dir to path"
  }

  PInfo "Begin building using make on $Threads threads"

  Push-Location "$Dir"
  mingw32-make.exe -j "$Threads"
  Pop-Location

  PInfo "Succesfully finished build"
}

# Run build.
BuildProj $Threads $Dir
exit 0
