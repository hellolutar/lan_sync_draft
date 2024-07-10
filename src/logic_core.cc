#include "logic_core.h"

const uint64_t LogicCore::isExtraAllDataNow(uint8_t *data, uint64_t data_len) const
{
    if (data_len < LEN_LAN_SYNC_HEADER_T)
        return 0;

    LanSyncPkt pkt(data);
    if (pkt.getTotalLen() > data_len)
        return 0;

    return pkt.getTotalLen();
}

void LogicCore::recv(const NetAddr &peer, uint8_t *data, uint64_t size)
{
    // adapter_->write()
}
