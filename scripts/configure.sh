#!/usr/bin/env bash
set -e -o pipefail

# --------------------------------------------------------------------------------------------------
# Configure the project for building.
# --------------------------------------------------------------------------------------------------

info()
{
  echo "${1}"
}

error()
{
  echo "ERROR: ${1}" >&2
}

fail()
{
  error "${1}"
  exit 1
}

hasCommand()
{
  [ -x "$(command -v "${1}")" ]
}

verifyBuildTypeOption()
{
  case "${1}" in
    Debug|Release)
      ;;
    *)
      fail "Unsupported build-type: '${1}'"
      ;;
  esac
}

verifyBoolOption()
{
  case "${1}" in
    On|Off)
      ;;
    *)
      fail "Unsupported bool value: '${1}'"
      ;;
  esac
}

configureProj()
{
  local type="${1}"
  local dir="${2}"
  local testsMode="${3}"
  local lintMode="${4}"
  local sanitiseMode="${5}"
  local coverageMode="${6}"

  verifyBuildTypeOption "${type}"
  verifyBoolOption "${testsMode}"
  verifyBoolOption "${lintMode}"
  verifyBoolOption "${sanitiseMode}"
  verifyBoolOption "${coverageMode}"

  # Create target directory if it doesn't exist yet.
  test -d "${dir}" || mkdir -p "${dir}"

  # Verify that cmake is present on path.
  hasCommand cmake || fail "'cmake' not found on path, it is required"

  info "Begin configuring build directory '${dir}' using cmake"

  cmake -B "${dir}" \
    -G "Unix Makefiles" \
    -DCMAKE_BUILD_TYPE="${type}" \
    -DBUILD_TESTING="${testsMode}" \
    -DLINTING="${lintMode}" \
    -DSANITISE="${sanitiseMode}" \
    -DCOVERAGE="${coverageMode}"

  info "Successfully configured build directory '${dir}'"
}

printUsage()
{
  echo "Options:"
  echo "-h,--help     Print this usage information"
  echo "-t,--type     Build type, options: Debug, Release (default)"
  echo "-d,--dir      Build directory, default: 'build'"
  echo "--tests       Include compiler and runtime tests"
  echo "--lint        Enable source linter"
  echo "--sanitise    Should santiser instrumentation be included in targets"
  echo "--coverage    Should coverage instrumentation be included in targets"
}

# Defaults.
buildType="Release"
buildDir="build"
testsMode="Off"
lintMode="Off"
sanitiseMode="Off"
coverageMode="Off"

# Parse options.
while [[ $# -gt 0 ]]
do
  case "${1}" in
    -h|--help)
      echo "Novus -- Configure"
      printUsage
      exit 0
      ;;
    -t|--type)
      buildType="${2}"
      shift 2
      ;;
    -d|--dir)
      buildDir="${2}"
      shift 2
      ;;
    --tests)
      testsMode="On"
      shift 1
      ;;
    --lint)
      lintMode="On"
      shift 1
      ;;
    --sanitise)
      sanitiseMode="On"
      shift 1
      ;;
    --coverage)
      coverageMode="On"
      shift 1
      ;;
    *)
      error "Unknown option '${1}'"
      printUsage
      exit 1
      ;;
  esac
done

# Run configuration.
configureProj \
  "${buildType}" \
  "${buildDir}" \
  "${testsMode}" \
  "${lintMode}" \
  "${sanitiseMode}" \
  "${coverageMode}"
exit 0
