#include "proto_server.h"

void ProtoServerRecv::recv(const NetAddr &from, uint8_t *data, uint64_t size)
{
    // 寻找session
    for (auto &&s : *sess_)
    {
        if (s->isMe(from))
        {
            s->recv(from, data, size);
            return;
        }
    }

    LanSyncPkt pkt(data);
    auto s = std::make_shared<ProtoSession>(core_logic_);
    if (from.type() == TransportType::UDP)
    {
        s->call(from, pkt);
        sess_->push_back(s);
    }
    else if (from.type() == TransportType::TCP)
    {
        s->bind(from, pkt);
        sess_->push_back(s);
        s->recv(from, data, size);
    }
    else
        throw "the type is unknow";
}

const ProtoSession &ProtoServer::findSession(const NetAddr &peer)
{
    for (size_t i = 0; i < sess_->size(); i++)
    {
        auto &s = sess_->at(i);
        if (s->isMe(peer))
            return *(s.get());
    }

    throw NotFoundException("ProtoServer::findSession can not found the peer !");
}
