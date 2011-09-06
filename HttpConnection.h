/**
 * HttpConnection header
 * @author Lucas Tan
 */

#ifndef HTTPCONNECTION_H
#define HTTPCONNECTION_H

#include <string>
#include "Socket.h"
#include "HttpRequest.h"
#include "HttpResponse.h"

/**
* Http Connection class that manages a connection over the HTTP protocol
*
* @author Lucas Tan
*/
class HttpConnection 
{
public:
    HttpConnection(const char* host, int port = 80);
    ~HttpConnection();

    /**
    * Connects to the remote host.
    * @return true if already connected or connected successfully; 
    *         false otherwise.
    */
    bool connect();

    /**
     * Closes the underlying connection immediately.
     */
    void close();

    HttpResponse* send(const HttpRequest& req);

private:
    /** Private copy of the host */
    const std::string host;

    const int port;

    Socket sock;
};

#endif // HTTPCONNECTION_H

