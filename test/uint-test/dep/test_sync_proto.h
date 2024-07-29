#ifndef __TEST_DEP_TEST_SYNC_PROTO_H_
#define __TEST_DEP_TEST_SYNC_PROTO_H_

#include <cstdint>
#include <string>

#include "proto/lan_share_protocol.h"

const std::uint16_t peer_cli_port = 18080;

const std::string peer_udp_cli_addr_str = "0.0.0.1:" + std::to_string(peer_cli_port);
const std::string peer_tcp_cli_addr_str = "0.0.0.1:" + std::to_string(peer_cli_port);
const std::string peer_udp_srv_addr_str = "0.0.0.1:" + std::to_string(default_udp_srv_port);
const std::string peer_tcp_srv_addr_str = "0.0.0.1:" + std::to_string(default_tcp_srv_port);

#endif