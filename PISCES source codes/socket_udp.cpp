//--------------socket_udp.cpp----------------------------------------------------
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
//--------------------------------------------------------------------------------

#include "socket_udp.h"

#ifndef SOCKET_ERROR_MESSAGE_H
#include "socket_error_message.h"
#endif

#ifndef SOCKET_APIS_H
#include "socket_apis.h"         // ip version abstraction code
#endif

// ======================================
//  constructor
// ======================================

//--------------------------------------------------------------------------
//  constructor, first form - create socket object that supports UDP traffic
//--------------------------------------------------------------------------
//
udp_socket_class::udp_socket_class(void) : socket_class()
{
    // **** set up static data for error messages ****

    static const string my_name = "udp_socket_class::init";
    static const string no_init = "can't init specified socket for UDP";

    // **** attempt the setup ****

    //  the following call initializes the socket as a TCP/IP family
    //  streaming socket (i.e., as a TCP socket)
    //
    socket_ = ::socket(get_ip_family(), SOCK_DGRAM, IPPROTO_UDP);

    if (socket_ == INVALID_SOCKET)
        throw socket_udp_init_exception(my_name, no_init, socket_error_message_class("socket").get_message());
}

//--------------------------------------------------------------------------
//  constructor, second form - copy constructor
//--------------------------------------------------------------------------
//
udp_socket_class::udp_socket_class(const udp_socket_class& source)
    : socket_class(source)
{
}

// ======================================
//  destructor
// ======================================
//
udp_socket_class::~udp_socket_class(void)
{
}

// *******************************************************************
//  exception classes for tcp_socket_class  [boilerplate]
// *******************************************************************

socket_udp_init_exception::socket_udp_init_exception(
    const string& name, const string& short_message, const string& long_message,
    const basic_exception& prior_exception)
    : socket_exception(name, short_message, long_message, prior_exception) {}

socket_udp_init_exception::socket_udp_init_exception(const socket_udp_init_exception& source)
    : socket_exception(source) {}

socket_udp_init_exception::~socket_udp_init_exception(void) {}

// *===*====*===*====*===*====*===*====*===*====*===*====*===*====*===*====
// auxiliary operators  [boilerplate]
// *===*====*===*====*===*====*===*====*===*====*===*====*===*====*===*====

ostream& operator<<(ostream& os, const socket_udp_init_exception& ex)
{
    ex.insert(os);
    return os;
}