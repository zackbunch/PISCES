//--------------selectable_udp_server.h---------------------------------------
//
//   Author:    Phil Pfeiffer
//   Date:      September 1999
//   Last Modified:  April 2020
//
//   Simplify management of a UDP connection by implementing
//   a selectable, server-side endpoint for ongoing communication as an object
//
//   Design notes:
//      This module combines the functionality
//      of a UDP server socket and a selectable socket.
//-----------------------------------------------------------------------------

#ifndef SELECTABLE_UDP_SERVER_H
#define SELECTABLE_UDP_SERVER_H

#ifndef UDP_SERVER_H
#include "udp_server.h"
#endif

#ifndef SOCKET_SELECT_H
#include "socket_select.h"
#endif

// *******************************************************************
//  the UDP selectable server socket class proper
//
//  definition of a UDP selectable server socket:
//    a socket that supports UDP server methods + selectability
//
// *******************************************************************

class selectable_udp_server_socket_class : virtual public udp_server_socket_class, virtual public selectable_socket_class
{
public:
    // =====================================
    //  constructor
    // =====================================
    //
    //  first form - standard constructor
    //  second form - copy constructor
    //
    selectable_udp_server_socket_class(void);
    selectable_udp_server_socket_class(const selectable_udp_server_socket_class&);

    // ======================================
    //  destructor
    // ======================================
    //
    virtual ~selectable_udp_server_socket_class(void);
};

#endif
