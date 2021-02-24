#!/usr/bin/env bash
set -e -o pipefail
trap interupt SIGINT

# --------------------------------------------------------------------------------------------------
# Watch novus source files for changes and automatically compile them.
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

interupt()
{
  info "Stopped watching"
}

watchFiles()
{
  [ -d "${1}" ] || fail "Provide the directory to watch as arg 1"
  [ -n "${2}" ] || fail "Provide the file extension to watch as arg 2"
  [ -n "${3}" ] || fail "Provide command to execute for changed files as arg 3"

  local dir="${1}"
  local extension="${2}"

  hasCommand inotifywait || fail "No 'inotifywait' command found on path, install 'inotify-tools'"

  info "Start watching '${watchDir}' for changes to '${extension}' files"

  inotifywait --monitor --recursive --quiet --event close_write --format '%w%f' "${dir}" |
    while read -r file
    do
      if [[ "${file}" == *."${extension}" ]]
      then
        # Execute command given to this function.
        "${@:3}" "${file}"
      fi
    done
}

compileNsFile()
{
  [ -x "${1}" ] || fail "Provide the novus compiler binary path as arg 1"
  [ -f "${2}" ] || fail "Provide the file to compile as arg 2"

  local compilerBinPath="${1}"
  local nsFilePath="${2}"

  clear
  info "File change detected: '${nsFilePath}'. Compiling:"

  # Compile the file.
  # note: ignore exit-code because we want to keep listening even if a compilation failed.
  "${compilerBinPath}" "${nsFilePath}" --validate-only || true

  info "Compilation complete. Watching for file changes..."
}

# Verify env args.
[ -d "${1}" ] || fail "Provide directory to watch as arg 1"
[ -x "${2}" ] || fail "Provide novus compiler binary path as arg 2"

readonly watchDir="${1}"
readonly compilerBin="${2}"

# Watch dir for ns file changes and auto compile them.
watchFiles "${watchDir}" "ns" compileNsFile "${compilerBin}"
exit 0
