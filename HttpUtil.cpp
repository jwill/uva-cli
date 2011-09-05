/**
 * HttpUtil class implementation.
 * @author Lucas Tan
 */ 

#include <string>
#include <map>
#include "HttpUtil.h"
#include "OutputStream.h"

static const char* const HEX = "0123456789ABCDEF";

size_t HttpUtil::urlEncode(const std::string& str, OutputStream& os)
{
    size_t i = 0;
    size_t written = 0;
    const size_t size = str.size();
    char buf[4];
    buf[0] = '%';

    for ( ; i < size; i++)
    {
        const char c = str[i];
        // includes spaces too
        if (c <= 32 || c > 126 || c == '&' || c == '=' 
          || c == '+' || c == '?')
        {
            buf[1] = HEX[(c / 16) & 0xf];
            buf[2] = HEX[c & 0xf];
            written += os.write(buf, 3);
        }
        else
        {
            written += os.write(&c, 1);
        }
    }

    return written;
}

typedef std::map<std::string,std::string> values_t;

size_t HttpUtil::urlEncode(
    const values_t& values,
    OutputStream& os)
{
    size_t written = 0;
    const char equal = '=';
    const char amp = '&';

    values_t::const_iterator it = values.begin();
    for (; it != values.end(); it++)
    {
        if (written)
        {
            written += os.write(&amp, 1);
        }
        written += urlEncode(it->first, os);
        written += os.write(&equal, 1);
        written += urlEncode(it->second, os);
    }

    return written;
}

size_t HttpUtil::encodeDigits(size_t num, OutputStream& os)
{
    char digits[30];
    size_t num_digits =  0;
    size_t v = num;
    do
    {
        v /= 10;
        num_digits++;
    }
    while (v);

    size_t i = num_digits - 1;
    v = num;
    do{
        digits[i] = (v % 10) + '0';
        v /= 10;
        i--;
    } while (v);

    return os.write(digits, num_digits);
}

