#include "proto_server.h"

void ProtoServerRecv::recv(const NetAddr &from, uint8_t *data, uint64_t size)
{
    // 寻找session
    for (auto &&s : *sess_)
    {
        if (s.isMe(from))
        {
            s.recv(from, data, size);
        }
    }
    

    // auto session_recv_f = std::bind(&ProtoSessionManager::recv, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    // todo 注意区分是tcpcli 还是 udpcli ；
    // auto r = RoadMaintainer::setupTCPCli(session_id_, from, core_logic_);
    // r->recv(from, data, size);
    // roads_->push_back(std::move(r));
    throw "注意区分是tcpcli 还是 udpcli ";
}

std::shared_ptr<ProtoSession> ProtoServerRecv::find(const NetAddr &from) const
{
    //
}
