//--------------selectable_udp_peer.h--------------------------------------
//
//   Author:    Phil Pfeiffer
//   Date:      September 1999
//   Last Modified:  April 2020
//
//   Simplify management of a UDP connection by implementing
//   a selectable, peer endpoint for ongoing communication as an object
//
//   Design notes:
//      This module combines the functionality
//      of a UDP peer socket and a selectable socket.
//-------------------------------------------------------------------------

#ifndef SELECTABLE_UDP_PEER_H
#define SELECTABLE_UDP_PEER_H

#ifndef UDP_PEER_H
#include "udp_peer.h"
#endif

#ifndef SOCKET_SELECT_H
#include "socket_select.h"
#endif

// *******************************************************************
//  the UDP selectable peer socket class proper
//
//  definition of a UDP selectable peer socket:
//    a socket that supports UDP peer methods + selectability
//
// *******************************************************************

class selectable_udp_peer_socket_class : virtual public udp_peer_socket_class, virtual public selectable_socket_class
{
public:
    // =====================================
    //  constructor
    // =====================================
    //
    //  first form - standard constructor
    //  second form - copy constructor
    //
    selectable_udp_peer_socket_class(void);
    selectable_udp_peer_socket_class(const selectable_udp_peer_socket_class&);

    // ======================================
    //  destructor
    // ======================================
    //
    virtual ~selectable_udp_peer_socket_class(void);
};

#endif
