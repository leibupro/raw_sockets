#!/bin/bash

source ./netns.bash

sudo ip netns exec ${NETNS1} ping -I ${BOB_IF} -c 1 ${ALICE_IP}

