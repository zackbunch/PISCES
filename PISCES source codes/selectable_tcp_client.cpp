//--------------selectable_tcp_client.cpp-----------------------------------------
//
//   Author:    Phil Pfeiffer
//   Date:      September 1999
//   Last Modified:  April 2020
//
//   Simplify management of a TCP connection by implementing
//   a selectable, client-side endpoint for ongoing communication as an object
//
//   Design notes:
//      This module combines the functionality
//      of a TCP client socket and a selectable socket.
//----------------------------------------------------------------------------------

#include "selectable_tcp_client.h"

// ======================================
//  constructor
// ======================================

// ---------------------------------------------------------------
//  constructor, first form - standard constructor
// ---------------------------------------------------------------
//
selectable_tcp_client_socket_class::selectable_tcp_client_socket_class(void)
    : tcp_client_socket_class(), selectable_socket_class()
{
}

// ---------------------------------------------------------------
//  constructor, second form - copy constructor
// ---------------------------------------------------------------
//
selectable_tcp_client_socket_class::selectable_tcp_client_socket_class(const selectable_tcp_client_socket_class& source)
    : tcp_client_socket_class(source), selectable_socket_class(source)
{
}

// ======================================
//  destructor
// ======================================
//
selectable_tcp_client_socket_class::~selectable_tcp_client_socket_class(void)
{
}