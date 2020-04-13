//--------------selectable_udp_server.cpp-----------------------------------------
//
//   Author:    Phil Pfeiffer
//   Date:      September 1999
//   Last Modified:  April 2020
//
//   Simplify management of a UDP connection by implementing
//   a selectable, server-side endpoint for ongoing communication as an object
//
//   Design notes:
//      This module combines the functionality
//      of a UDP server socket and a selectable socket.
//-------------------------------------------------------------------------------

#include "selectable_udp_server.h"

// ======================================
//  constructor
// ======================================

// ---------------------------------------------------------------
//  constructor, first form - standard constructor
// ---------------------------------------------------------------
//
selectable_udp_server_socket_class::selectable_udp_server_socket_class(void)
    : udp_server_socket_class(), selectable_socket_class()
{
}

// ---------------------------------------------------------------
//  constructor, second form - copy constructor
// ---------------------------------------------------------------
//
selectable_udp_server_socket_class::selectable_udp_server_socket_class(const selectable_udp_server_socket_class& source)
    : udp_server_socket_class(source), selectable_socket_class(source)
{
}

// ======================================
//  destructor
// ======================================
//
selectable_udp_server_socket_class::~selectable_udp_server_socket_class(void)
{
}