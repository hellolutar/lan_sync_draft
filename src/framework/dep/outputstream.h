#ifndef __FRAMEWORK_DEP_OUTPUTSTREAM_H_
#define __FRAMEWORK_DEP_OUTPUTSTREAM_H_

class OutputStream
{
private:
    bool closed_ = true;

public:
    OutputStream() {}
    OutputStream(bool closed) : closed_(closed) {}
    virtual ~OutputStream() {}

    virtual void close();
    virtual bool isClosed();
    virtual void write(std::shared_ptr<uint8_t[]> data, const uint64_t size) = 0;
};



#endif