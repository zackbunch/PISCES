//--------------socket_server.h--------------------------------------------------------------------
//
//   Author:   Phil Pfeiffer
//   Date:     August 1999
//   Last modified:  April 2020
//
//   Simplify task of managing a communication
//   by implementing server functionality as an object.
//
//   This module defines the primitives used by server processes-- namely,
//   -.  bind(), which links a port to a socket,
//   -.  reusability primitives, which specify whether multiple ports can bind to a given socket.
//-------------------------------------------------------------------------------------------------

#ifndef SOCKET_SERVER_H
#define SOCKET_SERVER_H

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

// *****************************************************************************
//  the server socket class proper
//
//  definition of a server socket:  a socket that supports
//
//  -.   a constructor that initializes server socket processing:
//            here, that simply calls the base class's constructor method
//
//  -.   a bind() method,
//       which associates a socket with a caller-specified port number,
//       and throws an exception if the attempt to bind fails
//
//  -.   methods that control whether a socket may be shared among
//       multiple processes:
//        is_reusable() -     return "TRUE" if multiple procs. can bind socket
//        set_reusable() -    make socket shareable among multiple processes
//        set_exclusive() -   make socket exclusive to one process (default)
//
//  note: a socket's shareability may not be changed, once a socket is bound
// *****************************************************************************

class server_socket_class : public virtual socket_class
{
public:

    // ======================================================
    //  inspectors
    // ======================================================

    //   discover if socket (i.e., protocol/IP addr/port no)
    //         may be bound by multiple applications
    //
    int is_reusable(void);

    // ======================================================================
    //  manipulators
    // ======================================================================

    //   primitive that binds a socket to a user-specified port number
    //
    void bind(const socket_service_access_point_class&);

    //   operating mode manipulators
    //       specify if socket (i.e., protocol/IP addr/port no)
    //            may be bound by multiple applications
    //       argument:  stream for receiving error messages
    //
    void set_reusable(void);
    void set_exclusive(void);

    // ======================================
    //  destructor
    // ======================================
    //
    virtual ~server_socket_class(void);

protected:
    // ====================================================================
    //  constructor
    // ====================================================================
    //
    //  protect constructor to ensure that class functions
    //     as an abstract class:
    //  instantiation of underlying socket depends on protocol in use,
    //     which must be specified by a second class derived
    //     from the base socket_class

    //  first form - standard constructor
    //  second form - copy constructor
    //
    server_socket_class(void);
    server_socket_class(const server_socket_class&);
};

// *******************************************************************
//  exception classes for server_socket_class
// *******************************************************************
//
// base class, socket_exception, defined in socket.h.

// derived (specific) exceptions:
//
//    socket_bind_exception -  couldn't bind to specified socket
//         (usually because port was already bound)

class socket_bind_exception : public socket_exception
{
public:
    socket_bind_exception(
        const string & = "", const string & = "", const string & = "",
        const basic_exception & = basic_exception());
    socket_bind_exception(const socket_bind_exception& source);
    virtual ~socket_bind_exception(void);
};

// *===*====*===*====*===*====*===*====*===*====*===*====*===*====*===*====
// auxiliary operators
// *===*====*===*====*===*====*===*====*===*====*===*====*===*====*===*====

ostream& operator<<(ostream&, const  socket_bind_exception&);

#endif
