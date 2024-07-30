#include "usage.h"

#include <filesystem>
#include <iostream>

using namespace std;

DEFINE_string(config_path, "properties.properties", "Location of the software configuration file");

static bool ValidateConfigPath(const char *flagname, const std::string &value)
{

    if (value.size() == 0)
        return false;

    filesystem::path p(value);
    if (!filesystem::exists(p))
    {
        cout << "ERROR: NOTFOUND: --config_path: " << value << endl;
        return false;
    }

    if (!filesystem::is_regular_file(value)){
        cout << "ERROR: NOT REGULAR FILE: --config_path: " << value << endl;
        return false;
    }

    return true;
}

DEFINE_validator(config_path, &ValidateConfigPath);

void Usage::parse()
{
    gflags::SetUsageMessage("usage description");
    gflags::SetVersionString("v0.0.1");
    gflags::ParseCommandLineFlags(&argc_, &argv_, false);
}
