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

configureProj()
{
  local type="${1}"
  local dir="${2}"

  # Verify the type is supported.
  case "${type}" in
    Release|Debug)
      ;;
    *)
      fail "Unsupported build-type: '${type}'"
      ;;
  esac

  # Create target directory if it doesn't exist yet.
  test -d "${dir}" || mkdir -p "${dir}"

  # Verify that cmake is present on path.
  hasCommand cmake || fail "'cmake' not found on path, it is required"

  info "Begin configuring build directory '${dir}' using cmake"

  cmake -B "${dir}" -DCMAKE_BUILD_TYPE="${type}"

  info "Succesfully configured build directory '${dir}'"
}

printUsage()
{
  echo "Novus -- Configure"
  echo "Options:"
  echo "-h,--help     Print this usage information"
  echo "-t,--type     Build type, options: Debug, Release (default)"
  echo "-d,--dir      Build directory, default: 'build'"
}

# Defaults.
buildType="Release"
buildDir="build"

# Parse options.
while [[ $# -gt 0 ]]
do
  case "${1}" in
    -h|--help)
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
    *)
      fail "Unknown option '${1}'"
      ;;
  esac
done

# Run configuration.
configureProj "${buildType}" "${buildDir}"
exit 0
