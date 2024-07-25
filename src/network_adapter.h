#ifndef __NETWORK_ADAPTER_H_
#define __NETWORK_ADAPTER_H_

#include "framework/dep/net_addr.h"
#include "proto_server.h"

class NetworkAdapter
{
protected:
    std::vector<std::shared_ptr<ProtoServer>> srvs_;

    virtual std::vector<NetAddr> query_local_ports();

    virtual std::vector<NetAddr> query_broad_ports();

    const ProtoSession &findSession(const NetAddr &peer);


public:
    NetworkAdapter(){};
    virtual ~NetworkAdapter()
    {
        srvs_.clear();
    };

    void start(std::shared_ptr<LogicWrite> core);

    void write(const NetAddr &peer, std::shared_ptr<uint8_t[]> data, uint64_t size);

    void udp_write(const NetAddr &peer, std::shared_ptr<uint8_t[]> data, uint64_t size);
    
    void rmSessionCallBack(const NetAddr &addr);
};

#endif