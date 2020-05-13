<#
.SYNOPSIS
  Test the project as configured.
.DESCRIPTION
  Test the project as configured using ctest.
  Dependencies: CMake atleast version 3.15: https://cmake.org
.PARAMETER Dir
  Default: build
  Build directory.
#>
[cmdletbinding()]
param(
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

function TestProj([string] $dir) {
  if ([string]::IsNullOrEmpty($dir)) {
    Fail "No build directory provided"
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

  PInfo "Begin testing using ctest"

  Push-Location "$dir"
  ctest.exe --output-on-failure
  $ctestResult = $?
  Pop-Location

  if ($ctestResult -ne 0) {
    Fail "Test failed"
  }

  PInfo "Succesfully finished testing"
}

# Run test.
TestProj $Dir
exit 0
