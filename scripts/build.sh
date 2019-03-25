#!/bin/bash
set -ex

if [ "$(id -u)" == "0" ]; then
   echo "This script must not be run as root" 1>&2
   exit 1
fi

if [ -z "$BUILDTYPE" ]
then
BUILDTYPE=Release
fi

cd "$(dirname "$(realpath "$0")")"/..

mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE=$BUILDTYPE ..
exec make -j 6

