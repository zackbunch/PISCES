//--------------socket_tcp.cpp----------------------------------------------------
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
//
//-----------------------------------------------------------------------------

#include "socket_tcp.h"

#include <sstream>   // stringstream

#ifndef SOCKET_ERROR_MESSAGE_H
#include "socket_error_message.h"
#endif

#ifndef SOCKET_APIS_H
#include "socket_apis.h"         // ip version abstraction code
#endif

// ======================================
//  constructor
// ======================================

//------------------------------------------------------------
//  constructor, first form- create socket object that supports TCP connections
//------------------------------------------------------------
//
tcp_socket_class::tcp_socket_class(void) : socket_class()
{
    // **** set up static data for error messages ****

    static const string my_name = "tcp_socket_class::init";
    static const string no_init = "can't init specified socket for TCP";

    // **** attempt the setup ****

    //  the following call initializes the socket as a TCP/IP family
    //  streaming socket (i.e., as a TCP socket)
    //
    socket_ = ::socket(get_ip_family(), SOCK_STREAM, IPPROTO_TCP);

    if (socket_ == INVALID_SOCKET)
        throw socket_tcp_init_exception(my_name, no_init, socket_error_message_class("socket").get_message());
}

//--------------------------------------------------------------------------
//  constructor, second form - copy constructor
//--------------------------------------------------------------------------
//
tcp_socket_class::tcp_socket_class(const tcp_socket_class& source)
    : socket_class(source)
{
}

// ============================================
//  manipulators
// ============================================

//------------------------------------------------------------
//  shutdown-- issue TCP half-close  (important for active connections)
//------------------------------------------------------------
//
void tcp_socket_class::shutdown(void)
{
    // **** set up static data for error messages ****

    static const string my_name = "tcp_socket_class::shutdown";
    static const string no_shutdown = "couldn't shutdown socket";

    // **** attempt the shutdown ****

    // second argument to sockets shutdown primitive is a mode argument
    // that can assume one of three values:
    //
    //     mode of 0 => further receives are disallowed.
    //     mode of 1 => further sends are disallowed.
    //     mode of 2 => further sends and receives are disallowed.
    //
    // Quinn and Shute  (_Windows Socket Network Programming_, Addison-Wesley)
    // strongly recommend against mode 0, and recommend against mode 2.

    if (::shutdown(socket_, 1) == SOCKET_ERROR)
    {
        // ignore shutdown failure if socket not connected or already closed
        //
        const long error_code = get_last_socket_error();
        if ((error_code != WSAENOTCONN) && (error_code != socket_already_closed()))
        {
            throw socket_shutdown_exception(my_name, no_shutdown, socket_error_message_class("shutdown").get_message());
        }
    }
}

// ======================================
//  destructor
// ======================================
//
tcp_socket_class::~tcp_socket_class(void)
{
}

// *******************************************************************
//  exception classes for tcp_socket_class  [boilerplate]
// *******************************************************************

socket_tcp_init_exception::socket_tcp_init_exception(
    const string& name, const string& short_message, const string& long_message,
    const basic_exception& prior_exception)
    : socket_exception(name, short_message, long_message, prior_exception) {}

socket_tcp_init_exception::socket_tcp_init_exception(const socket_tcp_init_exception& source)
    : socket_exception(source) {}

socket_tcp_init_exception::~socket_tcp_init_exception(void) {}

socket_shutdown_exception::socket_shutdown_exception(
    const string& name, const string& short_message, const string& long_message,
    const basic_exception& prior_exception)
    : socket_exception(name, short_message, long_message, prior_exception) {}

socket_shutdown_exception::socket_shutdown_exception(const socket_shutdown_exception& source)
    : socket_exception(source) {}

socket_shutdown_exception::~socket_shutdown_exception(void) {}

// *===*====*===*====*===*====*===*====*===*====*===*====*===*====*===*====
// auxiliary operators  [boilerplate]
// *===*====*===*====*===*====*===*====*===*====*===*====*===*====*===*====

ostream& operator<<(ostream& os, const socket_tcp_init_exception& ex)
{
    ex.insert(os);
    return os;
}

ostream& operator<<(ostream& os, const socket_shutdown_exception& ex)
{
    ex.insert(os);
    return os;
}