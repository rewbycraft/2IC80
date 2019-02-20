#!/bin/bash
set -e

if [ "$(id -u)" != "0" ]; then
   echo "This script must be run as root" 1>&2
   exit 1
fi

cd "$(dirname "$(realpath "$0")")"

ns="$1"
shift
exec ip netns exec "$ns" "$@"

