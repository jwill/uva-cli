/**
Socket class implementation.

@author Lucas Tan
*/

#include "Socket.h"

#ifdef OSWIN
    // For initializing the Win Sock lib.
    class Init 
    {
    public:
        Init() 
        { 
            // errors are rare so we ignore the return value
            // Also if error initializing, creating sockets will fail
            // so no big deal.
            WSADATA wsa = {0}; 
            WSAStartup(MAKEWORD(2, 2), &wsa); 
        }
        
        ~Init () 
        {  
            WSACleanup();            
        }
    };

    static Init init;
#else
    #include <errno.h>
#endif

Socket::~Socket()
{
    if (this->has_fd)
    {
        Socket::close(this->fd);
        this->has_fd = false;
    }
}

bool Socket::getHostAddr(const char *host, struct in_addr *addr)
{
    #ifdef OSWIN

    // gethostbyname is thread-safe because it uses thread local storage
    struct hostent *entry = gethostbyname(host);
    
    #else

    struct hostent storage;
    int h_error;
    struct hostent *entry = NULL;
    char aux[1024]; // 1024 should be big enough

    // thread-safe version
    int result = gethostbyname_r(host, &storage, aux, sizeof(aux),
                        &entry, &h_error);

    if (result < 0) return false;

    #endif

    if (! entry) return false;

    memcpy(addr, entry->h_addr_list[0], entry->h_length);
    
    return true;
}

bool Socket::connect()
{
    // return true if already connected
    if (this->has_fd) return true;

    struct sockaddr_in server_addr;

    memset(&server_addr, 0, sizeof(server_addr));

    if (! getHostAddr(this->host.c_str(), &server_addr.sin_addr))
    {
        return false;
    }

    this->fd = socket(AF_INET, SOCK_STREAM, 0);
    if (! Socket::isValid(this->fd))
    {
        return false;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(this->port);

    int result = ::connect(this->fd, (struct sockaddr*)&server_addr,
                    sizeof(server_addr));
    if (result < 0)
    {
        Socket::close(this->fd);
        this->has_fd = false;
        return false;
    }

    this->has_fd = true;
    return true;
}

int Socket::write(const void *data, int size)
{
    if (size < 0) return -1;

    int num_left = size;
    ssize_t num_written = 0;
    const char *cur_ptr = (const char*)data;

    while (num_left)
    {
    #ifdef OSWIN
        num_written = send(this->fd, cur_ptr, num_left, 0);
    #else    
        num_written = send(this->fd, cur_ptr, num_left, MSG_NOSIGNAL);
    #endif
        if (num_written <= 0)
        {
            #ifndef OSWIN
            if (errno == EINTR)
            {
                // continue if we were interrupted by a signal
                continue;
            }
            #endif
            // there's an error
            return -1;
        }
        num_left -= num_written;
        cur_ptr += num_written;
    }
    return size;
}

int Socket::read(void *data, int size)
{
    if (size < 0) return -1;

    int num_to_read = size;
    ssize_t num_read;
    char *cur_ptr = (char*)data;

    while (num_to_read)
    {
        num_read = recv(this->fd, cur_ptr, num_to_read, 0);
        if (num_read < 0)
        {
            #ifdef OSWIN
            return -1;
            #else
            if (num_read != EINTR) return -1;
            #endif
        }
        else if (num_read == 0) 
            return size - num_to_read; // End of file
        else
        {
            cur_ptr += num_read;
            num_to_read -= num_read;
        }
    }

    return size - num_to_read;
}


