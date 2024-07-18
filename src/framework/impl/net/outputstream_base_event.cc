#include "outputstream_base_event.h"

void OutputstreamBaseEvent::write(std::shared_ptr<uint8_t[]> data,
                                  const uint64_t size)
{
    bufferevent_write(buf_->getBuf(), data.get(), size);
}