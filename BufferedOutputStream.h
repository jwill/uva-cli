/**
 * BufferedOutputStream header
 * @author Lucas Tan
 */

#ifndef BUFFEREDOUTPUTSTREAM_H
#define BUFFEREDOUTPUTSTREAM_H

#include <cassert>
#include <stdlib.h>
#include <stddef.h>
#include "OutputStream.h"

/**
 * A filter that caches the output in a fix-sized buffer before
 * flushing to the underlying output stream.
 */
class BufferedOutputStream : public OutputStream
{
public:
    inline explicit BufferedOutputStream
    (OutputStream& _os, size_t _buf_size = 8192) : 
        buf_size(_buf_size),
        os(_os), 
        cur_size(0)
    {
        this->buf = malloc(buf_size);
        assert(this->buf);
    }

    inline ~BufferedOutputStream()
    {
        this->close();
    }

    /**
     * Flushes as much buffered data as possible to the underlying 
     * output stream and closes this output stream, leaving the 
     * underlying stream still open.
     * Does nothing if already closed.
     */
    inline void close() 
    {
        if (! this->buf) return;
   
        // Ignore the return value
        this->flush();

        free(this->buf);
        this->buf = NULL;
    }

    inline bool isClosed() const { return this->buf == NULL; }

    /**
     * Stores data in the internal buffer and if the buffer is full, writes
     * it to the underlying stream. 
     * @return No. of bytes written; negative on error or if this stream
     *         is already closed.
     */
    ssize_t write(const void* data, size_t size);

    /**
     * Flushes as much data as possible to the underlying output stream.
     * Does nothing if already closed.
     * @return true if all buffered data is flushed successfully; 
     *         false otherwise.
     * @note Even if false is returned, the buffered data will still be
     *       in a consistent state.
     */
    bool flush();

private:
    /** Capacity of the buffer */
    const size_t buf_size;

    /** Underlying output stream */
    OutputStream& os;

    /** Buffer. NULL if closed */
    void* buf;

    /** Current number of valid bytes in the buffer. */
    size_t cur_size;
};

#endif // BUFFEREDOUTPUTSTREAM_H

