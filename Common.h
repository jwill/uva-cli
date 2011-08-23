/**
Common defines

@author Lucas Tan
*/

#ifndef COMMON_H
#define COMMON_H

#if defined(WIN64) || defined(WIN32) || defined(WINDOWS)
    #define OSWIN
    #include <winsock2.h>
    typedef SOCKET sock_t;
#else
    typedef int sock_t;
#endif

#endif // COMMON_H


