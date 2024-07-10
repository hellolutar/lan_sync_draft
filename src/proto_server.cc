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

    ProtoSession s(core_logic_);
    if (from.type() == TransportType::UDP)
        s.call(from);
    else if (from.type() == TransportType::TCP)
        s.bind(from);
    else
        throw "the type is unknow";
    s.recv(from, data, size);
    sess_->push_back(s);
}

std::shared_ptr<ProtoSession> ProtoServerRecv::find(const NetAddr &from) const
{
    //
}

const  ProtoSession& ProtoServer::findSession(const NetAddr &peer)
{
    for (size_t i = 0; i < sess_->size(); i++)
    {
        auto &s = sess_->at(i);
        if (s.isMe(peer))
            return s;
    }

    throw "ProtoServer::findSession can not found the peer !";
}
