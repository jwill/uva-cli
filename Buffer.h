/**
Buffer class header

@author Lucas Tan
*/

#ifndef BUFFER_H
#define BUFFER_H

#include<stdlib.h>
#include<string.h>
#include<string>

/**
A buffer that extends automatically by doubling its capacity whenever
it runs out of free space.
*/
class Buffer
{
public:
    explicit Buffer();
    explicit Buffer(size_t init_cap);
    ~Buffer();

    /**
     * Appends data to this buffer.
     * @param data Pointer to the data to append
     * @param size Size of the data in bytes.
     * @return true if append is successful; 
     *         false if out of memory.
     * @note The current data in the buffer won't be modified if 
     *       false is returned.
     */
    bool append(const void *data, size_t size);

    bool append(const char *str) 
    {
        // this can be optimized so we don't scan the string twice
        return append(str, strlen(str));
    }

    bool append(const std::string& str)
    {
        return append(str.c_str(), str.size());
    }

    /**
     * Gets the current size of data in the buffer, in bytes.
     */
    inline size_t size() const { return this->cur_size; }

    /**
     * Gets a pointer to the internal buffer space.
     */
    inline const void *buffer() const { return this->buf; }

private:
    /** Pointer to the buffer space */
    void *buf;

    /** Capacity of the buffer */
    size_t cap;

    /** Current size of data in the buffer, in bytes */
    size_t cur_size;
};

#endif // BUFFER_H

