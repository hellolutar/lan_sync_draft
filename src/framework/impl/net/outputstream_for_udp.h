#ifndef __OUTPUTSTREAM_FOR_UDP_H_
#define __OUTPUTSTREAM_FOR_UDP_H_

#include "framework/itf/net/transport_proto.h"

class OutputstreamForUdp : public OutputStream
{
private:
    NetAddr peer_;
    int sock_;

public:
    OutputstreamForUdp(const NetAddr &p, int sk) : peer_(p), sock_(sk) {}
    ~OutputstreamForUdp() {}

    void write(std::shared_ptr<uint8_t[]> data, const uint64_t size) override;
};

#endif