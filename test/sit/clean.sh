#!/usr/bin/bash
function clean_local(){
    if [ -d "veth0" ]; then
         sudo rm -rf veth0
    fi
    if [ -d "veth1" ]; then
         sudo rm -rf veth1
    fi
}

function clean_net() {
    if [[ $(ip netns | grep ns0) == ns0 ]]; then
        sudo ip netns del ns0
    fi

    if [[ $(ip netns | grep ns1) == ns0 ]]; then
        sudo ip netns del ns1
    fi
    
    if [[ $(ip link | grep "vbridge:") == ns0 ]]; then
        sudo ip link set dev vbridge down
        sudo brctl delbr vbridge
    fi

    if [[ $(ip link | grep veth-0-br) == ns0 ]]; then
         sudo ip link delete veth-0-br
    fi

    if [[ $(ip link | grep veth-1-br) == ns0 ]]; then
         sudo ip link delete veth-1-br
    fi
    
}
function clean() {
    clean_local
    clean_net
}

clean