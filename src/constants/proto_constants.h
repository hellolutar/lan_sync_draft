#ifndef __PROTO_CONSTANTS_H_
#define __PROTO_CONSTANTS_H_

#include <cstdint>
#include <string>

const std::uint8_t FLAG_KEY_VALUE_SPLIT = 2; // 2 is: ':' and '\0'

const std::string XHEADER_URI = "uri";
const std::string XHEADER_HASH = "hash";
const std::string XHEADER_TCPPORT = "tcpport";
/**
 * format:
 *      content-range:${first byte pos}-${last byte pos}\0
 * example:
 *      range:0-500\0                   want to get [0, 500)
 *      range:0-\0                      want to get [0, total_size)
 */
const std::string XHEADER_RANGE = "range";

/**
 * format:
 *      content-range:${first byte pos}-${last byte pos + 1 }/${entity legth}\0
 * example:
 *      content-range:0-500/500/last\0     reply[0,500), total size for the file is 500, this pkt is the last pkt
 *      content-range:0-500/22400/more\0     reply[0,500), total size for the file is 22400, the pkt is one of the many pkts sent by the server.
 */
const std::string XHEADER_CONTENT_RANGE = "content-range";

const std::string FLAG_XHEADER_CONTENT_RANGE_LAST = "last";
const std::string FLAG_XHEADER_CONTENT_RANGE_MORE = "more";
const std::string FLAG_XHEADER_CONTENT_SEPERATE = "/";
const char FLAG_XHEADER_CONTENT_SEPERATE_CHAR = '/';
const std::string FLAG_XHEADER_CONTENT_BETWEEN = "-";
const char FLAG_XHEADER_CONTENT_BETWEEN_CHAR = '-';

#endif