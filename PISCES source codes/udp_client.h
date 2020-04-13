//--------------udp_client.h------------------------------------------------
//
//   Author:   Phil Pfeiffer
//   Date:     August 1999
//   Last modified:  April 2020
//
//   Simplify management of a UDP communication
//   by implementing a UDP client endpoint as an object
//
//   A UDP client socket combines
//     the functionality of a UDP socket (i.e., the initialization methods)
//     with the functionality of client sockets (i.e., connect())
//--------------------------------------------------------------------------

#ifndef UDP_CLIENT_H
#define UDP_CLIENT_H

#ifndef SOCKET_UDP_H
#include "socket_udp.h"
#endif

#ifndef SOCKET_CLIENT_H
#include "socket_client.h"
#endif

// *******************************************************************
//  the UDP client socket class proper
//
//  definition of a UDP client socket:
//    a socket that supports UDP init methods + Winsock client methods
//   (i.e., connect())
// *******************************************************************

class udp_client_socket_class : public udp_socket_class, public client_socket_class
{
public:
    // =====================================
    //  constructor
    // =====================================
    //
    //  first form - standard constructor
    //  second form - copy constructor
    //
    udp_client_socket_class(void);
    udp_client_socket_class(const udp_client_socket_class&);

    // ======================================
    //  destructor
    // ======================================
    //
    virtual ~udp_client_socket_class(void);
};

#endif
