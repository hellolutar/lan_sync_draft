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

using namespace std;

class buf_data
{
private:
  uint8_t *data_;
  uint64_t size_;

public:
  buf_data(uint8_t *data, uint64_t size);
  ~buf_data();

  uint8_t *data();
  uint64_t size();
};

class OutputStreamForTest : public OutputStream
{
private:
  queue<shared_ptr<buf_data>> bufs_;

public:
  OutputStreamForTest(/* args */) {}
  ~OutputStreamForTest();

  void write(uint8_t *data, uint64_t size) override;
  std::shared_ptr<buf_data> front();
  void pop(uint16_t size = 1);
  void clear();
};

class NetFrameworkEngineForTest : public NetframeworkEngine
{
private:
  map<NetAddr, shared_ptr<TcpServer>> tcpsrv_;
  map<NetAddr, shared_ptr<UdpServer>> udpsrv_;
  map<NetAddr, shared_ptr<TcpCli>> tcpcli_;
  map<NetAddr, shared_ptr<UdpCli>> udpcli_;

public:
  NetFrameworkEngineForTest(/* args */) {}
  ~NetFrameworkEngineForTest();

  std::shared_ptr<TcpServer> addTcpServer(const NetAddr &addr) override;
  std::shared_ptr<UdpServer> addUdpServer(const NetAddr &addr) override;
  std::shared_ptr<TcpCli> connectWithTcp(const NetAddr &addr) override;
  std::shared_ptr<UdpCli> connectWithUdp(const NetAddr &addr) override;

  void unregisterUdpCli(const NetAddr &addr) override;

  std::shared_ptr<TcpCli> findTcpCli(const NetAddr &addr) override;

  std::shared_ptr<NetAbility> queryTcpSerNetAbility(const NetAddr &addr);
  std::shared_ptr<NetAbility> queryTcpCliNetAbility(const NetAddr &addr);
  std::shared_ptr<NetAbility> queryUdpSerNetAbility(const NetAddr &addr);
  std::shared_ptr<NetAbility> queryUdpCliNetAbility(const NetAddr &addr);
  std::shared_ptr<NetAbility> queryNetAbility(const NetAddr &addr);
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
    return uri == other.uri && blk < other.blk;
  }

  bool operator==(const uri_block &other) const
  {
    return uri == other.uri && blk == other.blk;
  }
};

class ResourceManagerForTest : public ResourceManager
{
private:
  std::vector<Resource> table_;
  std::vector<Resource> need_to_sync_;
  bool validres_ = false;
  std::map<uri_block, std::shared_ptr<uint8_t[]>> read_from_data_;

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
  bool save(std::string uri, void *data, uint64_t offset,
            uint64_t data_len) override
  {
    return true;
  };
  std::shared_ptr<uint8_t[]> readFrom(std::string uri,
                                      const Block &blk) const override;

  void setIdx(std::vector<Resource> rs) { table_ = rs; };
  void setNeedToSync(std::vector<Resource> rs) { need_to_sync_ = rs; };
  void setValidRes(bool b) { validres_ = b; };
  void setReadFrom(uri_block k, std::shared_ptr<uint8_t[]> v);
};

#endif