#!/bin/bash
set -ex

if [ "$(id -u)" != "0" ]; then
   echo "This script must be run as root" 1>&2
   exit 1
fi

cd "$(dirname "$(realpath "$0")")"/..

function stop_gdb_server {
    for i in $(ps -eo pid,cgroup | grep attackdebug | awk '{print $1}') ; do kill $i ; done
    exit
}

trap stop_gdb_server INT TERM ERR
trap "kill 0" EXIT

cgcreate -g pids:attackdebug
cgexec -g pids:attackdebug gdbserver --once :1234 ./build/attack "$@"
