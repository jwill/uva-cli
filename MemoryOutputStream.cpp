/**
* MemoryOutputStream class implementation
* 
* @author Lucas Tan
*/

#include <cassert>
#include <stdlib.h>
#include <string.h>
#include "MemoryOutputStream.h"

MemoryOutputStream::MemoryOutputStream()
{
    this->cur_size = 0;
    this->cap = 128;
    this->buf = malloc(this->cap);
    assert(this->buf);
}

MemoryOutputStream::MemoryOutputStream(size_t init_cap)
{
    this->cur_size = 0;
    this->cap = init_cap == 0 ? 128 : init_cap;
    this->buf = malloc(this->cap);
    assert(this->buf);
}

MemoryOutputStream::~MemoryOutputStream()
{
    this->close();
}

ssize_t MemoryOutputStream::write(const void* data, size_t size)
{
    // Impose a limit of 1 GB to prevent arithmetic
    // overflow and also because we can only return a type of ssize_t
    if (! this->buf || size > 1024*1024*1024) return -1;

    if (size > this->cap - this->cur_size)
    {
        size_t new_cap = this->cap * 2;
        while (size > new_cap - this->cur_size)
        {
            new_cap *= 2;
        }

        void* new_buf = realloc(this->buf, new_cap);
        if (!new_buf) return -1;

        this->buf = new_buf;
        this->cap = new_cap;
    }

    // Append data to buffer
    memcpy((char*)this->buf + this->cur_size, data, size);
    this->cur_size += size;
    return size;
}


