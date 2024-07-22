#include <iostream>
#include <cassert>
#include <cstring>

#include "framework/impl/net/event_wrap.h"
#include "framework/itf/net/net_framework.h"
#include "framework/impl/net/net_framework_engine_base_event.h"

using namespace std;

const string CLOSE_STR = "close";

class TcpCliLogic : public LogicWrite
{
private:
    /* data */
public:
    TcpCliLogic(/* args */) {}
    ~TcpCliLogic() {}

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
    };
};

int main(int argc, char const *argv[])
{
    cout << "hello world" << endl;

    auto _base = make_shared<EventBaseWrap>(event_base_new());
    auto _eg = make_shared<NetFrameworkEngineBaseEvent>(_base);
    Netframework::init(_eg);

    auto engine = Netframework::getEngine();

    auto cli = engine->connectWithTcp(NetAddr("127.0.0.1:8080", TransportType::TCP));
    if (cli == 0)
        return -1;

    cli->bind(make_shared<TcpCliLogic>());

    std::shared_ptr<uint8_t[]> data(new uint8_t[2]());
    memcpy(data.get(), "hi", 2);

    cli->write(data, 2);
    engine->start();
    cli = nullptr;

    return 0;
}
