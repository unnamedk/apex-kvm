#!/bin/bash

ip="192.168.1.200"
qemu_pid=$(pidof qemu-system-x86_64)
apex_pid=$1

sudo ./build/release/lotusc $ip $qemu_pid $apex_pid 
