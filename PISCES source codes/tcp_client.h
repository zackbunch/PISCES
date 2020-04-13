//--------------tcp_client.h-------------------------------------------------
//
//   Author:   Phil Pfeiffer
//   Date:     August 1999
//   Last modified:  April 2020
//
//   Simplify management of a TCP communication
//   by implementing a TCP client endpoint as an object
//
//   A TCP client socket combines
//     the functionality of a TCP socket (i.e., the initialization methods)
//     with the functionality of client sockets (i.e., connect())
//
//--------------------------------------------------------------------------

#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#ifndef SOCKET_CLIENT_H
#include "socket_client.h"
#endif

#ifndef SOCKET_TCP_H
#include "socket_tcp.h"
#endif

// *******************************************************************
//  the TCP client socket class proper
//
//  definition of a TCP client socket:
//    a socket that supports TCP init methods + socket client methods
//    (i.e., connect())
//
// *******************************************************************

// **** important: in inheritance list below,
// ****     tcp_socket_class must precede client_socket_class
// ****     to ensure inheritance of the proper sendto()/recvfrom() methods

class tcp_client_socket_class : public tcp_socket_class, public client_socket_class
{
public:
    // =====================================
    //  constructor
    // =====================================
    //
    //  first form - standard constructor
    //  second form - copy constructor
    //
    tcp_client_socket_class(void);
    tcp_client_socket_class(const tcp_client_socket_class&);

    // ======================================
    //  destructor
    // ======================================
    //
    virtual ~tcp_client_socket_class(void);
};

#endif
