/**
 * BufferedOutputStream class implementation
 * @author Lucas Tan
 */

#include <cstdlib>
#include <cstring>
#include "BufferedOutputStream.h"

ssize_t BufferedOutputStream::write(const void* data, size_t size)
{
    if (! this->buf) return -1;

    size_t left = size;
    char* data_ptr = (char*)data;
    while (left)
    {
        // copy data over to our buffer
        const size_t space_left = this->buf_size - this->cur_size;
        const size_t to_copy = (left > space_left ? space_left : left);
        memcpy((char*)this->buf + this->cur_size, data_ptr, to_copy);
        this->cur_size += to_copy;

        left -= to_copy;
        data_ptr += to_copy;
        
        // if buffer is full, then write the entire buffer
        if (this->cur_size == this->buf_size)
        {
            ssize_t written = this->os.write(this->buf, this->cur_size);
            if (written <= 0) break;
            
            this->cur_size -= written;
            memmove(this->buf, (char*)this->buf + written, this->cur_size);
        }
    }

    return size - left;
}

bool BufferedOutputStream::flush()
{
    if (! this->buf) return false;

    ssize_t total_written = 0;

    // Write as much data as possible before flushing
    while (this->cur_size)
    {
        ssize_t written = this->os.write(
            (char*)this->buf + total_written, this->cur_size);
    
        if (written < 0) 
        {
            // this memmove could be avoided if we used a circular
            // buffer implementation instead...
            memmove(this->buf, (char*)this->buf + total_written, 
                this->cur_size);
            return false;
        }

        this->cur_size -= written;
        total_written += written;
    }

    return this->os.flush();
}


