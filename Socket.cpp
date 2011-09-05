/**
* Socket class implementation.
*
* @author Lucas Tan
*/

#include "Common.h"
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

    static Init __init;
#else
    #include <errno.h>
#endif

Socket::~Socket()
{
    this->close();
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

ssize_t Socket::write(const void *data, size_t size)
{
    ssize_t cap_size = size & SSIZE_T_MAX;
    ssize_t num_left = cap_size;
    const char *cur_ptr = (const char*)data;

    while (num_left)
    {
    #ifdef OSWIN
        ssize_t num_written = send(this->fd, cur_ptr, num_left, 0);
    #else    
        ssize_t num_written = send(this->fd, cur_ptr, num_left, MSG_NOSIGNAL);
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

            // if we have written nothing so far, then
            // report as error; else report the no. of bytes written
            return num_left == cap_size ? -1 : cap_size - num_left;
        }
        num_left -= num_written;
        cur_ptr += num_written;
    }

    return cap_size;
}

ssize_t Socket::read(void *data, size_t size)
{
    ssize_t cap_size = size & SSIZE_T_MAX;
    ssize_t num_to_read = cap_size;
    char *cur_ptr = (char*)data;

    while (num_to_read)
    {
        ssize_t num_read = recv(this->fd, cur_ptr, num_to_read, 0);
        if (num_read < 0)
        {
            #ifndef OSWIN
            if (errno == EINTR)  continue;
            #endif
            
            // If we have read nothing so far, then report as error
            // else report no. of bytes read.
            return (num_to_read == cap_size ? -1 :
                    cap_size - num_to_read);
        }
        else if (num_read == 0) 
            return cap_size - num_to_read; // End of file
        else
        {
            cur_ptr += num_read;
            num_to_read -= num_read;
        }
    }

    return cap_size;
}


