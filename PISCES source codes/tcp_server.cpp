//--------------tcp_server.cpp----------------------------------------------------------
//
//   Author:   Phil Pfeiffer
//   Date:     August 1999
//   Last modified:  April 2020
//
//   Simplify management of a TCP communication
//   by implementing a TCP server endpoint as an object
//
//   A TCP server socket combines
//     the functionality of a TCP socket (i.e., the initialization methods)
//     with the functionality of server sockets (i.e., bind)
//     and TCP server functions (i.e., listen(), accept())
//-------------------------------------------------------------------------------------

#include "tcp_server.h"

#ifndef SOCKET_ERROR_MESSAGE_H
#include "socket_error_message.h"
#endif

#include <sstream>   // stringstream

#ifndef TCP_ROLLOVER_H
#include "tcp_rollover.h"         // rollover socket class
#endif

// ======================================
//  constructor
// ======================================

// ---------------------------------------------------------------
//  constructor, first form - standard constructor
// ---------------------------------------------------------------
//
tcp_server_socket_class::tcp_server_socket_class(void)
    : tcp_socket_class(), server_socket_class()
{
}

// ---------------------------------------------------------------
//  constructor, second form - copy constructor
// ---------------------------------------------------------------
//
tcp_server_socket_class::tcp_server_socket_class(const tcp_server_socket_class& source)
    : tcp_socket_class(source), server_socket_class(source)
{
}

// =======================================================================
//  manipulators
// =======================================================================

//----------------------------------------------------------------------
//  listen- sets number of pending connections to queue at one time.
//     stream argument: stream for error messages  (default: stderr)
//----------------------------------------------------------------------
//
void tcp_server_socket_class::listen(const unsigned backlog)
{
    // **** set up static data for error messages ****

    static const string my_name = "tcp_server_socket_class::listen";

    // **** attempt to listen, and establish backlog ****

    // use the socket API's version of listen()
    //
    if (::listen(socket_, backlog) == SOCKET_ERROR)
    {
        stringstream p_message;
        p_message << "can't set connection backlog to " << backlog;
        throw socket_listen_exception(my_name, p_message.str(), socket_error_message_class("listen").get_message());
    }
}

//----------------------------------------------------------------------
//  accept- obtains an actual connection with an end user --
//          returning *new* socket for use in end-to-end communications
//     stream argument: stream for error messages  (default: stderr)
//----------------------------------------------------------------------
//
void tcp_server_socket_class::accept(tcp_rollover_socket_class& new_socket)
{
    // **** set up static data for error messages ****

    static const string my_name = "tcp_server_socket_class::accept";
    static const string no_accept = "can't accept connection";

    // **** attempt the accept ****

    // ==== create buffer for accept() operation ====
    //
    socket_address host;

    // ==== use socket API to accept connection ====
    //
    //   note that accept() returns a new socket,
    //     dedicated to communicating with client that just obtained the server's attention.
    //   use of second "handoff" socket allows server to resume listening
    //      for connection requests from other clients on the original socket

    struct sockaddr* const host_reference = reinterpret_cast<struct sockaddr*>(&host);
    sockAddrBufferLenType len = sizeof(host);  // must store length in separate variable

    const SOCKET accept_socket = ::accept(socket_, host_reference, &len);

    if (accept_socket == INVALID_SOCKET)
        throw socket_accept_exception(my_name, no_accept, socket_error_message_class("accept").get_message());

    // ==== update result object  ====
    //
    new_socket = accept_socket;
}

// ======================================
//  destructor
// ======================================
//
tcp_server_socket_class::~tcp_server_socket_class(void)
{
}

// *******************************************************************
//  exception classes for client_socket_class and all derived classes
// *******************************************************************

socket_listen_exception::socket_listen_exception(
    const string& name, const string& short_message, const string& long_message,
    const basic_exception& prior_exception)
    : socket_exception(name, short_message, long_message, prior_exception) {}

socket_listen_exception::socket_listen_exception(const socket_listen_exception& source)
    : socket_exception(source) {}

socket_listen_exception::~socket_listen_exception(void) {}

socket_accept_exception::socket_accept_exception(
    const string& name, const string& short_message, const string& long_message,
    const basic_exception& prior_exception)
    : socket_exception(name, short_message, long_message, prior_exception) {}

socket_accept_exception::socket_accept_exception(const socket_accept_exception& source)
    : socket_exception(source) {}

socket_accept_exception::~socket_accept_exception(void) {}

// =*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
//  auxiliary operators for exception classes [boilerplate]
// =*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*

ostream& operator<<(ostream& os, const socket_listen_exception& ex)
{
    ex.insert(os);
    return os;
}

ostream& operator<<(ostream& os, const socket_accept_exception& ex)
{
    ex.insert(os);
    return os;
}