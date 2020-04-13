//--------------udp_server.h-----------------------------------------------
//
//   Author:   Phil Pfeiffer
//   Date:     August 1999
//   Last modified:  April 2020
//
//   Simplify management of a UDP communication
//   by implementing a UDP server endpoint as an object
//
//   A UDP server socket combines
//     the functionality of a UDP socket (i.e., the initialization methods)
//     with the functionality of server sockets (i.e., bind())
//--------------------------------------------------------------------------

#ifndef UDP_SERVER_H
#define UDP_SERVER_H

#ifndef SOCKET_UDP_H
#include "socket_udp.h"
#endif

#ifndef SOCKET_SERVER_H
#include "socket_server.h"
#endif

// *************************************************************************************
//  the UDP server socket class proper
//
//  definition of a UDP server socket:
//    a socket that supports UDP init methods + socket server methods  (i.e., bind())
//
// *************************************************************************************

class udp_server_socket_class : public udp_socket_class, public server_socket_class
{
public:
    // =====================================
    //  constructor
    // =====================================
    //
    //  first form - standard constructor
    //  second form - copy constructor
    //
    udp_server_socket_class(void);
    udp_server_socket_class(const udp_server_socket_class&);

    // ======================================
    //  destructor
    // ======================================
    //
    virtual ~udp_server_socket_class(void);
};

#endif
