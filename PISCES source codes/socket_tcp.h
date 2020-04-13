//--------------socket_tcp.h----------------------------------------------
//
//   Author:   Phil Pfeiffer
//   Date:     August 1999
//   Last modified:  April 2020
//
//   Simplify task of managing a communication
//   by implementing TCP functionality as an object.
//
//   This module captures primitives specific to TCP processes--
//   -.  the socket initialization logic (init())
//   -.  connection-oriented communication primitives (send()/ recv())
//   -.  methods for identifying the process on other end of a connection
//-----------------------------------------------------------------------

#ifndef SOCKET_TCP_H
#define SOCKET_TCP_H

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

// **************************************************************************
//  the TCP socket class proper
//
//  definition of a server socket:  a socket that
//
//  -.  supports a constructor that initializes TCP-based socket processing
//  -.  supports connection-based (send()/recv()) message processing
//  -.  supports the shutdown() [TCP half-close] primitive
//
// **************************************************************************

class tcp_socket_class : public virtual socket_class
{
public:
    // ======================================
    //  manipulators
    // ======================================

    //  **** connection shutdown ****
    //
    //   shutdown should be called before close of an active connection.
    //
    void shutdown(void);

    // ======================================
    //  destructor
    // ======================================
    //
    virtual ~tcp_socket_class(void);

protected:
    // =====================================
    //  constructor
    // =====================================
    //
    //  protect constructor to ensure that class functions
    //     as an abstract class:
    //  instantiation of underlying socket depends on method of use
    //     (e.g., socket used as client, socket used as server)
    //     which must be specified by a second class derived
    //     from the base socket_class

    //  first form - standard constructor
    //  second form - copy constructor
    //
    tcp_socket_class(void);
    tcp_socket_class(const tcp_socket_class&);
};

// *******************************************************************
//  exception classes for tcp_socket_class
// *******************************************************************
//
// base class, socket_exception, defined in socket.h.

// derived (specific) exceptions:
//
//   socket_tcp_init_exception -  couldn't init TCP socket
//   socket_shutdown_exception - couldn't shut down TCP socket

class socket_tcp_init_exception : public socket_exception
{
public:
    socket_tcp_init_exception(
        const string & = "", const string & = "", const string & = "",
        const basic_exception & = basic_exception());
    socket_tcp_init_exception(const socket_tcp_init_exception& source);
    virtual ~socket_tcp_init_exception(void);
};

class socket_shutdown_exception : public socket_exception
{
public:
    socket_shutdown_exception(
        const string & = "", const string & = "", const string & = "",
        const basic_exception & = basic_exception());
    socket_shutdown_exception(const socket_shutdown_exception& source);
    virtual ~socket_shutdown_exception(void);
};

// *===*====*===*====*===*====*===*====*===*====*===*====*===*====*===*====
// auxiliary operators
// *===*====*===*====*===*====*===*====*===*====*===*====*===*====*===*====

ostream& operator<<(ostream&, const socket_tcp_init_exception&);
ostream& operator<<(ostream&, const socket_shutdown_exception&);

#endif
