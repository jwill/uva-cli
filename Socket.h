/**
* Socket class header
*
* @author Lucas Tan
*/

#ifndef SOCKET_H
#define SOCKET_H

#include "Common.h"
#include <cstdlib>
#include <cstdio>
#include <string>
#include "OutputStream.h"

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
class Socket : public OutputStream
{
public:
    #ifdef OSWIN
    inline static bool isValid(sock_t sock){ return sock != INVALID_SOCKET; }
    inline static void close(sock_t sock){ closesocket(sock); }
    #else
    inline static bool isValid(sock_t sock){ return sock >= 0; }
    inline static void close(sock_t sock){ close(sock); }
    #endif

    /**
     * Initializes this instance without connecting
     */
    explicit Socket(const char* _host, int _port = 80)
        : host(_host), 
          port(_port),
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
    ssize_t write(const void* data, size_t size);

    /**
     * Reads from the socket safely and will auto retry
     * on interrupts.
     * @return Number of bytes read; zero on EOF; negative on error
     */
    ssize_t read(void* buf, size_t size);

    inline bool flush() 
    {
        // We let the OS decide when to flush
        return this->has_fd; 
    }

    inline void close() 
    { 
        if (this->has_fd)
        {
            close(this->fd); 
            this->has_fd = false; 
        }
    }

    inline bool isClosed() const { return !this->has_fd; }

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

