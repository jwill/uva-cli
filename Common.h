/**
 * Common defines 
 *
 * @author Lucas Tan
 */

#ifndef COMMON_H
#define COMMON_H

#if defined(WIN64) || defined(WIN32) || defined(WINDOWS)
    #define OSWIN
    #include <winsock2.h>
    typedef SOCKET sock_t;
    typedef SSIZE_T ssize_t;
#else
    typedef int sock_t;
    #include <unistd.h>
#endif

#define SSIZE_T_MAX ((ssize_t)(((size_t)-1) >> 1))

#endif // COMMON_H


