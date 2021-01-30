#!/usr/bin/env bash
set -e -o pipefail

# --------------------------------------------------------------------------------------------------
# Test the project as configured.
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

testProj()
{
  local threads="${1}"
  local dir="${2}"
  local filter="${3}"
  local coverageExecutable="${4}"
  local coverageIgnoreRegex="${5}"
  local coverageReportPath="${6}"
  local rawCoverageDir="$(pwd)/${dir}/tests/test_rawcoverage/"
  local indexedCoveragePath="$(pwd)/${dir}/tests/test_coverage.profdata"

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
  hasCommand ctest || fail "'ctest' not found on path, it is required"

  info "Begin testing using ctest (filter: ${filter}) on ${threads} threads"

  # Remove previous coverage outputs (if they exist)
  rm -rf "${coverageReportPath}" "${rawCoverageDir}" "${indexedCoveragePath}"

  # Run all tests using ctest.
  (
    cd "${dir}" && LLVM_PROFILE_FILE="${rawCoverageDir}/%p.profraw" \
    ctest --tests-regex "${filter}" --output-on-failure -j "${threads}"
  )

  info "Successfully finished testing"

  if test -d "${rawCoverageDir}"
  then
    info "Found raw coverage data, indexing..."
    hasCommand llvm-profdata || fail "'llvm-profdata' not found on path, it is required to create coverage data"
    hasCommand llvm-cov || fail "'llvm-cov' not found on path, it is required to create coverage reports"

    llvm-profdata merge -sparse "${rawCoverageDir}"/* -o "${indexedCoveragePath}"
    info "Generating coverage report"
    llvm-cov export "${coverageExecutable}" \
      -format="lcov" \
      -instr-profile="${indexedCoveragePath}" \
      -ignore-filename-regex="${coverageIgnoreRegex}" > "${coverageReportPath}"
    info "Saved coverage report: ${coverageReportPath}"
  fi
}

printUsage()
{
  echo "Options:"
  echo "-h,--help               Print this usage information"
  echo "-t,--threads            Number of threads to use, defaults to number of processors"
  echo "-d,--dir                Build directory, default: 'build'"
  echo "-f,--filter             Regex pattern to select the tests to run, default: '.+'"
  echo "--coverage-executable   Path to exeutable to collect coverage for, default: 'bin/novtests'"
  echo "--coverage-ignore-regex Skip coverage for files that match this regex, default: 'tests/'"
  echo "--coverage-report-path  Output path for the coverage file, default: 'test_coverage.lcov'"
}

# Defaults.
numThreads="$(numProcessors)"
buildDir="build"
filter=".+"
coverageExecutable="bin/novtests"
coverageIgnoreRegex="tests/"
coverageReportPath="test_coverage.lcov"

# Parse options.
while [[ $# -gt 0 ]]
do
  case "${1}" in
    -h|--help)
      echo "Novus -- Test"
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
    -f|--filter)
      filter="${2}"
      shift 2
      ;;
    --coverage-executable)
      coverageExecutable="${2}"
      shift 2
      ;;
    --coverage-ignore-regex)
      coverageIgnoreRegex="${2}"
      shift 2
      ;;
    --coverage-report-path)
      coverageReportPath="${2}"
      shift 2
      ;;
    *)
      error "Unknown option '${1}'"
      printUsage
      exit 1
      ;;
  esac
done

testProj "${numThreads}" \
         "${buildDir}" \
         "${filter}" \
         "${coverageExecutable}" \
         "${coverageIgnoreRegex}" \
         "${coverageReportPath}"
exit 0
