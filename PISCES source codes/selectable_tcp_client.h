//--------------selectable_tcp_client.h-------------------------------------------
//
//   Author:    Phil Pfeiffer
//   Date:      September 1999
//   Last Modified:  April 2020
//
//   Simplify management of a TCP connection by implementing
//   a selectable, client-side endpoint for ongoing communication as an object
//
//   Design notes:
//      This module combines the functionality
//      of a TCP client socket and a selectable socket.
//----------------------------------------------------------------------------------

#ifndef SELECTABLE_TCP_CLIENT_H
#define SELECTABLE_TCP_CLIENT_H

#ifndef TCP_CLIENT_H
#include "tcp_client.h"
#endif

#ifndef SOCKET_SELECT_H
#include "socket_select.h"
#endif

// *******************************************************************
//  the TCP selectable client socket class proper
//
//  definition of a selectable TCP client socket:
//      a socket that supports TCP client methods + selection
// *******************************************************************

class selectable_tcp_client_socket_class : virtual public tcp_client_socket_class, virtual public selectable_socket_class
{
public:
    // =====================================
    //  constructor
    // =====================================
    //
    //  first form - standard constructor
    //  second form - copy constructor
    //
    selectable_tcp_client_socket_class(void);
    selectable_tcp_client_socket_class(const selectable_tcp_client_socket_class&);

    // ======================================
    //  destructor
    // ======================================
    //
    virtual ~selectable_tcp_client_socket_class(void);
};

#endif
