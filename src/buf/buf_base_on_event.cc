#include "buf/buf_base_on_event.h"

BufBaseonEvent::BufBaseonEvent(/* args */)
{
    buf = evbuffer_new();
}

BufBaseonEvent::~BufBaseonEvent()
{
    if (buf != nullptr)
        evbuffer_free(buf);
}

uint64_t BufBaseonEvent::add(uint8_t *data_in, uint64_t len)
{
    if (evbuffer_add(buf, data_in, len) == -1)
    {
        return 0;
    }
    return len;
}

std::shared_ptr<uint8_t[]> BufBaseonEvent::data()
{
    if (d != nullptr)
        d = nullptr;

    uint64_t len = evbuffer_get_length(buf);

    std::shared_ptr<uint8_t[]> p(new uint8_t[len]());
    d = std::move(p);
    uint64_t act_len = evbuffer_copyout(buf, d.get(), len);
    if (act_len != len)
    {
        // LOG_WARN("BufBaseonEvent::data():  act_len != len : {} != {}", act_len, len);
    }
    return d;
}

uint64_t BufBaseonEvent::size()
{
    return evbuffer_get_length(buf);
}
