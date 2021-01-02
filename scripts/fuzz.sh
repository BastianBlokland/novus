#!/usr/bin/env bash
set -e -o pipefail

# --------------------------------------------------------------------------------------------------
# Run a fuzz executable.
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

fuzz()
{
  local executablePath="${1}"
  local corpusPath="${2}"
  local duration="${3}"
  local numJobs="${4}"
  local rawCoverageDir="$(pwd)/fuzz_rawcoverage/"
  local indexedCoveragePath="$(pwd)/fuzz_coverage.profdata"

  test -f "${executablePath}" || fail "No fuzz executable found at path '${executablePath}'"

  if test -z "${corpusPath}"
  then
    local corpusPath="$(pwd)/fuzz/corpus/$(basename ${executablePath})"
  fi

  mkdir -p "${corpusPath}"                            # Create the corpus directory if its missing.
  rm -rf "${rawCoverageDir}" "${indexedCoveragePath}" # Remove previous coverage outputs (if they exist).
  rm -f fuzz-*.log                                    # Remove any old fuzz logs.

  info "Begin fuzzing for '${duration}' seconds"

  # Run fuzz executable
  LLVM_PROFILE_FILE="${rawCoverageDir}/%p.profraw" \
  "${executablePath}" "${corpusPath}" \
    -max_total_time=${duration} \
    -jobs=${numJobs} \
    -workers=${numJobs} \
    -print_final_stats=1 \
    -print_pcs=1

  info "Finished fuzzing"

  if test -d ${rawCoverageDir}
  then
    info "Found raw coverage data, indexing..."
    hasCommand llvm-profdata || fail "'llvm-profdata' not found on path, it is required to create coverage data"
    hasCommand llvm-cov || fail "'llvm-cov' not found on path, it is required to create coverage reports"

    llvm-profdata merge -sparse "${rawCoverageDir}"/* -o "${indexedCoveragePath}"
    info "Coverage data indexed, printing report:"
    llvm-cov report "${executablePath}" "-instr-profile=${indexedCoveragePath}"
  fi
}

printUsage()
{
  echo "Options:"
  echo "-h,--help       Print this usage information"
  echo "-p,--path       Path to fuzz executable"
  echo "-c,--corpus     Path to the corpus to use, default: 'fuzz/corpus/{EXE_NAME}'"
  echo "-d,--duration   Duration to run the fuzzer in seconds, default: '60'"
  echo "-j,--jobs       Number of fuzz jobs to run, default: '2'"
}

# Defaults.
duration="60"
numJob="2"

# Parse options.
while [[ $# -gt 0 ]]
do
  case "${1}" in
    -h|--help)
      echo "Novus -- Fuzz"
      printUsage
      exit 0
      ;;
    -p|--path)
      executablePath="${2}"
      shift 2
      ;;
    -c|--corpus)
      corpusPath="${2}"
      shift 2
      ;;
    -d|--duration)
      duration="${2}"
      shift 2
      ;;
    -j|--jobs)
      numJob="${2}"
      shift 2
      ;;
    *)
      error "Unknown option '${1}'"
      printUsage
      exit 1
      ;;
  esac
done

! test -z "${executablePath}" || fail "Provide a path to an fuzz executable using the '--path' parameter"

fuzz "${executablePath}" "${corpusPath}" "${duration}" "${numJob}"
exit 0
