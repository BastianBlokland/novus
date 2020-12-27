#!/usr/bin/env bash
set -e -o pipefail

# --------------------------------------------------------------------------------------------------
# Upload test coverage to codecov.io.
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

uploadCoverage()
{
  local uploadToken="${1}"
  local coverageFile="${2}"

  test -f "${coverageFile}" || fail "No coverage file found at path: '${coverageFile}'"

  info "Uploading coverage to codecov.io"

  bash <(curl -s https://codecov.io/bash) \
    -t "${uploadToken}" \
    -f "${coverageFile}" \
    -F unittests,novstd \
    -Z

  info "Finished uploading coverage"
}

printUsage()
{
  echo "Options:"
  echo "-h,--help   Print this usage information"
  echo "-t,--token  CodeCov upload token (required)"
  echo "-f,--file   Path to the coverage report file, default: 'test_coverage.lcov'"
}

# Defaults.
uploadToken=""
coverageFile="test_coverage.lcov"

# Parse options.
while [[ $# -gt 0 ]]
do
  case "${1}" in
    -h|--help)
      echo "Novus -- Upload Coverage"
      printUsage
      exit 0
      ;;
    -t|--token)
      uploadToken="${2}"
      shift 2
      ;;
    -f|--file)
      coverageFile="${2}"
      shift 2
      ;;
    *)
      error "Unknown option '${1}'"
      printUsage
      exit 1
      ;;
  esac
done

! test -z "${uploadToken}" || fail "Provide a CodeCov upload token using the '--token' parameter"

uploadCoverage "${uploadToken}" "${coverageFile}"
exit 0
