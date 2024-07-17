#ifndef __OUTPUTSTREAM_BASE_EVENT_H_
#define __OUTPUTSTREAM_BASE_EVENT_H_


#include "event_wrap.h"
#include "framework/itf/net/transport_proto.h"


class OutputstreamBaseEvent : public OutputStream
{
private:
    std::shared_ptr<BuffereventWrap> buf_;

public:
    OutputstreamBaseEvent(std::shared_ptr<BuffereventWrap> buf)
        : buf_(buf) {}
    ~OutputstreamBaseEvent() {}

    void write(std::shared_ptr<uint8_t[]> data, const uint64_t size) override;
};

#endif