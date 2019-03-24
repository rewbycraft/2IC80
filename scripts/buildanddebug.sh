#!/bin/bash
set -ex

if [ "$(id -u)" == "0" ]; then
   echo "This script must not be run as root" 1>&2
   exit 1
fi

cd "$(dirname "$(realpath "$0")")"/..

./scripts/build.sh
exec sudo ./scripts/debug.sh -l 0 -n c2
