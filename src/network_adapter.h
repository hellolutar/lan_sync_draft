#ifndef __NETWORK_ADAPTER_H_
#define __NETWORK_ADAPTER_H_

#include "framework/net/net_addr.h"
#include "proto_session.h"

class NetworkAdapter
{
private:
    std::vector<std::shared_ptr<ProtoSession>> sess_;

    std::vector<NetAddr> query_local_ports();

    const std::shared_ptr<ProtoSession> findSession(uint32_t road_id);

public:
    NetworkAdapter(){};
    ~NetworkAdapter(){};

    void recv(uint32_t road_id, uint8_t *data, uint64_t size);

    void start();

    /**
     * @brief connect peer with udp
     *
     * @param peer
     * @return uint32_t road_id
     */
    uint32_t setUpSessionWithPeer(const NetAddr &peer);

    void write(uint32_t road_id, uint8_t *data, uint64_t size);
};

#endif