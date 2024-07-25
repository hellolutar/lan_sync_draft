#include "proto_server.h"

std::shared_ptr<ProtoSession> ProtoServerRecv::findSession(const NetAddr &from)
{
    for (auto &&s : *sess_)
    {
        if (s->isMe(from))
            return s;
    }
    return nullptr;
}

void ProtoServerRecv::recv(const NetAddr &from, std::shared_ptr<uint8_t[]> data, uint64_t size)
{
    LanSyncPkt pkt(data);

    if (pkt.getType() == lan_sync_type_enum::LAN_SYNC_TYPE_HELLO)
    {
        uint16_t peerPort = *(reinterpret_cast<uint16_t *>(pkt.getPayload().get()));
        NetAddr peer(from);
        peer.setType(TransportType::TCP);
        peer.setPort(peerPort);

        auto s = findSession(peer);
        if (s != nullptr)
        {
            s->recv(from, data, size);
            return;
        }
        s = std::make_shared<ProtoSession>(core_logic_);
        if (s->call(from, pkt))
        {
            // 注意这里绑定的session应该是tcpcli;
            sess_->push_back(s);
        }
    }
    else
    {
        auto s = findSession(from);
        if (s != nullptr)
        {
            s->recv(from, data, size);
            return;
        }
        s = std::make_shared<ProtoSession>(core_logic_);
        if (s->bind(from, pkt))
        {
            sess_->push_back(s);
            s->recv(from, data, size);
        }
    }
}

const ProtoSession &ProtoServer::findSession(const NetAddr &peer)
{
    for (uint64_t i = 0; i < sess_->size(); i++)
    {
        auto &s = sess_->at(i);
        if (s->isMe(peer))
            return *(s.get());
    }

    throw NotFoundException("ProtoServer::findSession can not found the peer !");
}

bool ProtoServer::rmSessionIfExist(const NetAddr &addr)
{
    for (auto it = sess_->begin(); it != sess_->end(); it++)
    {
        auto a = *it;
        if (a->isMe(addr))
        {
            sess_->erase(it);
            return true;
        }
    }
    return false;
}
