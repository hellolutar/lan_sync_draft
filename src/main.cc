
#include "end_point.h"
#include "conf/usage.h"
#include "conf/properties_parse.h"
#include "framework/itf/net/net_framework.h"
#include "framework/impl/net/net_framework_engine_base_event.h"
#include "framework/itf/timer/timer_trigger_framework.h"
#include "framework/impl/timer/timer_framework_engine_base_event.h"

using namespace std;

int main(int argc, char *argv[])
{
    Usage u(argc, argv);
    u.parse();

    PropertiesParse p(FLAGS_config_path);

    auto _base = make_shared<EventBaseWrap>(event_base_new());
    auto _eg = make_shared<NetFrameworkEngineBaseEvent>(_base);
    Netframework::init(_eg);

    auto timer_ = make_shared<TimerFrameworkEngineBaseEvent>(_base);
    TimerFramework::init(timer_);

    Endpoint ed;
    ed.setResourceManager(make_shared<ResourceManagerBaseFilesystem>(p.query(PropertiesParse::RESOURCE_HOME)));
    
    ed.init();
    ed.run();

    return 0;
}
