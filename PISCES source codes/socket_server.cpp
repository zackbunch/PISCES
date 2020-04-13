//--------------socket_server.cpp---------------------------------------------------------------
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
//-----------------------------------------------------------------------------------------------

#include "socket_server.h"

#ifndef BOOL_H
#include "bool.h"
#endif

#include <sstream>   // stringstream

#ifndef SOCKET_ERROR_MESSAGE_H
#include "socket_error_message.h"
#endif

#ifndef SERVICE_ACCESS_POINT_H
#include "service_access_point.h"   // service access point
#endif

// ======================================
//  constructor
// ======================================

//------------------------------------------------------------
//  constructor, first form - create socket object that acts as server object
//------------------------------------------------------------
//
server_socket_class::server_socket_class(void) : socket_class()
{
}

//--------------------------------------------------------------------------
//  constructor, second form - copy constructor
//--------------------------------------------------------------------------
//
server_socket_class::server_socket_class(const server_socket_class& source)
    : socket_class(source)
{
}

// ======================================
//  inspector
// ======================================

//----------------------------------------------------------------------------
//  is_reusable-
//     check if whether socket name (i.e., protocol/IP addr/port no)
//     can be reused--i.e., bound simultaneously by more than one process.
//
//     returns TRUE if socket is reusable, otherwise FALSE
//----------------------------------------------------------------------------
//
int server_socket_class::is_reusable(void)
{
    // **** set up static data for error messages ****

    static const string my_name = "server_socket_class::is_reusable";
    static const string no_check = "reusability check failed";
    static const string no_data = "reusability check returned no data";

    // **** check reusability ****

    char option_val;         // return value from getsockopt() stored here
    int option_len;          // size of data returned by getsockopt() stored here
    try
    {
        get_sock_option(SOL_SOCKET, SO_REUSEADDR, &option_val, &option_len);
    }
    catch (const basic_exception& message)
    {
        throw socket_option_exception(my_name, no_check, "", message);
    }

    //  ==== audit result before returning ====
    //
    if (option_len < 1) throw socket_option_exception(my_name, no_check, no_data);

    //  ==== socket reusable iff OPTVAL is nonzero ====
    //
    return (option_val == 0) ? FALSE : TRUE;
}

// ============================================
//  manipulators
// ============================================

// -----------------------------------------------------
//  bind primitive
//      note that the port number is a 16-bit quantity:
//      this constraint is immposed by TCP
// -----------------------------------------------------
//
void server_socket_class::bind(const socket_service_access_point_class& access_point)
{
    // **** set up static data for error messages ****

    static const string my_name = "server_socket_class::bind";
    static const string no_bind = "can't bind specified port";

    // **** attempt the connect ****

    // ==== get required data from user objects ====

    const port_number_type port_number = access_point.port_object().port();

    // ==== init data for bind() operation ====

    // includes port number, receiver IP address, and--just in case--
    //     zero sin_zero field of host_structure.
    //
    // call to htons() ensures that port number, in buffer, is in standard
    //     (network) byte ordering, as required by socket library spec
    //
    socket_address host;

    populate_socket_address_struct(host, port_number, addr_any);    //CHANGED

    /*host.sin_family = get_ip_family();      // use TCP/IP protocol suite
    host.sin_port = htons( port_number );
    host.sin_addr.s_addr = INADDR_ANY;    // use local host*/

    //  ==== use Socket API to bind port
    //
    struct sockaddr* const host_reference = reinterpret_cast<struct sockaddr*>(&host);
    if (::bind(socket_, host_reference, sizeof(host)) == SOCKET_ERROR)
    {
        stringstream p_message;
        p_message << no_bind << " (" << port_number << ")";

        throw socket_bind_exception(my_name, p_message.str(), socket_error_message_class("bind").get_message());
    }
}

//-------------------------------------------------------------------------
//  set_reusable-
//     specify that socket name (i.e., protocol/IP addr/port no)
//     may be reused--i.e., bound simultaneously by more than one process.
//
//     returns TRUE if socket is reusable, otherwise FALSE
//-------------------------------------------------------------------------
//
void server_socket_class::set_reusable(void)
{
    // **** set up static data for error messages ****

    static const string my_name = "server_socket_class::set_reusable";
    static const string no_set = "attempt to make socket reusable failed";

    // **** set reusability ****

    static char option_val = 1;      // make reusable
    try
    {
        setSockOpt(SOL_SOCKET, SO_REUSEADDR, &option_val, sizeof(option_val));
    }
    catch (const basic_exception& message)
    {
        throw socket_option_exception(my_name, no_set, "", message);
    }
}

//----------------------------------------------------------------
//  set_exclusive-
//     specify that socket name (i.e., protocol/IP addr/port no)
//     may not be reused--i.e., is exclusive to current process
//
//     returns TRUE if socket is reusable, otherwise FALSE
//----------------------------------------------------------------
//
void server_socket_class::set_exclusive(void)
{
    // **** set up static data for error messages ****

    static const string my_name = "server_socket_class::set_exclusive";
    static const string no_set = "attempt to make socket non-reusable failed";

    // **** set reusability ****

    static char option_val = 0;      // make exclusive
    try
    {
        setSockOpt(SOL_SOCKET, SO_REUSEADDR, &option_val, sizeof(option_val));
    }
    catch (const basic_exception& message)
    {
        throw socket_option_exception(my_name, no_set, "", message);
    }
}

// ======================================
//  destructor
// ======================================
//
server_socket_class::~server_socket_class(void)
{
}

// *******************************************************************
//  exception classes for client_socket_class [boilerplate]
// *******************************************************************

socket_bind_exception::socket_bind_exception(
    const string& name, const string& short_message, const string& long_message,
    const basic_exception& prior_exception)
    : socket_exception(name, short_message, long_message, prior_exception) {}

socket_bind_exception::socket_bind_exception(const socket_bind_exception& source)
    : socket_exception(source) {}

socket_bind_exception::~socket_bind_exception(void) {}

// *===*====*===*====*===*====*===*====*===*====*===*====*===*====*===*====
// auxiliary operators  [boilerplate]
// *===*====*===*====*===*====*===*====*===*====*===*====*===*====*===*====

ostream& operator<<(ostream& os, const socket_bind_exception& ex)
{
    ex.insert(os);
    return os;
}