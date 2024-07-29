#ifndef __NET_FRAMEWORK_ENGINE_FOR_TEST_H_
#define __NET_FRAMEWORK_ENGINE_FOR_TEST_H_

#include <cstring>
#include <map>
#include <queue>
#include <vector>

#include "exc/not_found_exc.h"
#include "framework/itf/net/net_framework.h"
#include "framework/itf/timer/timer_trigger_framework.h"
#include "resource_manager.h"
#include "uint-test/dep/test_common.h"

using namespace std;

class buf_data
{
private:
  std::shared_ptr<uint8_t[]> data_;
  uint64_t size_;

public:
  buf_data(std::shared_ptr<uint8_t[]> data, uint64_t size);
  ~buf_data();

  std::shared_ptr<uint8_t[]> data();
  uint64_t size();
};

class OutputStreamForTest : public OutputStream
{
private:
  queue<shared_ptr<buf_data>> bufs_;

public:
  OutputStreamForTest(/* args */) {}
  ~OutputStreamForTest();

  void write(std::shared_ptr<uint8_t[]> data, uint64_t size) override;
  std::shared_ptr<buf_data> front();
  void pop(uint16_t size = 1);
  void clear();
  uint64_t size();
};

class UdpCliForTest : public UdpCli
{
public:
  UdpCliForTest(/* args */)
  {
    os_ = std::make_shared<OutputStreamForTest>();
  }
  UdpCliForTest(int sock, NetAddr peer) : UdpCli(sock, peer)
  {
    os_ = std::make_shared<OutputStreamForTest>();
  }
  ~UdpCliForTest() {}

  void write(std::shared_ptr<uint8_t[]> data, uint64_t size) override
  {
    os_->write(data, size);
  }
};

class TcpCliForTest : public TcpCli
{
public:
  TcpCliForTest(NetAddr peer) : TcpCli(peer)
  {
    os_ = std::make_shared<OutputStreamForTest>();
  }
  ~TcpCliForTest() {}

  void write(std::shared_ptr<uint8_t[]> data, uint64_t size) override
  {
    os_->write(data, size);
  }
};

class TcpServerForTest : public TcpServer
{
private:
  NetAddr port_;
  std::shared_ptr<OutputStreamForTest> os_ = std::make_shared<OutputStreamForTest>();

public:
  TcpServerForTest() {}
  TcpServerForTest(NetAddr port) : TcpServer(port) {}
  virtual ~TcpServerForTest() {};

  std::shared_ptr<OutputStreamForTest> os() { return os_; };
};

class UdpServerForTest : public UdpServer
{
private:
  NetAddr port_;
  std::shared_ptr<OutputStreamForTest> os_ = std::make_shared<OutputStreamForTest>();

public:
  UdpServerForTest() {}
  UdpServerForTest(NetAddr port) : port_(port) {}
  ~UdpServerForTest() {};

  std::shared_ptr<OutputStreamForTest> os() { return os_; };
};

class NetFrameworkEngineForTest : public NetframeworkEngine
{
private:
  map<NetAddr, shared_ptr<TcpServerForTest>> tcpsrv_;
  map<NetAddr, shared_ptr<UdpServerForTest>> udpsrv_;
  map<NetAddr, shared_ptr<TcpCliForTest>> tcpcli_;
  map<NetAddr, shared_ptr<UdpCliForTest>> udpcli_;

public:
  NetFrameworkEngineForTest(/* args */) {}
  ~NetFrameworkEngineForTest();

  std::shared_ptr<TcpServer> addTcpServer(const NetAddr &addr) override;
  std::shared_ptr<UdpServer> addUdpServer(const NetAddr &addr) override;
  std::shared_ptr<TcpCli> connectWithTcp(const NetAddr &addr) override;
  std::shared_ptr<UdpCli> connectWithUdp(const NetAddr &addr) override;
  void addConn(std::shared_ptr<Connection>) override{};
  void start() {};

  void unRegister(const NetAddr &addr) override;

  std::shared_ptr<TcpCli> findTcpCli(const NetAddr &addr) override;

  std::shared_ptr<TcpServerForTest> queryTcpSerNetAbility(const NetAddr &addr);
  std::shared_ptr<TcpCliForTest> queryTcpCliNetAbility(const NetAddr &addr);
  std::shared_ptr<UdpServerForTest> queryUdpSerNetAbility(const NetAddr &addr);
  std::shared_ptr<UdpCliForTest> queryUdpCliNetAbility(const NetAddr &addr);
  std::shared_ptr<NetAbility> queryNetAbility(const NetAddr &addr);

  void tcp_disconn(const NetAddr &addr);
};

class TimerFrameworkEngineForTest : public TimerFrameworkEngine
{
public:
  TimerFrameworkEngineForTest(/* args */) {}
  ~TimerFrameworkEngineForTest() {}
  void run() {}

  std::shared_ptr<Trigger> findTrigger(uint16_t id);

  void tick(uint64_t since_last);
};

class uri_block
{
private:
  std::string uri;
  Block blk;

public:
  uri_block(std::string u, Block b) : uri(u), blk(b) {}
  ~uri_block() {}

  bool operator<(const uri_block &other) const
  {
    return str() < other.str();
  }

  bool operator==(const uri_block &other) const
  {
    return str() == other.str();
  }

  std::string str() const;
};

class ResourceManagerForTest : public ResourceManager
{
private:
  std::vector<Resource> table_;
  std::vector<Resource> need_to_sync_;
  bool validres_ = false;
  mutable std::map<uri_block, std::shared_ptr<uint8_t[]>> read_from_data_;
  mutable std::map<uri_block, std::shared_ptr<uint8_t[]>> read_from_history_data_;

public:
  ResourceManagerForTest(/* args */) {}
  virtual ~ResourceManagerForTest() {}

  std::vector<Resource> idx() override { return table_; };

  const Resource &query(std::string uri) const override
  {
    for (size_t i = 0; i < table_.size(); i++)
    {
      if (table_[i].getUri() == uri)
      {
        return table_[i];
      }
    }
    throw NotFoundException("ResourceManagerForTest:query()! uri:", uri);
  };
  bool validRes(std::string uri, std::string hash) const override
  {
    return validres_;
  };
  std::vector<Resource> need_to_sync(
      std::vector<struct Resource> peer_table) const override
  {
    return need_to_sync_;
  };
  bool save(std::string uri, std::shared_ptr<uint8_t[]> data, uint64_t offset,
            uint64_t data_len) override
  {
    return true;
  };
  std::optional<std::shared_ptr<uint8_t[]>> readFrom(std::string uri, const Block &blk) const override;

  void setIdx(std::vector<Resource> rs) { table_ = rs; };
  void setNeedToSync(std::vector<Resource> rs) { need_to_sync_ = rs; };
  void setValidRes(bool b) { validres_ = b; };
  void setReadFrom(uri_block k, std::shared_ptr<uint8_t[]> v);
  void setReadFrom(const LanSyncPkt &pkt);

  std::optional<std::shared_ptr<uint8_t[]>> queryReadFromHistory(uri_block k);
};

class NetworkAdapterForTest : public NetworkAdapter
{
protected:
  std::vector<NetAddr> query_local_ports() override;
  std::vector<NetAddr> query_broad_ports() override;

public:
  NetworkAdapterForTest(/* args */) {}
  ~NetworkAdapterForTest() {}

  bool isSessionExists(const NetAddr &peer);
};

#endif