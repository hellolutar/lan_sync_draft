#include "logic.h"
#include "log/log.h"

void LogicWrite::write(std::shared_ptr<uint8_t[]> data, uint64_t size)

{
    if (os_ != nullptr)
        os_->write(data, size);
    else
        WARN_F("LogicWrite::write(): can not send, because the os is not found");
}