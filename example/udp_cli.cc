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

class UdpCliLogic : public LogicProto
{
private:
    std::stringstream ss;
    void save()
    {
        auto peg = PersistFramework::getEngine();
        string content = ss.str();
        auto u8arr = str_to_uint8(content);
        peg->saveTo("udp_cli_rcv_file.txt", 0, u8arr, content.size());
        ss.str("");
    }

public:
    UdpCliLogic(/* args */) {}
    ~UdpCliLogic() {}

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

    auto cli = engine->connectWithUdp(NetAddr("127.0.0.1:8080", TransportType::UDP));
    if (cli == 0)
        return -1;

    cli->bind(make_shared<UdpCliLogic>());

    std::shared_ptr<uint8_t[]> data(new uint8_t[2]());
    memcpy(data.get(), "hi", 2);

    cli->write(data, 2);
    engine->start();
    cli = nullptr;

    return 0;
}
