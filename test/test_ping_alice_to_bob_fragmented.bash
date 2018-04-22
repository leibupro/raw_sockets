#!/bin/bash

source ./netns.bash

sudo ip netns exec ${NETNS0} ping -I ${ALICE_IF} -s 2048 -c 1 ${BOB_IP}

