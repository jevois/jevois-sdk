#!/bin/bash
set -e

CI_BUILD_USER=$(id -u -n)
CI_BUILD_UID=$(id -u)
CI_BUILD_GROUP=$(id -g -n)
CI_BUILD_GID=$(id -g)
CI_BUILD_HOME="/home/${CI_BUILD_USER}"

echo "Welcome to JeVois-SDK OS builder\n"

mkdir -p debs

docker run --rm -it --name jevois-sdk-build --pid=host --network=host \
       -v $(pwd):/jevois-sdk --env "CI_BUILD_HOME=${CI_BUILD_HOME}" --workdir /jevois-sdk \
       --env "CI_BUILD_USER=${CI_BUILD_USER}" --env "CI_BUILD_UID=${CI_BUILD_UID}" \
       --env "CI_BUILD_GROUP=${CI_BUILD_GROUP}" --env "CI_BUILD_GID=${CI_BUILD_GID}" \
       -v /etc/localtime:/etc/localtime:ro -v /etc/timezone:/etc/timezone:ro -d /dev:/dev \
       jevois/jevois-sdk-build:1.21.0 \
       /with_the_same_user "/jevois-sdk/rebuild-os.sh -y"
