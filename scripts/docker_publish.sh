#!/usr/bin/env bash
set -e -o pipefail

# --------------------------------------------------------------------------------------------------
# Build and publish the docker container.
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

printUsage()
{
  echo "Options:"
  echo "-h,--help    Print this usage information"
  echo "-f,--file    Path to the dockerfile, default: 'Dockerfile'"
  echo "-r,--repo    Docker repository, default: 'bastianblokland/novus'"
}

getVersionFromDockerImage()
{
  local tgtImg="${1}"
  test -n "${tgtImg}" || fail "Provide docker image name as arg 1"

  local versionNum
  versionNum=$(sudo docker run --rm --entrypoint cat "${tgtImg}" /novus/VERSION)

  test -n "${versionNum}" || fail "Failed to retreive version number"

  echo "${versionNum}"
}

# Defaults.
dockerFile="Dockerfile"
repository="bastianblokland/novus"

# Parse options.
while [[ $# -gt 0 ]]
do
  case "${1}" in
    -h|--help)
      echo "Novus -- Docker publish"
      printUsage
      exit 0
      ;;
    -f|--file)
      dockerFile="${2}"
      shift 2
      ;;
    -r|--repo)
      repository="${2}"
      shift 2
      ;;
    *)
      error "Unknown option '${1}'"
      printUsage
      exit 1
      ;;
  esac
done

# Verify dependencies.
hasCommand docker || fail "'docker' not found on path, it is required"

readonly imageName="novus"

info "Building docker image"
sudo docker build -t "${imageName}" . --file "${dockerFile}"

readonly tag=$(getVersionFromDockerImage "${imageName}")

info "Puhsing image: ${repository}:${tag} to repository"
sudo docker tag "${imageName}" "${repository}:${tag}"

info "Tagging image: ${repository}:latest"
sudo docker tag "${imageName}" "${repository}:latest"

info "Pushing image to repository"
sudo docker push "${repository}:${tag}"
sudo docker push "${repository}:latest"

info "Sucesfully published docker image"
exit 0
