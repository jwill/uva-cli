/**
 * HttpUtil header
 * @author Lucas Tan
 */

#ifndef HTTPUTIL_H
#define HTTPUTIL_H

#include <stdlib.h>
#include <string>
#include <map>
#include "OutputStream.h"

/**
 * Http utilities.
 */
class HttpUtil
{
public:
    /**
     * Encodes a string in the URL encoded format.
     * @param str String to be encoded
     * @param os Encoded string will be appended to this stream.
     * @return Number of characters written.
     */
    static size_t urlEncode(const std::string& str, OutputStream& os);

    /**
     * Encodes a map of name-values pairs, in no particular order.
     * @return Number of characters written.
     */
    static size_t urlEncode(const std::map<std::string,std::string>& values,
                            OutputStream& os);

    /**
     * Encodes the digits of a size_t value
     * @return Number of characters written.
     */
    static size_t encodeDigits(size_t num, OutputStream& os);
};

#endif // HTTPUTIL_H

