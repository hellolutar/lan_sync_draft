#ifndef __PERSIST_FRAMEWORK_H_
#define __PERSIST_FRAMEWORK_H_

#include <memory>

class PersistFrameworkEgine
{
public:
    PersistFrameworkEgine(/* args */) {}
    virtual ~PersistFrameworkEgine() {}
};

class PersistFramework
{
protected:
    static std::shared_ptr<PersistFrameworkEgine> engine_;

public:
    PersistFramework(/* args */) {}
    virtual ~PersistFramework() {}

    static void init(std::shared_ptr<PersistFrameworkEgine> eg);
    static std::shared_ptr<PersistFrameworkEgine> getEngine();
};

#endif