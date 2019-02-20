#!/bin/bash
set -e

if [ "$(id -u)" != "0" ]; then
   echo "This script must be run as root" 1>&2
   exit 1
fi

cd "$(dirname "$(realpath "$0")")"

exec ip netns exec b1 mtr -n 2a0b:6b83:2c80:1234::1

