/**
 * HttpRequestData implementation
 * @author Lucas Tan
 *
 */

#include "Common.h"
#include <string.h>
#include <stdio.h>
#include "HttpRequestData.h"
#include "HttpUtil.h"

static const size_t BUF_SIZE = 16*1024;

bool HttpRequestData::write(OutputStream& os, const char* boundary) const
{
    std::string bound("--");
    bound.append(boundary);

    values_t::const_iterator it = this->myvalues.begin();
    for (; it != this->myvalues.end(); it++)
    {
        os.write(bound);
        os.write("\r\nContent-Disposition: form-data; name=\"");
        HttpUtil::urlEncode(it->first, os); // are we supposed to url encode
        os.write("\"\r\n\r\n");
        HttpUtil::urlEncode(it->second, os);
        os.write("\r\n");
    }

    bool ok = true;
    if (this->myfiles.size())
    {
            char* buf = new (std::nothrow) char[BUF_SIZE];
            if (! buf)
            {
                // at least ensure we terminate the request properly
                os.write(bound);
                os.write("--\r\n");
                return false;
            }

            it = this->myfiles.begin();
            for (; it != this->myfiles.end(); it++)
            {
                // open the file first since it has the highest chances to fail
                FILE *f = fopen(it->second.c_str(), "rb");
                if (!f) { ok = false; break; }

                os.write(bound);
                // TODO : write filename="xxx"
                os.write("\r\nContent-Disposition: form-data; name=\"");
                HttpUtil::urlEncode(it->first, os);
                // We don't care about the mime-type
                os.write("\"\r\nContent-Transfer-Encoding: binary\r\n"
                         "Content-Type: application/octet-stream\r\n\r\n");

                size_t num_read;
                while ((num_read = fread(buf, 1, BUF_SIZE, f)) > 0)
                {
                    os.write(buf, num_read);
                }

                os.write("\r\n");

                // Did we encounter EOF or error ?
                if (! feof(f)){ fclose(f); ok = false; break;}
                fclose(f);
            }

            delete[] buf;
    }

    // at least ensure we terminate the request properly
    os.write(bound);
    os.write("--\r\n");
    return ok;
}


