#include "framework_engine_for_test.h"

buf_data::buf_data(std::shared_ptr<uint8_t[]> data, uint64_t size)
{
    data_ = data;
    size_ = size;
}

buf_data::~buf_data()
{
    data_ = nullptr;
}

std::shared_ptr<uint8_t[]> buf_data::data()
{
    return data_;
}

uint64_t buf_data::size()
{
    return size_;
}

OutputStreamForTest::~OutputStreamForTest()
{
    clear();
}

void OutputStreamForTest::write(std::shared_ptr<uint8_t[]> data, uint64_t size)
{
    auto b = make_shared<buf_data>(data, size);
    bufs_.push(b);
}

std::shared_ptr<buf_data> OutputStreamForTest::front()
{
    return bufs_.front();
}

void OutputStreamForTest::pop(uint16_t size)
{
    for (size_t i = 0; i < size; i++)
        bufs_.pop();
}

void OutputStreamForTest::clear()
{
    while (!bufs_.empty())
    {
        bufs_.pop();
    }
}

NetFrameworkEngineForTest::~NetFrameworkEngineForTest()
{
    tcpsrv_.clear();
    udpsrv_.clear();
    tcpcli_.clear();
    udpcli_.clear();
}

std::shared_ptr<TcpServer> NetFrameworkEngineForTest::addTcpServer(const NetAddr &addr)
{
    auto srv = std::make_shared<TcpServer>(addr);
    auto os = std::make_unique<OutputStreamForTest>();
    srv->setOutputStream(std::move(os));
    tcpsrv_[addr] = srv;
    return srv;
}

std::shared_ptr<UdpServer> NetFrameworkEngineForTest::addUdpServer(const NetAddr &addr)
{
    auto srv = std::make_shared<UdpServer>(addr);
    auto os = std::make_unique<OutputStreamForTest>();
    srv->setOutputStream(std::move(os));
    udpsrv_[addr] = srv;
    return srv;
}

std::shared_ptr<TcpCli> NetFrameworkEngineForTest::connectWithTcp(const NetAddr &addr)
{
    auto cli = std::make_shared<TcpCli>(addr);
    auto os = std::make_unique<OutputStreamForTest>();
    cli->setOutputStream(std::move(os));
    tcpcli_[addr] = cli;
    return cli;
}

std::shared_ptr<UdpCli> NetFrameworkEngineForTest::connectWithUdp(const NetAddr &addr)
{
    auto cli = std::make_shared<UdpCliForTest>(1, addr);
    auto os = std::make_unique<OutputStreamForTest>();
    cli->setOutputStream(std::move(os));
    udpcli_[addr] = cli;
    return cli;
}

void NetFrameworkEngineForTest::unregisterUdpCli(const NetAddr &addr)
{
    auto iter = udpcli_.find(addr);
    if (iter != udpcli_.end())
        udpcli_.erase(iter);
}

std::shared_ptr<TcpCli> NetFrameworkEngineForTest::findTcpCli(const NetAddr &addr)
{
    auto iter = tcpcli_.find(addr);
    if (iter != tcpcli_.end())
        return iter->second;
    throw NotFoundException("NetFrameworkEngineForTest::findTcpCli(): ", addr);
}

std::shared_ptr<NetAbility> NetFrameworkEngineForTest::queryTcpSerNetAbility(const NetAddr &addr)
{
    auto iter = tcpsrv_.find(addr);
    if (iter != tcpsrv_.end())
        return iter->second;

    throw NotFoundException("NetFrameworkEngineForTest::queryTcpSerNetAbility(): ", addr);
}

std::shared_ptr<NetAbility> NetFrameworkEngineForTest::queryTcpCliNetAbility(const NetAddr &addr)
{
    auto iter = tcpcli_.find(addr);
    if (iter != tcpcli_.end())
        return iter->second;

    throw NotFoundException("NetFrameworkEngineForTest::queryTcpCliNetAbility(): ", addr);
}

std::shared_ptr<NetAbility> NetFrameworkEngineForTest::queryUdpSerNetAbility(const NetAddr &addr)
{
    auto iter = udpsrv_.find(addr);
    if (iter != udpsrv_.end())
        return iter->second;

    throw NotFoundException("NetFrameworkEngineForTest::queryUdpSerNetAbility(): ", addr);
}

std::shared_ptr<NetAbility> NetFrameworkEngineForTest::queryUdpCliNetAbility(const NetAddr &addr)
{
    auto iter = udpcli_.find(addr);
    if (iter != udpcli_.end())
        return iter->second;

    throw NotFoundException("NetFrameworkEngineForTest::queryUdpCliNetAbility(): ", addr);
}

std::shared_ptr<NetAbility> NetFrameworkEngineForTest::queryNetAbility(const NetAddr &addr)
{
    if (udpsrv_.find(addr) != udpsrv_.end())
        return udpsrv_.find(addr)->second;

    if (tcpsrv_.find(addr) != tcpsrv_.end())
        return tcpsrv_.find(addr)->second;

    if (tcpcli_.find(addr) != tcpcli_.end())
        return tcpcli_.find(addr)->second;

    if (udpcli_.find(addr) != udpcli_.end())
        return udpcli_.find(addr)->second;

    throw NotFoundException("NetFrameworkEngineForTest::queryNetAbility(): ", addr);
}

std::shared_ptr<Trigger> TimerFrameworkEngineForTest::findTrigger(std::uint16_t id)
{
    for (auto &&trg : trgs_)
    {
        if (trg->getId() == id)
            return trg;
    }

    throw NotFoundException("TimerFrameworkEngineForTest::findTrigger()! id:", id);
}

void TimerFrameworkEngineForTest::tick(uint64_t since_last)
{
    for (auto &&trg : trgs_)
        trg->trigger();
}

std::optional<std::shared_ptr<uint8_t[]>> ResourceManagerForTest::readFrom(std::string uri, const Block &blk) const
{
    uri_block k{uri, blk};
    auto iter = read_from_data_.find(k);
    if (iter != read_from_data_.end())
    {
        auto ret = make_optional<std::shared_ptr<uint8_t[]>>(iter->second);
        read_from_data_.erase(iter);
        return ret;
    }
    return nullopt;
}

void ResourceManagerForTest::setReadFrom(uri_block k, std::shared_ptr<uint8_t[]> v)
{
    read_from_data_[k] = v;
    read_from_history_data_[k] = v;
}

void ResourceManagerForTest::setReadFrom(const LanSyncPkt &pkt)
{
    auto uri_opt = pkt.queryXheader(XHEADER_URI);
    auto range_opt = pkt.queryXheader(XHEADER_RANGE);
    Range range(range_opt.value());

    // 分批传输
    uint64_t offset = range.getStart();
    while (offset < range.getEnd())
    {
        uint64_t size = min(BLOCK_SIZE, (range.getEnd() - offset));

        Block b(offset, offset + size);
        Range r(b.start, b.end);

        setReadFrom({uri_opt.value(), b}, gen_u8_array(size));

        offset += size;
    }
}

std::optional<std::shared_ptr<uint8_t[]>> ResourceManagerForTest::queryReadFromHistory(uri_block k)
{
    auto iter = read_from_history_data_.find(k);
    if (iter == read_from_history_data_.end())
    {
        return nullopt;
    }
    return make_optional<std::shared_ptr<uint8_t[]>>(iter->second);
}

std::string uri_block::str() const
{
    return uri + "-" + to_string(blk.start) + "-" + to_string(blk.end);
}

std::vector<NetAddr> NetworkAdapterForTest::query_local_ports()
{
    auto addr = NetAddr("0.0.0.10:" +  to_string(default_udp_srv_port));
    auto addr2 = NetAddr("0.0.0.20:" +  to_string(default_udp_srv_port));
    return {addr, addr2};
}

std::vector<NetAddr> NetworkAdapterForTest::query_broad_ports()
{
    return std::vector<NetAddr>();
}
