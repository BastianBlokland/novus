<#
.SYNOPSIS
  Test the project as configured.
.DESCRIPTION
  Test the project as configured using ctest.
  Dependencies: CMake atleast version 3.15: https://cmake.org
.PARAMETER Threads
  Default: -1 (As many as there are cores in the machine)
  Number of threads to use for testing.
.PARAMETER Dir
  Default: build
  Build directory.
.PARAMETER Filter
  Default: '.+'
  Regex pattern to select the tests to run.
#>
[cmdletbinding()]
param(
  [int]$Threads = -1,
  [string]$Dir = "build",
  [string]$Filter = ".+"
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

function TestProj([int] $threads, [string] $dir) {
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

  # Verify Makefile in build directory exists.
  if (!(Test-Path $dir)) {
    Fail "No 'Makefile' found in build directory, please run 'configure.ps1' first"
  }

  # Verify that ctest is present on path.
  if (!(Get-Command "ctest.exe" -ErrorAction SilentlyContinue)) {
    Fail "'ctest.exe' not found on path, please install cmake (https://cmake.org)"
  }

  PInfo "Begin testing using ctest (filter: ${filter}) on $threads threads"

  Push-Location "$dir"
  & ctest.exe --tests-regex "$filter" --output-on-failure -j "$threads"
  $ctestResult = $LASTEXITCODE
  Pop-Location

  if ($ctestResult -ne 0) {
    Fail "Test failed"
  }

  PInfo "Successfully finished testing"
}

TestProj $Threads $Dir
exit 0
