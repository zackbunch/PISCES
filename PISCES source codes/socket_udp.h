//--------------socket_udp.h----------------------------------------------------------------
//
//   Author:   Phil Pfeiffer
//   Date:     September 1999
//   Last modified:  April 2020
//
//   Simplify task of managing a communication
//   by implementing UDP functionality as an object.
//
//   This module captures primitives specific to TCP processes--
//   -.  the socket initialization logic (init())
//   -.  connection-oriented communication primitives (send()/ recv())
//   -.  methods for identifying the process on other end of a connection
//-----------------------------------------------------------------------------------------

#ifndef SOCKET_UDP_H
#define SOCKET_UDP_H

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

// ***************************************************************************
//  the UDP socket class proper
//
//  definition of a UDP socket:  a socket that
//  -. supports a constructor that initializes UDP-based socket processing
// ***************************************************************************

class udp_socket_class : public virtual socket_class
{
public:
    // ======================================
    //  destructor
    // ======================================
    //
    virtual ~udp_socket_class(void);

protected:
    // =====================================
    //  constructor
    // =====================================
    //
    //  protect constructors to ensure that class functions
    //     as an abstract class:
    //  instantiation of underlying socket depends on method of use
    //     (e.g., socket used as client, socket used as server)
    //     which must be specified by a second class derived
    //     from the base socket_class

    //  first form - standard constructor
    //  second form - copy constructor
    //
    udp_socket_class(void);
    udp_socket_class(const udp_socket_class&);
};

// *******************************************************************
//  exception classes for udp_socket_class
// *******************************************************************
//
// base class, socket_exception, defined in socket.h.

// derived (specific) exceptions:
//
//   socket_udp_init_exception -  couldn't init UDP socket

class socket_udp_init_exception : public socket_exception
{
public:
    socket_udp_init_exception(
        const string & = "", const string & = "", const string & = "",
        const basic_exception & = basic_exception());
    socket_udp_init_exception(const socket_udp_init_exception& source);
    virtual ~socket_udp_init_exception(void);
};

// *===*====*===*====*===*====*===*====*===*====*===*====*===*====*===*====
// auxiliary operators
// *===*====*===*====*===*====*===*====*===*====*===*====*===*====*===*====

ostream& operator<<(ostream&, const socket_udp_init_exception&);

#endif
