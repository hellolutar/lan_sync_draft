#include <iostream>
#include <cassert>
#include <cstring>

#include "framework/impl/net/event_wrap.h"
#include "framework/itf/net/net_framework.h"
#include "framework/impl/net/net_framework_engine_base_event.h"

using namespace std;

const string SHUTDOWN_STR = "shutdown";

class TcpCliLogic : public LogicWrite
{
private:
    /* data */
public:
    TcpCliLogic(/* args */) {}
    ~TcpCliLogic() {}

    const uint64_t isExtraAllDataNow(std::shared_ptr<uint8_t[]> data, uint64_t data_len) const override
    {
        return data_len;
    };
    void recv(const NetAddr &peer, std::shared_ptr<uint8_t[]> data, uint64_t size) override
    {
        string str(reinterpret_cast<char *>(data.get()));
        str = str.substr(0, size);
        cout << str;

        write(data, size);

        if (str == SHUTDOWN_STR)
        {
            auto eg = Netframework::getEngine();
            eg->shutdown(); // 这里会导致数据还没有发出去
        }
    };
};

int main(int argc, char const *argv[])
{
    cout << "hello world" << endl;

    auto base = make_shared<EventBaseWrap>(event_base_new());

    Netframework::init(make_shared<NetFrameworkEngineBaseEvent>(base));

    auto engine = Netframework::getEngine();

    auto cli = engine->connectWithTcp(NetAddr("127.0.0.1:8080", TransportType::TCP));
    if (cli == 0)
        return -1;

    cli->bind(make_shared<TcpCliLogic>());

    std::shared_ptr<uint8_t[]> data(new uint8_t[2]());
    memcpy(data.get(), "hi", 2);

    cli->write(data, 2);

    engine->start();

    return 0;
}
