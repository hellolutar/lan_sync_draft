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

const uint64_t ProtoServerRecv::isExtraAllDataNow(std::shared_ptr<uint8_t[]> data, uint64_t data_len) const
{
    return core_logic_->isExtraAllDataNow(data, data_len);
}

void ProtoServerRecv::recv(NetAbilityContext &ctx, std::shared_ptr<uint8_t[]> data, uint64_t size)
{
    NetAddr from = ctx.from();
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
            s->recv(ctx, data, size);
            return;
        }
        s = std::make_shared<ProtoSession>(core_logic_);
        if (s->call(from, pkt))
        {
            // 注意这里绑定的session应该是tcpcli;
            auto a = sess_->size();
            sess_->push_back(s);
            a = sess_->size();
        }
    }
    else if (pkt.getType() == lan_sync_type_enum::LAN_SYNC_TYPE_EXIT)
    {
        auto eg = Netframework::getEngine();
        eg->shutdown();
    }
    else
    {
        auto s = findSession(from);
        if (s != nullptr)
        {
            s->recv(ctx, data, size);
            return;
        }
        s = std::make_shared<ProtoSession>(core_logic_);
        if (s->bind(from, pkt))
        {
            sess_->push_back(s);
            s->recv(ctx, data, size);
        }
    }
}

const std::optional<std::shared_ptr<ProtoSession>> ProtoServer::findSession(const NetAddr &peer)
{
    for (uint64_t i = 0; i < sess_->size(); i++)
    {
        auto &s = sess_->at(i);
        if (s->isMe(peer))
            return std::make_optional<std::shared_ptr<ProtoSession>>(s);
    }

    return std::nullopt;
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
