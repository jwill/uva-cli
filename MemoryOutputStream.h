/**
 * MemoryOutputStream class header
 * @author Lucas Tan
 */

#ifndef MEMORYOUTPUTSTREAM_H
#define MEMORYOUTPUTSTREAM_H

#include <string.h>
#include <string>
#include "OutputStream.h"

/**
 * A memory-backed buffer that extends automatically by doubling its 
 * capacity whenever it runs out of free space.
 */
class MemoryOutputStream : public OutputStream
{
public:
    explicit MemoryOutputStream();
    explicit MemoryOutputStream(size_t init_cap);
    ~MemoryOutputStream();

    /**
     * Appends data to this buffer.
     * @param data Pointer to the data to append
     * @param size Size of the data in bytes.
     * @return either value of 'size' if append is successful or
     *         negative if out of memory.
     * @note The current data in the buffer won't be modified if 
     *       a negative value is returned. This function is all-or-none,
     *       that is, either all or none of the data will be appended.
     */
    ssize_t write(const void* data, size_t size);

    inline bool flush() 
    { 
        // Nowhere to flush to
        return this->buf != NULL; 
    }

    inline void close() 
    {
        if (this->buf == NULL) return;

        free(this->buf);
        this->buf = NULL;
        this->cap = 0;
        this->cur_size = 0;
    }

    inline bool isClosed() const { return this->buf == NULL; }

    /**
     * Clears the content of the memory buffer
     */
    inline void clear() { this->cur_size = 0; }

    /**
     * Gets the current size of data in the buffer, in bytes.
     * @return current size if this stream is still open; zero if closed.
     */
    inline size_t size() const { return this->cur_size; }

    /**
     * Gets a pointer to the internal buffer space.
     * @return valid pointer if this stream is still open; NULL if closed.
     */
    inline const void* buffer() const { return this->buf; }

private:
    /** Pointer to the buffer space. NULL if closed. */
    void* buf;

    /** Capacity of the buffer */
    size_t cap;

    /** Current size of data in the buffer, in bytes */
    size_t cur_size;
};

#endif // MEMORYOUTPUTSTREAM_H

