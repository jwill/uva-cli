/**
Socket class implementation.

@author Lucas Tan
*/

#include "Socket.h"

Socket::~Socket()
{
    if (this->fd >= 0)
    {
        close(this->fd);
        this->fd = -1;
    }
}

bool Socket::getHostAddr(const char *host, struct in_addr *addr)
{
    struct hostent *entry;
    int result;


}

bool Socket::connect()
{
    // return true if already connected
    if (this->fd >= 0) return true;

    struct sockaddr_in server_addr;

    memset(&server_addr, 0, sizeof(server_addr));

    if (! getHostAddr(this->host, &server_addr.sin_addr))
    {
        return false;
    }

    this->fd = socket(AF_INET, SOCK_STREAM, 0);
    if (this->fd < 0)
    {
        return false;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(this->port);

    int result = connect(this->fd, (struct sockaddr*)&server_addr,
                    sizeof(server_addr));
    if (result < 0)
    {
        close(this->fd);
        this->fd = -1;
        return false;
    }

    return true;
}

size_t Socket::write(const void *data, size_t size)
{
    
}




