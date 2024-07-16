#ifndef __LAN_SHARE_PROTOCOL_H
#define __LAN_SHARE_PROTOCOL_H

#include <cstdint>
#include <cstring>

#include <optional>
#include <string>
#include <vector>
#include <map>
#include <sstream>

#include <arpa/inet.h>

#include "buf/abs_buf.h"
#include "constants/proto_constants.h"

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
    std::shared_ptr<uint8_t[]> payload = nullptr; //  ~LanSyncPkt()
    uint16_t header_len = 0;
    uint32_t total_len = 0;

public:
    enum lan_sync_version version;
    enum lan_sync_type_enum type;
    LanSyncPkt(enum lan_sync_version version, enum lan_sync_type_enum type)
        : version(version), type(type), header_len(LEN_LAN_SYNC_HEADER_T), total_len(LEN_LAN_SYNC_HEADER_T), payload(nullptr){};
    LanSyncPkt(lan_sync_header_t *header);
    LanSyncPkt(std::shared_ptr<uint8_t[]> header);
    ~LanSyncPkt();

    void writeTo(AbsBuf &buf);

    void addXheader(const std::string key, const std::string value);

    const std::optional<std::string> queryXheader(std::string key) const;

    std::shared_ptr<uint8_t[]> getPayload();
    std::shared_ptr<uint8_t[]> getPayload() const;

    void setPayload(void *data, uint64_t datalen);

    uint16_t getHeaderLen() const;
    uint32_t getTotalLen() const;
    uint32_t getPayloadSize() const;
    enum lan_sync_version getVersion() const;
    enum lan_sync_type_enum getType() const;
    const std::map<std::string, std::string> getXheaders() const;

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

#endif