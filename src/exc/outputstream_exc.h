#ifndef __LANSYNC_EXEC_OUTPUTSTREAM_EXEC_H_
#define __LANSYNC_EXEC_OUTPUTSTREAM_EXEC_H_

#include <stdexcept>
#include <iostream>

class OutputStreamException : public std::exception
{
public:
    OutputStreamException(std::string msg)
    {
        std::cerr << "OutputStreamException" << msg << std::endl;
    }
    ~OutputStreamException() {}
};

#endif