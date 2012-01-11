/**
* Implementation of the HttpRequest class
* 
* @author Lucas Tan
*/

#include <map>
#include <string>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <time.h>
#include "HttpRequest.h"
#include "HttpRequestData.h"
#include "OutputStream.h"
#include "MemoryOutputStream.h"
#include "HttpUtil.h"

const std::string HttpRequest::USER_AGENT("User-Agent");
const std::string HttpRequest::HOST("Host");
const std::string HttpRequest::CONNECTION("Connection");
const std::string HttpRequest::KEEP_ALIVE("Keep-Alive");
const std::string HttpRequest::ACCEPT_CHARSET("Accept-Charset");
const std::string HttpRequest::CACHE_CONTROL("Cache-Control");
const std::string HttpRequest::ACCEPT_LANGUAGE("Accept-Language");

bool HttpRequest::write(OutputStream& os) const 
{
    switch(this->method)
    {
    case GET :
        os.write("GET");
        break;
    case POST :
        os.write("POST");
        break;
    }

    os.write(" /");
    os.write(this->path);

    // Writes the query string
    if (this->method == GET)
    {
        os.write("?");
        HttpUtil::urlEncode(this->mydata.values(), os);
    }

    os.write(" HTTP/1.1\r\n");
    
    // write the headers
    for (const_iterator it = this->begin();
         it != this->end(); 
         it++)
    {
        os.write(it->first);
        os.write(": ");
        os.write(it->second);
        os.write("\r\n");
    }
    
    if (this->method == POST)
    {
        if (this->mydata.hasFile())
        {
            char boundary[24];
            sprintf(boundary, "__%08X%08X", (int)time(NULL), 
                    rand() * rand());
            os.write("Content-Type: multipart/form-data; boundary=");
            os.write(boundary);
            os.write("\r\n\r\n");
            return this->mydata.write(os, boundary);
        }
        else
        {
            MemoryOutputStream mos;
            HttpUtil::urlEncode(this->mydata.values(), mos);
            
            os.write("Content-Type: application/x-www-form-urlencoded\r\n");
            os.write("Content-Length: "); 
            HttpUtil::encodeDigits(mos.size(), os);
            os.write("\r\n\r\n");
            os.write(mos.buffer(), mos.size());
            return true;
        }
    }

    os.write("\r\n");
    return true;
}

