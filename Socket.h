/**
Socket class header

@author Lucas Tan
*/

#ifndef SOCKET_H
#define SOCKET_H

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>

/**
 * TCP-based socket
 */
class Socket
{
public:
    /**
     * Initializes this instance without connecting
     */
    explicit Socket(const char *in_host, int in_port = 80)
        : host(in_host), 
          port(in_port),
          fd(-1),
          num_unread(0),
          unread_idx(0)
    {
        // nothing    
    }
        
    ~Socket();

    /**
     * Connects to the host and port.
     * @return true if connected successfully; false otherwise.
     * @note This will still return true if already connected.
     */
    bool connect();

    /**
     * Writes to the socket safely and will auto retry 
     * on interrupts.
     * @return Number of bytes written.
     */
    size_t write(const void *data, size_t size);

    /**
     * Reads from the socket safely and will auto retry
     * on interrupts.
     * @return Number of bytes read.
     */
    size_t read(void *buf, size_t size);

protected:
    const size_t BUFFER_SIZE = 8192;

    static bool getHostAddr(const char *host, struct in_addr *addr);

private:
    /** Host address */
    const char *const host;

    /** Port */
    const int port;

    /** Zero-based index of the next unread byte */
    size_t unread_idx;

    /** Number of unread bytes */
    size_t num_unread;

    /** Internal buffer for reading */
    char buffer[BUFFER_SIZE];

    /** Socket file descriptor. */
    int fd;
};

#endif // SOCKET_H

