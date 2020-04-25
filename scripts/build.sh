#!/usr/bin/env bash
set -e -o pipefail

# --------------------------------------------------------------------------------------------------
# Build the project as configured.
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

isMacos()
{
  [[ "$(uname -s)" = Darwin* ]]
}

hasCommand()
{
  [ -x "$(command -v "${1}")" ]
}

numProcessors()
{
  if hasCommand nproc
  then
    nproc
  elif isMacos
  then
    sysctl -n hw.ncpu
  else
    # Default to 1 processor.
    echo "1"
  fi
}

buildProj()
{
  local threads="${1}"
  local dir="${2}"

  # Verify threads is a number.
  case "${threads}" in
    ''|*[!0-9]*)
      fail "Invalid number of threads '${threads}', has to be number"
      ;;
  esac

  # Verify build directory exists.
  test -d "${dir}" || \
    fail "Build directory is missing, please run 'configure.sh' first"

  # Verify Makefile in build directory exists.
  test -f "${dir}/Makefile" || \
    fail "No 'Makefile' found in build directory, please run 'configure.sh' first"

  # Verify that make is present on path.
  hasCommand make || fail "'make' not found on path, it is required"

  info "Begin building using make on ${threads} threads"

  cd "${dir}" && make -j "${threads}"

  info "Succesfully finished build"
}

printUsage()
{
  echo "Options:"
  echo "-h,--help       Print this usage information"
  echo "-t,--threads    Number of threads to use, defaults to number of processors"
  echo "-d,--dir        Build directory, default: 'build'"
}

# Defaults.
numThreads="$(numProcessors)"
buildDir="build"

# Parse options.
while [[ $# -gt 0 ]]
do
  case "${1}" in
    -h|--help)
      echo "Novus -- Build"
      printUsage
      exit 0
      ;;
    -t|--threads)
      numThreads="${2}"
      shift 2
      ;;
    -d|--dir)
      buildDir="${2}"
      shift 2
      ;;
    *)
      error "Unknown option '${1}'"
      printUsage
      exit 1
      ;;
  esac
done

# Run build.
buildProj "${numThreads}" "${buildDir}"
exit 0
