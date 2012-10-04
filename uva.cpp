/**
 * Main entry point
 * @author Lucas Tan 
 */

#include <web/HttpRequest.hpp>
#include <net/Socket.hpp>
#include <string>
#include <iostream>
#include <cstdio>
#include <cstdlib>

using namespace cpp::net;
using namespace cpp::web;

void test()
{
    Socket s("www.yahoo.com");   
    s.connect();
      
    std::cout << "connected" << std::endl;

    HttpRequest req("/");
    req.write(s);

    std::cout << "sent request" << std::endl;
    
    ssize_t len =0;
    char buf[1024];
    while ((len = s.read(buf, sizeof(buf) - 1)) > 0)
    {
        buf[len] = 0;
        std::cout << buf;
    }
    
    // TODO : handle chunked transfer encoding, otherwise use http 1.0
}

int main(int argc, char **argv)
{
    test();
}

