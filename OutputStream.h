/**
 * OutputStream class header
 * @author Lucas Tan
 */

#ifndef OUTPUTSTREAM_H
#define OUTPUTSTREAM_H

#include "Common.h"
#include <stdlib.h>
#include <string>
#include <string.h>

/**
 * Base class of all output stream classes.
 */
class OutputStream
{
public:
    /**
     * Closes this output stream 
     */
    virtual void close();

    virtual bool isClosed() const;

    /**
     * Flushes all buffered data to the underlying output sink.
     * @return true if all data is flushed successfully; false otherwise.
     */
    virtual bool flush();

    /**
     * Writes data to this output stream.
     * @return No. of bytes written; negative on failure
     */
    virtual ssize_t write(const void* data, size_t size);

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

