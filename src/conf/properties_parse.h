#ifndef __PROPERTIES_PARSE_H_
#define __PROPERTIES_PARSE_H_

#include <string>
#include <map>

using namespace std;

class PropertiesParse
{
private:
    string filepath;
    map<string, string> properties;

public:
    PropertiesParse(string filepath);
    ~PropertiesParse();

    string query(string key);

    static const std::string DISCOVER_IPS;
    static const std::string PROTO_DISCOVER_SERVER_UDP_PORT;
    static const std::string PROTO_SYNC_SERVER_TCP_PORT;
    static const std::string RESOURCE_HOME;
    static const std::string LOG_LEVEL;
};

#endif