#!/usr/bin/env bash
set -e -o pipefail

# --------------------------------------------------------------------------------------------------
# Cross platform wrapper around the linux nproc executable (which returns the number of processors).
# --------------------------------------------------------------------------------------------------

isMacos()
{
  [[ "$(uname -s)" = Darwin* ]]
}

hasCommand()
{
  [ -x "$(command -v "${1}")" ]
}

if hasCommand nproc
then
  nproc
  exit 0
fi

if isMacos
then
  sysctl -n hw.ncpu
  exit 0
fi

echo "no nproc (or alternatives) not found" >&2
exit 1
