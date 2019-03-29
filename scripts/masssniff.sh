#!/bin/bash
set -ex

if [ "$(id -u)" == "0" ]; then
   echo "This script must not be run as root" 1>&2
   exit 1
fi

USER=$(whoami)

cd "$(dirname "$(realpath "$0")")"/..

for rtr in f1 e1 e2 c1 c2 d1 d2 a1 a2 a3; do
    if [ -e /tmp/sniff-$rtr.pipe ]; then
        rm /tmp/sniff-$rtr.pipe
    fi
    mkfifo /tmp/sniff-$rtr.pipe
done

sudo cgcreate -g pids:attackmasssniff

for rtr in f1 e1 e2 c1 c2 d1 d2 a1 a2 a3; do
    sudo cgexec -g pids:attackmasssniff ip netns exec $rtr tcpdump -Z $USER -s 0 -n -w /tmp/sniff-$rtr.pipe -U -i any &
done

control_c()
{
  for i in $(ps -eo pid,cgroup | grep attackmasssniff | awk '{print $1}') ; do sudo kill $i ; done
  exit 0
}

trap control_c SIGINT

INTF="-k "
for rtr in f1 e1 e2 c1 c2 d1 d2 a1 a2 a3; do
    INTF="$INTF -i /tmp/sniff-$rtr.pipe"
done

wireshark $INTF

control_c
