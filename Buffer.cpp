/**
Buffer class implementation

@author Lucas Tan
*/

#include <cassert>
#include "Buffer.h"

Buffer::Buffer()
{
    this->cur_size = 0;
    this->cap = 128;
    this->buf = malloc(this->cap);
    assert(this->buf);
}

Buffer::Buffer(size_t init_cap)
{
    this->cur_size = 0;
    this->cap = init_cap == 0 ? 128 : init_cap;
    this->buf = malloc(this->cap);
    assert(this->buf);
}

Buffer::~Buffer()
{
    free(this->buf);
    this->buf = NULL;
}

bool Buffer::append(const void *data, size_t size)
{
    if (size > this->cap - this->cur_size)
    {
        size_t new_cap = this->cap * 2;
        while (size > new_cap - this->cur_size)
        {
            new_cap *= 2;
        }

        void *new_buf = realloc(this->buf, new_cap);
        if (!new_buf) return false;

        this->buf = new_buf;
        this->cap = new_cap;
    }

    // Append data to buffer
    memcpy((char*)this->buf + this->cur_size, 
            data, size);
    this->cur_size += size;
    return true;
}


