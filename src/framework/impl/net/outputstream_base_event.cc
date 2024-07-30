#include "outputstream_base_event.h"
#include "exc/outputstream_exc.h"

void OutputstreamBaseEvent::write(std::shared_ptr<uint8_t[]> data,
                                  const uint64_t size)
{
    if (buf_->isRelease())
    {
        closed_ = true;
        throw OutputStreamException("is closed");
    }
    
    bufferevent_write(buf_->getBuf(), data.get(), size);
}