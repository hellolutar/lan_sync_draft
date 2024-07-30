#include <iostream>
#include <cassert>
#include <cstring>
#include <sstream>

#include "example_common.h"
#include "framework/itf/net/net_framework.h"
#include "framework/itf/persist/persist_framework.h"
#include "framework/impl/net/net_framework_engine_base_event.h"
#include "framework/impl/persist/persist_framework_egine_filesystem.h"

using namespace std;

const string CLOSE_STR = "close";
const string SHUTDOWN_STR = "shutdown";

class TcpSrvLogic : public LogicWrite
{
private:
    std::stringstream ss;
    void save()
    {
        auto peg = PersistFramework::getEngine();
        string content = ss.str();
        auto u8arr = str_to_uint8(content);
        peg->saveTo("tcp_srv_rcv_file.txt", 0, u8arr, content.size());
        ss.str("");
    }

public:
    TcpSrvLogic(/* args */) {}
    ~TcpSrvLogic() {}

    void recv(NetAbilityContext &ctx, std::shared_ptr<uint8_t[]> data, uint64_t size) override
    {
        string str(reinterpret_cast<char *>(data.get()));
        str = str.substr(0, size);
        cout << str;
        ss << str;

        ctx.write(data, size);

        if (str == CLOSE_STR)
        {
            auto eg = Netframework::getEngine();
            eg->unRegister(ctx.from());
            close();
            save();
        }
        else if (str == SHUTDOWN_STR)
        {
            auto eg = Netframework::getEngine();
            eg->shutdown();
            save();
        }
    };
};

int main(int argc, char const *argv[])
{
    auto _base = make_shared<EventBaseWrap>(event_base_new());
    auto _eg = make_shared<NetFrameworkEngineBaseEvent>(_base);
    Netframework::init(_eg);

    auto _peg = make_shared<PersistFrameworkEgineImplWithFileSystem>();
    PersistFramework::init(_peg);

    auto engine = Netframework::getEngine();

    auto srv = engine->addTcpServer(NetAddr(":8080", TransportType::TCP));
    if (srv == 0)
        return -1;

    srv->bind(make_shared<TcpSrvLogic>());

    engine->start();
    srv = nullptr;

    return 0;
}