#include "outputstream_for_udp.h"

void OutputstreamForUdp::write(std::shared_ptr<uint8_t[]> data, const uint64_t size)
{
    auto be_addr = peer_.sockaddrV4();
    sendto(sock_, data.get(), size, 0, (struct sockaddr *)&be_addr, sizeof(struct sockaddr_in));
}
