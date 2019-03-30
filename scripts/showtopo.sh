#!/bin/bash
set -e

if [ "$(id -u)" != "0" ]; then
   echo "This script must be run as root" 1>&2
   exit 1
fi

for rtr in f1 e1 e2 c1 c2 d1 d2 a1 a2 a3; do
    echo "Router $rtr:"
    for intf in $(ip netns exec $rtr ip link show | grep -E '^[0-9]+:' | cut -d':' -f2 | cut -d '@' -f1); do
        echo -e "\t$intf:\tMAC $(ip netns exec $rtr ip link show dev $intf | tail -n 1 | sed -r 's/.* ([0-9a-f:]+) brd.*/\1/')"
        for ip in $(ip netns exec $rtr ip address show dev $intf | grep "inet" | sed -r 's/.*inet6? ([0-9a-f.:/]+) scope.*/\1/'); do
            echo -e "\t\t$ip"
        done
    done
done