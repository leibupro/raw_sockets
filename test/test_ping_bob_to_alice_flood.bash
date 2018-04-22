#!/bin/bash

source ./netns.bash

sudo ip netns exec ${NETNS1} ping -I ${BOB_IF} -s 65507 -c 20000 -f ${ALICE_IP}

