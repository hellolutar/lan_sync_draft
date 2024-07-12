#ifndef __LAN_SHARE_PROTOCOL_H
#define __LAN_SHARE_PROTOCOL_H

#include <cstdint>
#include <cstring>

#include <string>
#include <vector>
#include <map>
#include <sstream>

#include <arpa/inet.h>

#include "buf/abs_buf.h"

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

const std::uint16_t SIZE_1Kbit = 8196;

enum class lan_sync_version : uint8_t
{
    VER_0_1 = 1,
};

enum lan_sync_type_enum : uint8_t
{
    LAN_SYNC_TYPE_HELLO = 1,
    LAN_SYNC_TYPE_HELLO_ACK,
    LAN_SYNC_TYPE_GET_TABLE_INDEX,
    LAN_SYNC_TYPE_REPLY_TABLE_INDEX,
    LAN_SYNC_TYPE_GET_RESOURCE,
    LAN_SYNC_TYPE_REPLY_RESOURCE,
    LAN_SYNC_TYPE_UPDATE_RESOURCE,
    LAN_SYNC_TYPE_CLOSE,
    LAN_SYNC_TYPE_EXIT = 255,
};

std::string convert_lan_sync_type_enum(lan_sync_type_enum type);

typedef struct lan_sync_header
{
    enum lan_sync_version version;
    enum lan_sync_type_enum type;
    uint16_t header_len;
    uint32_t total_len;

} lan_sync_header_t;

#define LEN_LAN_SYNC_HEADER_T sizeof(lan_sync_header_t)

class LanSyncPkt
{
private:
    std::map<std::string, std::string> xheader;
    void *payload = nullptr; //  ~LanSyncPkt()
    uint16_t header_len = 0;
    uint32_t total_len = 0;

public:
    enum lan_sync_version version;
    enum lan_sync_type_enum type;
    LanSyncPkt(enum lan_sync_version version, enum lan_sync_type_enum type)
        : version(version), type(type), header_len(LEN_LAN_SYNC_HEADER_T), total_len(LEN_LAN_SYNC_HEADER_T), payload(nullptr){};
    LanSyncPkt(lan_sync_header_t *header);
    LanSyncPkt(void *header);
    ~LanSyncPkt();

    void writeTo(AbsBuf &buf);

    void addXheader(const std::string key, const std::string value);

    std::string queryXheader(std::string key);

    void *getPayload();
    void *getPayload() const;

    void setPayload(void *data, uint64_t datalen);

    uint16_t getHeaderLen();
    uint32_t getTotalLen();
    uint32_t getPayloadSize();
    enum lan_sync_version getVersion();
    enum lan_sync_type_enum getType();
    const std::map<std::string, std::string> getXheaders();

    bool operator==(const LanSyncPkt &p) const;
};

bool compareChar(char *l, char *r, uint32_t cnt);

class ContentRange
{
private:
    uint64_t start_pos;
    uint64_t size;
    uint64_t total_size;
    bool is_last;

public:
    // str like : 0-500/500/last
    ContentRange(std::string str);
    ContentRange(uint64_t start_pos, uint64_t size, uint64_t total_size, bool is_last)
        : start_pos(start_pos), size(size), total_size(total_size), is_last(is_last){};
    ~ContentRange();

    uint64_t getStartPos();
    uint64_t getSize();
    uint64_t getTotalSize();
    bool isLast();
    std::string to_string();
};

class Range
{
private:
    uint64_t start_pos;
    uint64_t size;

public:
    Range(std::string str);
    Range(uint64_t start_pos, uint64_t size) : start_pos(start_pos), size(size){};
    ~Range();
    uint64_t getStartPos();
    uint64_t getSize();
    std::string to_string();

    static std::string defaultStr;
};

#endif