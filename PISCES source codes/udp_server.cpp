//--------------udp_server.cpp------------------------------------------------
//
//   Author:   Phil Pfeiffer
//   Date:     August 1999
//   Last modified:  April 2020
//
//   Simplify management of a UDP communication
//   by implementing a UDP server endpoint as an object
//
//   A UDP server socket combines
//     the functionality of a UDP socket (i.e., the initialization methods)
//     with the functionality of server sockets (i.e., bind())
//-----------------------------------------------------------------------------

#include "udp_server.h"

// ======================================
//  constructor
// ======================================

// ---------------------------------------------------------------
//  constructor, first form - standard constructor
// ---------------------------------------------------------------
//
udp_server_socket_class::udp_server_socket_class(void)
    : udp_socket_class(), server_socket_class()
{
}

// ---------------------------------------------------------------
//  constructor, second form - copy constructor
// ---------------------------------------------------------------
//
udp_server_socket_class::udp_server_socket_class(const udp_server_socket_class& source)
    : udp_socket_class(source), server_socket_class(source)
{
}

// ======================================
//  destructor
// ======================================
//
udp_server_socket_class::~udp_server_socket_class(void)
{
}