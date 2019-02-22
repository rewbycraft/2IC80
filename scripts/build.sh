#!/bin/bash
set -e

if [ "$(id -u)" == "0" ]; then
   echo "This script must not be run as root" 1>&2
   exit 1
fi

cd "$(dirname "$(realpath "$0")")"/..

mkdir -p build
cd build
cmake ..
exec make -j 6

