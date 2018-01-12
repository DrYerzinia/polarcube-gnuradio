#!/bin/bash
ifconfig tun0 1.1.1.2
ip route add 1.1.1.0/24 dev tun0
ip link set dev tun0 mtu 255

