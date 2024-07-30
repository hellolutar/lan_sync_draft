#!/usr/bin/bash

NUMBER=0

cd "veth${NUMBER}"

sudo ip netns exec "ns${NUMBER}" bash

