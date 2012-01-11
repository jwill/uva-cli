/**
 * HttpRequestData header
 * @author Lucas Tan
 */

#ifndef HTTPREQUESTDATA_H
#define HTTPREQUESTDATA_H

#include <cstdlib>
#include <cstring>
#include <string>
#include <map>
#include "OutputStream.h"

/**
 * Represents the POST/GET variables and any file data.
 *
 */
class HttpRequestData
{
public:
    typedef std::map<std::string, std::string> values_t;

private:
    std::map<std::string,std::string> myvalues;
    std::map<std::string,std::string> myfiles;

public:
    inline HttpRequestData() { /* nothing */ }

    inline void addValue(const std::string& key, const std::string& value)
    {
        this->myvalues[key] = value;
    }

    inline void addFile(const std::string& key, const std::string& fileName)
    {
        // Allows collision of key names with values' key names
        this->myfiles[key] = fileName;
    }

    inline values_t& values() const 
        { return const_cast<values_t&>(this->myvalues); }

    inline values_t& files() const 
        { return const_cast<values_t&>(this->myfiles); }

    /**
     * Checks whether there is any file data
     */
    inline bool hasFile() const { return this->myfiles.size() > 0; }

    /**
     * Writes the variables and file data to an output stream as if
     * the request method is POST.
     * @return true if everything is written successfully; false otherwise.
     * @note Sources of errors: stream error, file I/O error
     */
    bool write(OutputStream& os, const char* boundary) const;
};

#endif // HTTPREQUESTDATA_H

