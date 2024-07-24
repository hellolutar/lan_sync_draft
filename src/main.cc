
#include "end_point.h"
#include "conf/usage.h"
#include "conf/properties_parse.h"
#include "framework/itf/net/net_framework.h"
#include "framework/impl/net/net_framework_engine_base_event.h"
#include "framework/itf/timer/timer_trigger_framework.h"
#include "framework/impl/timer/timer_framework_engine_base_event.h"
#include "framework/impl/persist/persist_framework_egine_filesystem.h"

using namespace std;

int main(int argc, char *argv[])
{
    Usage u(argc, argv);
    u.parse();

    auto _base = make_shared<EventBaseWrap>(event_base_new());
    auto _eg = make_shared<NetFrameworkEngineBaseEvent>(_base);
    Netframework::init(_eg);

    auto timer_ = make_shared<TimerFrameworkEngineBaseEvent>(_base);
    TimerFramework::init(timer_);

    auto persister_ = make_shared<PersistFrameworkEgineImplWithFileSystem>();
    PersistFramework::init(persister_);

    Endpoint ed;

    auto conf = make_shared<PropertiesParse>(FLAGS_config_path);
    ed.setResourceManager(make_shared<ResourceManagerBaseFilesystem>(conf->query(PropertiesParse::RESOURCE_HOME)));
    ed.setPropertiesParse(conf);

    ed.init();
    ed.run();

    return 0;
}
