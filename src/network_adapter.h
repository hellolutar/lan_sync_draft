#ifndef __NETWORK_ADAPTER_H_
#define __NETWORK_ADAPTER_H_

#include "net/net_addr.h"
#include "proto_session_manager.h"

class NetworkAdapter
{
private:
    std::vector<std::shared_ptr<ProtoSessionManager>> sess_;

    std::vector<NetAddr> query_local_ports();

    std::function<void(uint32_t, uint8_t *, uint64_t)> logic_recv_f_;

    const std::shared_ptr<ProtoSessionManager> findSession(uint32_t road_id);

public:
    NetworkAdapter(){};
    NetworkAdapter(std::function<void(uint32_t, uint8_t *, uint64_t)> f) : logic_recv_f_(f) {}
    ~NetworkAdapter() {}

    void setLogicRecvFunc(std::function<void(uint32_t, uint8_t *, uint64_t)> logic_recv_f);

    void recv(uint32_t road_id, uint8_t *data, uint64_t size);

    void start();

    void connPeer(const NetAddr &peer);

    void write(uint32_t road_id, uint8_t* data, uint64_t size);
};

#endif