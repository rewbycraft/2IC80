#!/bin/bash
set -e

if [ "$(id -u)" != "0" ]; then
   echo "This script must be run as root" 1>&2
   exit 1
fi

cd "$(dirname "$(realpath "$0")")"

for rtr in e1 e2 c1 c2 d1 d2 a1 a2 a3 f1 b1; do
	echo "=> Configuring router $rtr..."
	for pid in $(ip netns pids $rtr 2>/dev/null); do
		echo "==> Killing pid $pid..."
		kill $pid
	done
	ip netns del $rtr 2>/dev/null || true
	ip netns add $rtr
	ip netns exec $rtr ip link set lo up
	echo 1 | ip netns exec $rtr tee /proc/sys/net/ipv6/conf/all/forwarding > /dev/null
	echo 1 | ip netns exec $rtr tee /proc/sys/net/ipv6/conf/default/forwarding > /dev/null
	echo 0 | ip netns exec $rtr tee /proc/sys/net/ipv4/conf/all/rp_filter > /dev/null
	echo 0 | ip netns exec $rtr tee /proc/sys/net/ipv4/conf/default/rp_filter > /dev/null
done

PFX=2a0b:6b83:2c80:ffff::

function addlink()
{
	echo "=> Adding link between $1 and $2..."
	ip link add name $1-$2 type veth peer name $2-$1
	ip link set dev $1-$2 netns $1
	ip link set dev $2-$1 netns $2
	ip netns exec $1 ip link set $1-$2 up
	ip netns exec $2 ip link set $2-$1 up
	ip netns exec $1 ip address add ${PFX}$1:$2:1/124 dev $1-$2
	ip netns exec $2 ip address add ${PFX}$1:$2:2/124 dev $2-$1
}

addlink e1 c1
addlink e1 c2
addlink e2 c1
addlink e2 c2
addlink c1 c2
addlink c1 d1
addlink c1 d2
addlink c2 d1
addlink c2 d2
addlink d1 d2
addlink e1 a1
addlink e2 a2
addlink a1 a3
addlink a2 a3
addlink f1 d1
addlink f1 d2
addlink b1 f1

for rtr in f1 e1 e2 c1 c2 d1 d2 a1 a2 a3; do
	echo "=> Starting bird $rtr..."
	ip netns exec $rtr bird -c ./configs/$rtr.conf -s /tmp/$rtr.sock -P /tmp/$rtr.pid -D /tmp/$rtr-dbg.log
	birdc -s /tmp/$rtr.sock debug all all
done

ip netns exec a3 ip address add 2a0b:6b83:2c80:1234::1/64 dev lo
ip netns exec b1 ip route add default via 2a0b:6b83:2c80:ffff:0:b1:f1:2

