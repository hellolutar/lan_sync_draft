#include <iostream>
#include <cassert>
#include <cstring>

#include "framework/impl/net/event_wrap.h"
#include "framework/itf/net/net_framework.h"
#include "framework/impl/net/net_framework_engine_base_event.h"

using namespace std;

const string CLOSE_STR = "close";
const string SHUTDOWN_STR = "shutdown";

class TcpSrvLogic : public LogicWrite
{
private:
    /* data */
public:
    TcpSrvLogic(/* args */) {}
    ~TcpSrvLogic() {}

    void recv(const NetAddr &peer, std::shared_ptr<uint8_t[]> data, uint64_t size) override
    {
        string str(reinterpret_cast<char *>(data.get()));
        str = str.substr(0, size);
        cout << str;

        write(data, size);

        if (str == CLOSE_STR)
        {
            auto eg = Netframework::getEngine();
            eg->unRegister(peer);
            close();
        }
        else if (str == SHUTDOWN_STR)
        {
            auto eg = Netframework::getEngine();
            eg->shutdown();
        }
    };
};

int main(int argc, char const *argv[])
{
    auto _base = make_shared<EventBaseWrap>(event_base_new());
    auto _eg = make_shared<NetFrameworkEngineBaseEvent>(_base);
    Netframework::init(_eg);

    auto engine = Netframework::getEngine();

    auto srv = engine->addTcpServer(NetAddr(":8080", TransportType::TCP));
    if (srv == 0)
        return -1;

    srv->bind(make_shared<TcpSrvLogic>());

    engine->start();
    srv = nullptr;

    return 0;
}