//--------------tcp_rollover.h--------------------------------------------------------------
//
//   Author:   Phil Pfeiffer
//   Date:     August 1999
//   Last modified:  April 2020
//
//   Simplify management of a TCP communication by implementing
//   server-side endpoint for ongoing communication as an object
//
//   A TCP rollover socket captures the secondary connection returned
//   by a TCP accept() primitive. This connection transfer provides concurrency:
//   using two sockets allows the incoming call to be fielded on the rollover socket
//   and the original, service socket to be freed to receive more requests.
//-----------------------------------------------------------------------------------------

#ifndef TCP_ROLLOVER_H
#define TCP_ROLLOVER_H

#ifndef SOCKET_TCP_H
#include "socket_tcp.h"
#endif

// *******************************************************************
//  the TCP rollover socket class proper
//
//  definition of a TCP rollover socket:
//    a socket that supports TCP's send and receive methods,
//    and is already connected--
//        hence, cannot be reconnected or bound to any other port.
//
//  Note:  use the class's assignment method to set this object's
//         socket member datum to the socket returned by accept().
// *******************************************************************

class tcp_rollover_socket_class : public tcp_socket_class
{
public:
    // =====================================
    //  constructor
    // =====================================
    //
    //  first form - standard constructor
    //  second form - copy constructor
    //
    tcp_rollover_socket_class(void);
    tcp_rollover_socket_class(const tcp_rollover_socket_class&);

    //  =====================================================
    //  assignment
    //  =====================================================
    //
    //  first form:   standard object copy.
    //  second form:  update internal socket variable.
    //     designed for use in conjunction with call to TCP accept()

    void operator=(const tcp_rollover_socket_class&);
    void operator=(const SOCKET&);

    // ======================================
    //  destructor
    // ======================================
    //
    virtual ~tcp_rollover_socket_class(void);
};

#endif
