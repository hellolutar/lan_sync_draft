#ifndef __NETWORK_ADAPTER_H_
#define __NETWORK_ADAPTER_H_

#include "framework/net/net_addr.h"
#include "proto_server.h"

class NetworkAdapter
{
private:
    std::vector<std::shared_ptr<ProtoServer>> srvs_;

    std::vector<NetAddr> query_local_ports();

    const ProtoSession& findSession(const NetAddr& peer);

public:
    NetworkAdapter(){};
    ~NetworkAdapter(){};

    void start();

    /**
     * @brief connect peer with udp
     *
     * @param peer
     * @return uint32_t road_id
     */
    uint32_t setUpSessionWithPeer(const NetAddr &peer);

    void write(const NetAddr& peer, uint8_t *data, uint64_t size);
};

#endif