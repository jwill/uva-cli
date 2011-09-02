/**
* HttpRequest class header
*
* @author Lucas Tan
*/

#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include <map>
#include <string>
#include "BufferedOutputStream.h"
#include "HttpRequestData.h"

/**
* This class represents an HTTP request, complete with headers,
* POST/GET variables and any file upload data.
*
*/
class HttpRequest
{
public:
    enum Method
    {
        GET, POST // That's all we support for now
    };

private:
    typedef std::map<std::string, std::string> headermap_t;

    /** Http headers in a key-value map */
    headermap_t headers;

    /** Relative path for the request, excluding the query string.
     * Does not have a leading slash */
    std::string path; 
    
    /** The variables and any file data. */
    HttpRequestData mydata;

    /** Request method. */
    Method method;

public:
    typedef headermap_t::const_iterator const_iterator;

    static const std::string USER_AGENT;
    static const std::string HOST;
    static const std::string CONNECTION;
    static const std::string KEEP_ALIVE;
    static const std::string ACCEPT_CHARSET;
    static const std::string CACHE_CONTROL;
    static const std::string ACCEPT_LANGUAGE;

    HttpRequest(const std::string& _path) : 
        path(_path), method(GET) 
    {
        // set a bunch of default headers
        this->headers[ACCEPT_LANGUAGE] = "en-us";
        this->headers[ACCEPT_CHARSET] = "ISO-8859-1,UTF-8";
        this->headers[USER_AGENT] = "Mozilla/5.0 (Windows NT 6.1)";
        this->headers[CACHE_CONTROL] = "no-cache";
    }

    inline HttpRequestData& data() { return this->mydata;  }

    /** Sets the request method */
    inline void setMethod(Method method)
    {
        this->method = method;
    }

    /** Gets the request method */
    inline Method getMethod() const { return this->method; }

    inline const_iterator begin() const 
    {
        return this->headers.begin();
    }

    inline const_iterator end() const
    {
        return this->headers.end();
    }

    /**
    * Check whether a key exists.
    * @param key Key to find
    * @return true if the key exists; false otherwise.
    */
    inline bool has(const std::string& key) const
    {
        return this->headers.find(key) != this->headers.end();
    }

    /**
    * Removes a specified key
    * @return true if the key exists and is removed;
    *         false otherwise.
    */
    inline bool remove(const std::string& key)
    {
        return this->headers.erase(key) > 0;
    }

    inline std::string& operator[](const std::string& key) 
    {
        return this->headers[key];
    }

    /**
     * Writes this request to an output stream.
     * @return true if everything is written successfully;
     *         false otherwise.
     */
    bool write(OutputStream& os) const;
};

#endif // HTTPREQUEST_H

