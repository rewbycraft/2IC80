#!/bin/bash
set -e

if [ "$(id -u)" == "0" ]; then
   echo "This script must not be run as root" 1>&2
   exit 1
fi

tmux \
  new-session -n "f1" "tail -f /tmp/f1.log ; read" \; \
  new-window -n "e1" "tail -f /tmp/e1.log ; read" \; \
  new-window -n e2 "tail -f /tmp/e2.log ; read" \; \
  new-window -n c1 "tail -f /tmp/c1.log ; read" \; \
  new-window -n c2 "tail -f /tmp/c2.log ; read" \; \
  new-window -n d1 "tail -f /tmp/d1.log ; read" \; \
  new-window -n d2 "tail -f /tmp/d2.log ; read" \; \
  new-window -n a1 "tail -f /tmp/a1.log ; read" \; \
  new-window -n a2 "tail -f /tmp/a2.log ; read" \; \
  new-window -n a3 "tail -f /tmp/a3.log ; read" \; \
  select-layout even-vertical
