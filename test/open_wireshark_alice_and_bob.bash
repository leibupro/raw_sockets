#!/bin/bash

source ./netns.bash

gnome-terminal -- sudo ip netns exec ${NETNS0} wireshark-gtk & 
gnome-terminal -- sudo ip netns exec ${NETNS1} wireshark-gtk & 

