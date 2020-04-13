//--------------socket_client.cpp------------------------------------------
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
//--------------------------------------------------------------------------

#include "socket_client.h"

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
//  constructor, first form - create socket object that acts as client
//------------------------------------------------------------
//
client_socket_class::client_socket_class(void) : socket_class()
{
}

//--------------------------------------------------------------------------
//  constructor, second form - copy constructor
//--------------------------------------------------------------------------
//
client_socket_class::client_socket_class(const client_socket_class& source)
    : socket_class(source)
{
}

// ======================================
//  manipulator
// ======================================

// -----------------------------------------------------------
//  connect() -
//      link to remote socket at specified IP address and port.
// -----------------------------------------------------------
//
void client_socket_class::connect(const socket_service_access_point_class& access_point)
{
    // **** set up static data for error messages ****

    static const string my_name = "client_socket_class::connect";
    static const string no_connection = "can't connect to specified host/ port";

    // **** attempt the connect ****

    // ==== get required data from user objects ====

    const string ip_address = access_point.ip_address().ip_address_as_string();
    const port_number_type port_number = access_point.port_object().port();

    // ==== init data for connect() operation ====

    // includes port number, receiver IP address, and--just in case--
    //     zero sin_zero field of host_structure.
    // call to htons() ensures that port number, in buffer, is in standard
    //     (network) byte ordering, as required by socket library spec
    //
    socket_address host;
    populate_socket_address_struct(host, port_number, socket_address_string_to_numerical(ip_address.c_str()));
    struct sockaddr* const host_reference = reinterpret_cast<struct sockaddr*>(&host);

    if (::connect(socket_, host_reference, sizeof(host)) < 0)
    {
        stringstream p_message;
        p_message << no_connection << " (" << ip_address << ", " << port_number << ")";
        throw socket_connect_exception(my_name, p_message.str(), socket_error_message_class("connect").get_message());
    }
}

// -----------------------------------------------------
//  disconnect() -
//      break connection with current port
//
//      warning: the disconnect convention is not supported by all socket
//      implementations
//           (cf. Quinn and Shute, _Windows Sockets Network Programming_)
//
// -----------------------------------------------------
//
void client_socket_class::disconnect(void)
{
    // **** set up static data for error messages ****

    static const string my_name = "client_socket_class::disconnect";
    static const string no_disconnect = "can't disconnect from current connection";

    // **** attempt the disconnect ****

    // disconnect by connecting to INADDR_ANY.  don't need port
    //
    socket_address host;
    populate_socket_address_struct(host, 0, addr_any);
    struct sockaddr* const host_reference = reinterpret_cast<struct sockaddr*>(&host);

    if (::connect(socket_, host_reference, sizeof(host)) < 0)
    {
        throw socket_connect_exception(my_name, no_disconnect, socket_error_message_class("connect").get_message());
    }
}

// ======================================
//  destructor
// ======================================
//
client_socket_class::~client_socket_class(void)
{
}

// *******************************************************************
//  exception classes for client_socket_class [boilerplate]
// *******************************************************************

socket_connect_exception::socket_connect_exception(
    const string& name, const string& short_message, const string& long_message,
    const basic_exception& prior_exception)
    : socket_exception(name, short_message, long_message, prior_exception) {}

socket_connect_exception
::socket_connect_exception(const socket_connect_exception& source)
    : socket_exception(source) {}

socket_connect_exception::~socket_connect_exception(void) {}

// *===*====*===*====*===*====*===*====*===*====*===*====*===*====*===*====
// auxiliary operators  [boilerplate]
// *===*====*===*====*===*====*===*====*===*====*===*====*===*====*===*====

ostream& operator<<(ostream& os, const  socket_connect_exception& ex)
{
    ex.insert(os);
    return os;
}