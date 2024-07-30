#ifndef __LANSYNC_TEST_COMMON_H_
#define __LANSYNC_TEST_COMMON_H_

#include <string>
#include <sstream>
#include <cstdint>
#include <algorithm>
#include <memory>
#include <cstring>

#include "uint-test/dep/framework_engine_for_test.h"
#include "proto/lan_share_protocol.h"
#include "buf/buf_base_on_event.h"
#include "uint-test/dep/test_sync_proto.h"

std::string genStr(uint32_t size);

std::shared_ptr<uint8_t[]> gen_u8_array(uint64_t size);

LanSyncPkt pkt_hello(void *payload = nullptr, uint64_t size = 0);
LanSyncPkt pkt_hello_ack(void *payload = nullptr, uint64_t size = 0);
LanSyncPkt pkt_req_idx();
LanSyncPkt pkt_reply_idx(std::vector<Resource> tb);
LanSyncPkt pkt_req_rs(Resource r);

#endif