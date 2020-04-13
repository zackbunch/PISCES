//--------------socket_client.h------------------------------------------
//
//   Author:   Phil Pfeiffer
//   Date:     August 1999
//   Last modified:  April 2020
//
//   Simplify task of managing a communication
//   by implementing client functionality as an object.
//
//   This module defines the primitives used by server processes-- namely,
//   -.  connect(), which connects a client to a server
//------------------------------------------------------------------------

#ifndef SOCKET_CLIENT_H
#define SOCKET_CLIENT_H

#include <string>

#ifndef PLATFORM_H
#include "platform.h"
#endif

#ifndef STD_H
#include "std.h"
#endif

#ifndef SOCKET_H
#include "socket.h"
#endif

class socket_service_access_point_class;

// ****************************************************************************
//  the client socket class proper
//
//  definition of a client socket:  a socket that supports
//
//  -.   a constructor that initializes server socket processing:
//       here, that simply calls the base class's constructor method
//
//  -.   a connect() method,
//       which links a socket with a caller-specified target port
//       for current protocol, throwing exception if attempt to connect fails
//
//  -.   (possibly) a disconnect() method,
//       which breaks a connection (by connecting to INADDR_ANY)
//       warning: the disconnect convention is not supported by all socket
//       implementations
//             (cf. Quinn and Shute, _Windows Sockets Network Programming_)
// ****************************************************************************

class client_socket_class : public virtual socket_class
{
public:
    // ======================================
    //  manipulator
    // ======================================

    //  connect primitive--
    //      link to remote socket at specified IP address and port.
    //
    void connect(const socket_service_access_point_class&);

    //  disconnect primitive--break link with remote socket
    //      (warning: not supported by all implementations !!)
    //
    void disconnect(void);

    // ======================================
    //  destructor
    // ======================================
    //
    virtual ~client_socket_class(void);

protected:
    // ====================================================================
    //  constructor
    // ====================================================================

    //  protect constructor to ensure that class functions
    //     as an abstract class:
    //  instantiation of underlying socket depends on protocol in use,
    //     which must be specified by a second class derived
    //     from the base socket_class

    //  first form - standard constructor
    //  second form - copy constructor
    //
    client_socket_class(void);
    client_socket_class(const client_socket_class&);
};

// *******************************************************************
//  exception classes for client_socket_class
// *******************************************************************
//
// base class, socket_exception, defined in socket.h.

// derived (specific) exceptions:
//
//    socket_connect_exception -  couldn't connect to specified socket
//         (usually because IP address wasn't valid,
//          or port wasn't bound at time of connect)

class socket_connect_exception : public socket_exception
{
public:
    socket_connect_exception(
        const string & = "", const string & = "", const string & = "",
        const basic_exception & = basic_exception());
    socket_connect_exception(const socket_connect_exception& source);
    virtual ~socket_connect_exception(void);
};

// *===*====*===*====*===*====*===*====*===*====*===*====*===*====*===*====
// auxiliary operators
// *===*====*===*====*===*====*===*====*===*====*===*====*===*====*===*====

ostream& operator<<(ostream&, const socket_connect_exception&);

#endif
