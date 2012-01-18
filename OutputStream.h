/**
 * OutputStream class header
 * @author Lucas Tan
 */

#ifndef OUTPUTSTREAM_H
#define OUTPUTSTREAM_H

#include "Common.h"
#include <cstdlib>
#include <cstring>
#include <string>


/**
 * Base class of all output stream classes.
 */
class OutputStream
{
public:
    /**
     * Closes this output stream 
     */
    virtual void close() = 0;

    virtual bool isClosed() const = 0;

    /**
     * Flushes all buffered data to the underlying output sink.
     * @return true if all data is flushed successfully; false otherwise.
     */
    virtual bool flush() = 0;

    /**
     * Writes data to this output stream.
     * @return No. of bytes written; negative on failure
     */
    virtual ssize_t write(const void* data, size_t size) = 0;

    inline ssize_t write(const char* str) 
    {
        // this can be optimized so we don't scan the string twice
        return write(str, strlen(str));
    }

    inline ssize_t write(const std::string& str)
    {
        return write(str.c_str(), str.size());
    }
};

#endif // OUTPUTSTREAM_H

