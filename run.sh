#!/bin/bash

ip=$(ip -o route get to 8.8.8.8 | sed -n 's/.*src \([0-9.]\+\).*/\1/p')
qemu_pid=$(pidof qemu-system-x86_64)
apex_pid=$1

sudo ./build/release/lotusc $ip $qemu_pid $apex_pid 
