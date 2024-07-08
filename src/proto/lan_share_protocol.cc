#include "lan_share_protocol.h"

using namespace std;

LanSyncPkt::~LanSyncPkt()
{
    if (payload != nullptr)
    {
        free(payload);
        payload = nullptr;
    }
}

// https://blog.csdn.net/weixin_43919932/article/details/111304250
vector<string> splitstr(string str, const char split)
{
    vector<string> ret;
    istringstream iss(str);
    string token;
    while (getline(iss, token, split))
    {
        ret.push_back(token);
    }
    return ret;
}

LanSyncPkt::LanSyncPkt(lan_sync_header_t *header)
{
    if (header->total_len < LEN_LAN_SYNC_HEADER_T)
    {
        total_len = 0;
        header_len = 0;
        return;
    }
    version = header->version;
    type = header->type;
    header_len = ntohs(header->header_len);
    total_len = ntohl(header->total_len);

    uint16_t xheader_len = header_len - LEN_LAN_SYNC_HEADER_T;
    if (xheader_len > 0)
    {
        // parse header
        char *xhdp = (char *)(header + 1);
        uint16_t kvstr_len = 0;
        char *xhdpi = xhdp;
        for (; kvstr_len < xheader_len;)
        {
            xhdpi = xhdp + kvstr_len;
            string kvstr(xhdpi);
            kvstr_len += kvstr.size() + 1; // 1 is '\0'
            vector<string> kvv = splitstr(kvstr, ':');
            if (kvv.size() >= 2)
            {
                xheader[kvv[0]] = kvv[1];
            }
        }
    }

    // parse data
    uint32_t data_len = total_len - header_len;
    if (data_len > 0)
    {
        payload = (char *)malloc(data_len);
        char *datap = (char *)(header) + header_len;
        memcpy(payload, datap, data_len);
    }else{
        payload = nullptr;
    }
}

LanSyncPkt::LanSyncPkt(void *rawp)
{
    lan_sync_header_t *hd = reinterpret_cast<lan_sync_header_t *>(rawp);
    new (this) LanSyncPkt(hd);
}

void LanSyncPkt::writeTo(AbsBuf &buf)
{

    lan_sync_header_t *hd = (lan_sync_header_t *)malloc(total_len);
    memset(hd, 0, total_len);

    hd->version = version;
    hd->type = type;
    hd->header_len = htons(header_len);
    hd->total_len = htonl(total_len);

    // 写入header
    char *xhdp = (char *)(hd + 1);

    char *xhdpi = xhdp;
    for (auto i = xheader.begin(); i != xheader.end(); i++)
    {
        string key = (*i).first;
        string value = (*i).second;
        uint16_t kv_len = key.size() + value.size() + FLAG_KEY_VALUE_SPLIT;
        sprintf(xhdpi, "%s:%s\0", key.data(), value.data());
        xhdpi += kv_len;
    }

    // 写入data
    uint16_t xhd_len = header_len - LEN_LAN_SYNC_HEADER_T;
    char *datap = (char *)(xhdp + xhd_len);
    memcpy(datap, payload, total_len - header_len);

    buf.add((uint8_t *)hd, total_len);
    free(hd);
}

void LanSyncPkt::addXheader(const string key, const string value)
{
    string exists_value = xheader[key];
    uint16_t kv_len = 0;
    if (exists_value.size() != 0)
    {
        // todo(lutar) 移除， 并更新header_len total_len
        kv_len = key.size() + exists_value.size() + FLAG_KEY_VALUE_SPLIT;
        header_len -= kv_len;
        total_len -= kv_len;
    }
    xheader[key] = value;
    kv_len = key.size() + value.size() + FLAG_KEY_VALUE_SPLIT;
    header_len += kv_len;
    total_len += kv_len;
}

string LanSyncPkt::queryXheader(string key)
{
    return xheader[key];
}

const map<string, string> LanSyncPkt::getXheaders()
{
    return xheader;
}

bool LanSyncPkt::operator==(const LanSyncPkt &p) const
{
    return version == p.version && header_len == p.header_len && total_len == p.total_len && type == p.type && xheader == p.xheader;
}

void *LanSyncPkt::getPayload()
{
    return payload;
}

void LanSyncPkt::setPayload(void *data_arg, uint32_t datalen)
{
    if (payload != nullptr)
    {
        uint32_t datalen = total_len - header_len;
        total_len -= datalen;
        free(payload);
    }

    payload = (void *)malloc(datalen);
    memset(payload, 0, datalen);
    memcpy(payload, data_arg, datalen);

    total_len += datalen;
}

uint16_t LanSyncPkt::getHeaderLen()
{
    return header_len;
}
uint32_t LanSyncPkt::getTotalLen()
{
    return total_len;
}
uint32_t LanSyncPkt::getPayloadSize()
{
    return total_len - header_len;
}

enum lan_sync_version LanSyncPkt::getVersion()
{
    return version;
}
enum lan_sync_type_enum LanSyncPkt::getType()
{
    return type;
}

bool compareChar(char *l, char *r, uint32_t cnt)
{
    for (size_t i = 0; i < cnt; i++)
    {
        if (*l != *r)
            return false;
    }
    return true;
}

ContentRange::ContentRange(string str)
{
    vector<string> rawv = splitstr(str, FLAG_XHEADER_CONTENT_SEPERATE_CHAR);
    sscanf(rawv[1].c_str(), "%lu", &total_size);
    if (FLAG_XHEADER_CONTENT_RANGE_LAST == rawv[2])
        is_last = true;
    else
        is_last = false;

    vector<string> raw_range = splitstr(rawv[0], FLAG_XHEADER_CONTENT_BETWEEN_CHAR);
    sscanf(raw_range[0].c_str(), "%lu", &start_pos);
    uint64_t end = 0;
    sscanf(raw_range[1].c_str(), "%lu", &end);
    size = end - start_pos;
}

ContentRange::~ContentRange()
{
}

uint64_t ContentRange::getStartPos()
{
    return start_pos;
}
uint64_t ContentRange::getSize()
{
    return size;
}
uint64_t ContentRange::getTotalSize()
{
    return total_size;
}
bool ContentRange::isLast()
{
    return is_last;
}

// str like : 0-500/500/last
string ContentRange::to_string()
{
    stringstream ss;
    ss << start_pos << FLAG_XHEADER_CONTENT_BETWEEN << (start_pos + size) << FLAG_XHEADER_CONTENT_SEPERATE << total_size << FLAG_XHEADER_CONTENT_SEPERATE;
    if (is_last)
        ss << FLAG_XHEADER_CONTENT_RANGE_LAST;
    else
        ss << FLAG_XHEADER_CONTENT_RANGE_MORE;

    return ss.str();
}

string Range::defaultStr = "0-0/0/last";

// str like : 0-500
// str like : 0-
Range::Range(string str)
{
    vector<string> rawv = splitstr(str, FLAG_XHEADER_CONTENT_BETWEEN_CHAR);
    sscanf(rawv[0].c_str(), "%lu", &start_pos);
    if (rawv.size() == 1)
    {
        size = 0;
        return;
    }
    uint64_t end = 0;
    sscanf(rawv[1].c_str(), "%lu", &end);
    size = end - start_pos;
}

Range::~Range()
{
}

uint64_t Range::getStartPos()
{
    return start_pos;
}
uint64_t Range::getSize()
{
    return size;
}

// str like : 0-500
// str like : 0-
string Range::to_string()
{
    stringstream ss;
    ss << start_pos << FLAG_XHEADER_CONTENT_BETWEEN;
    if (size != 0)
    {
        ss << (start_pos + size);
    }

    return ss.str();
}

std::string convert_lan_sync_type_enum(lan_sync_type_enum type)
{
    switch (type)
    {
    case LAN_SYNC_TYPE_HELLO:
        return "HELLO";
    case LAN_SYNC_TYPE_HELLO_ACK:
        return "HELLO_ACK";
    case LAN_SYNC_TYPE_GET_TABLE_INDEX:
        return "GET_TB_IDX";
    case LAN_SYNC_TYPE_REPLY_TABLE_INDEX:
        return "REPLY_TB_IDX";
    case LAN_SYNC_TYPE_GET_RESOURCE:
        return "GET_RS";
    case LAN_SYNC_TYPE_REPLY_RESOURCE:
        return "REPLY_RS";
    case LAN_SYNC_TYPE_UPDATE_RESOURCE:
        return "UPDATE_RS";
    case LAN_SYNC_TYPE_CLOSE:
        return "CLOSE";
    case LAN_SYNC_TYPE_EXIT:
        return "EXIT";
    default:
        return "UNKNOW";
    }
}
