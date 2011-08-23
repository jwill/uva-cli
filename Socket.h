/**
Socket class header

@author Lucas Tan
*/

#ifndef SOCKET_H
#define SOCKET_H

#include "Common.h"
#include <stdlib.h>
#include <stdio.h>
#include <string>

#ifndef OSWIN
    #include <signal.h>
    #include <sys/socket.h>
    #include <sys/types.h>
    #include <sys/wait.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <netinet/in.h>
#endif

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
          fd(0),
          has_fd(false)
    {
        // nothing    
    }
        
    ~Socket();

    /**
     * Connects to the host and port.
     * @return true if connected successfully; false otherwise.
     * @note This will still return true even if already connected.
     */
    bool connect();

    /**
     * Writes to the socket safely and will auto retry 
     * on interrupts.
     * @return Number of bytes written; negative on error
     */
    int write(const void *data, int size);

    /**
     * Reads from the socket safely and will auto retry
     * on interrupts.
     * @return Number of bytes read; negative on error
     */
    int read(void *buf, int size);

    #ifdef OSWIN
    inline static bool isValid(sock_t sock) { return sock != INVALID_SOCKET; }
    inline static void close(sock_t sock) { closesocket(sock); }
    #else
    inline static bool isValid(sock_t sock) { return sock >= 0; }
    inline static void close(sock_t sock) { close(sock); }
    #endif

protected:
    static bool getHostAddr(const char *host, struct in_addr *addr);

private:
    /** Private copy of host */
    const std::string host;

    /** Port number */
    const int port;

    /** Socket file descriptor. */
    sock_t fd;

    /** Whether fd is valid */
    bool has_fd;
};

#endif // SOCKET_H

