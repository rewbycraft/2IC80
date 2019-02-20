#!/bin/bash
set -e

if [ "$(id -u)" != "0" ]; then
   echo "This script must be run as root" 1>&2
   exit 1
fi

cd "$(dirname "$(realpath "$0")")"

for rtr in e1 e2 c1 c2 d1 d2 a1 a2 a3 f1 b1; do
	echo "=> Deconfiguring router $rtr..."
	for pid in $(ip netns pids $rtr); do
		echo "==> Killing pid $pid..."
		kill $pid
	done
	ip netns del $rtr 2>/dev/null || true
done

