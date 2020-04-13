//--------------udp_peer.h------------------------------------------------------------------
//
//   Author:   Phil Pfeiffer
//   Date:     August 1999
//   Last modified:  April 2020
//
//   Simplify management of a UDP communication
//   by implementing a UDP peer endpoint as an object
//
//   A UDP server socket combines
//     the functionality of a UDP socket (i.e., the initialization methods)
//     with the functionality of client and server sockets i.e., connect(), bind())
//-------------------------------------------------------------------------------------------

#ifndef UDP_PEER_H
#define UDP_PEER_H

#ifndef SOCKET_UDP_H
#include "socket_udp.h"
#endif

#ifndef SOCKET_CLIENT_H
#include "socket_client.h"
#endif

#ifndef SOCKET_SERVER_H
#include "socket_server.h"
#endif

// ********************************************************************************
//  the UDP peer socket class proper
//
//  definition of a UDP peer socket:
//    a socket that supports UDP init methods + Winsock client and server methods
//
// ********************************************************************************

class udp_peer_socket_class : public udp_socket_class, public server_socket_class, public client_socket_class
{
public:
    // =====================================
    //  constructor
    // =====================================
    //
    //  first form - standard constructor
    //  second form - copy constructor
    //
    udp_peer_socket_class(void);
    udp_peer_socket_class(const udp_peer_socket_class&);

    // ======================================
    //  destructor
    // ======================================
    //
    virtual ~udp_peer_socket_class(void);
};

#endif
