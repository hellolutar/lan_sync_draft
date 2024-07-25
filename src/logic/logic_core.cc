#include "logic_core.h"
#include "log/log.h"
#include "framework/itf/timer/timer_trigger_framework.h"

using namespace std;

void LogicCore::helloAck(const NetAddr &peer, const LanSyncPkt &pkt)
{
    auto idx = rm_->idx();
    auto dto = ResourceSerializer::serialize(idx);
    LanSyncPkt p = {lan_sync_version::VER_0_1, LAN_SYNC_TYPE_GET_TABLE_INDEX};

    // todo 取消udp的定时请求
    auto discover_ip = conf_->query(PropertiesParse::DISCOVER_IPS);
    auto discover_port = conf_->query(PropertiesParse::PROTO_DISCOVER_SERVER_UDP_PORT);
    NetAddr wantoDel(discover_ip + ":" + discover_port, TransportType::UDP);
    hello_trg_->delCtx(wantoDel);

    DEBUG_F("LogicCore::helloAck(): send get idx pkt!");

    BufBaseonEvent buf;
    p.writeTo(buf);
    write(buf.data(), buf.size());
}

void LogicCore::reqIdx(const NetAddr &peer, const LanSyncPkt &pkt)
{
    auto idx = rm_->idx();
    auto dto = ResourceSerializer::serialize(idx);
    LanSyncPkt p = {lan_sync_version::VER_0_1, LAN_SYNC_TYPE_REPLY_TABLE_INDEX};

    p.setPayload(dto.data.get(), dto.size);

    BufBaseonEvent buf;
    p.writeTo(buf);
    write(buf.data(), buf.size());
}

void LogicCore::reqRs(const NetAddr &peer, const LanSyncPkt &pkt)
{
    auto uri_opt = pkt.queryXheader(XHEADER_URI);
    auto range_opt = pkt.queryXheader(XHEADER_RANGE);
    if (!uri_opt.has_value() || !range_opt.has_value())
    {
        // LOG_ERROR("SyncService::handleLanSyncReplyResource() : query header is failed! ");
        return;
    }

    Range range(range_opt.value());

    if (range.size() <= BLOCK_SIZE)
    {
        Block b(range.getStart(), range.getEnd());
        readDataThenReplyRs(uri_opt.value(), b, peer);
    }
    else
    {
        // 分批传输
        uint64_t offset = range.getStart();
        while (offset < range.getEnd())
        {
            uint64_t size = min(BLOCK_SIZE, (range.getEnd() - offset));

            Block b(offset, offset + size);
            // Range r(b.start, b.end);
            readDataThenReplyRs(uri_opt.value(), b, peer);

            offset += b.size();
        }
    }
}

void LogicCore::readDataThenReplyRs(const std::string &uri, const Block &b, const NetAddr &peer)
{
    auto data_opt = rm_->readFrom(uri, b);
    if (!data_opt.has_value())
    {
        // LOGWARN
        return;
    }
    LanSyncPkt p = {lan_sync_version::VER_0_1, LAN_SYNC_TYPE_REPLY_RESOURCE};
    Range r(b.start, b.end);
    p.addXheader(XHEADER_RANGE, r.to_string());
    p.addXheader(XHEADER_URI, uri);
    p.setPayload(data_opt.value().get(), b.size());

    BufBaseonEvent buf;
    p.writeTo(buf);
    write(buf.data(), buf.size());
}

void LogicCore::recvIdx(const NetAddr &peer, const LanSyncPkt &pkt)
{
    // todo
    std::vector<Resource> tb = ResourceSerializer::deserialize(pkt.getPayload(), pkt.getPayloadSize());
    auto need_to_sync_rs = rm_->need_to_sync(tb);

    for (auto &&r : need_to_sync_rs)
        coor_->add_resource(r.getUri(), {0, r.getSize()}, {adapter_, peer});
}

void LogicCore::recvRs(const NetAddr &peer, const LanSyncPkt &pkt)
{
    auto uri_opt = pkt.queryXheader(XHEADER_URI);
    auto range_opt = pkt.queryXheader(XHEADER_RANGE);
    if (!uri_opt.has_value() || !range_opt.has_value())
    {
        // LOG_ERROR("SyncService::handleLanSyncReplyResource() : query header is failed! ");
        return;
    }
    auto &uri = uri_opt.value();
    auto &range_str = range_opt.value();
    Range r(range_str);

    auto data = pkt.getPayload();

    if (rm_->save(uri, data, r.getStart(), r.size()))
    {
        Block blk(r.getStart(), r.getEnd());
        coor_->taskManager()->success(uri, blk);
    }
}

void LogicCore::shutdown(const NetAddr &peer, const LanSyncPkt &pkt) {}

const uint64_t LogicCore::isExtraAllDataNow(std::shared_ptr<uint8_t[]> data,
                                            uint64_t data_len) const
{
    if (data_len < LEN_LAN_SYNC_HEADER_T)
        return 0;

    LanSyncPkt pkt(data);
    if (pkt.getTotalLen() > data_len)
        return 0;

    return pkt.getTotalLen();
}

void LogicCore::recv(const NetAddr &peer, std::shared_ptr<uint8_t[]> data, uint64_t size)
{
    LanSyncPkt pkt(data);

    DEBUG_F("LogicCore::recv(): from:{}\ttype:{}", peer.str(), convert_lan_sync_type_enum(pkt.getType()));

    switch (pkt.getType())
    {
    case lan_sync_type_enum::LAN_SYNC_TYPE_HELLO_ACK:
        helloAck(peer, pkt);
        break;
    case lan_sync_type_enum::LAN_SYNC_TYPE_GET_TABLE_INDEX:
        reqIdx(peer, pkt);
        break;
    case lan_sync_type_enum::LAN_SYNC_TYPE_GET_RESOURCE:
        reqRs(peer, pkt);
        break;
    case lan_sync_type_enum::LAN_SYNC_TYPE_REPLY_TABLE_INDEX:
        recvIdx(peer, pkt);
        break;
    case lan_sync_type_enum::LAN_SYNC_TYPE_REPLY_RESOURCE:
        recvRs(peer, pkt);
        break;
    case lan_sync_type_enum::LAN_SYNC_TYPE_EXIT:
        shutdown(peer, pkt);
        break;
    default:
        throw NotFoundException(
            "LogicCore::recv() can not match the pkt.type!");
        break;
    }
}

void LogicCore::setNetworkAdapter(std::shared_ptr<NetworkAdapter> ad)
{
    adapter_ = nullptr;
    adapter_ = ad;
}

void LogicCore::setResourceManager(std::shared_ptr<ResourceManager> rm)
{
    rm_ = rm;
    if (coor_ != nullptr)
        coor_->setResourceManager(rm);
}

void LogicCore::setTaskCoordinator(std::shared_ptr<TaskCoordinator> coor)
{
    coor_ = coor;

    if (rm_ != nullptr)
        coor_->setResourceManager(rm_);
}

void LogicCore::setUdpCliTrigger(std::shared_ptr<UdpCliTrigger> trg)
{
    hello_trg_ = trg;
}

void LogicCore::setPropertiesParse(std::shared_ptr<PropertiesParse> conf)
{
    conf_ = conf;
}
