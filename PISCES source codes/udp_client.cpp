//--------------udp_client.cpp--------------------------------------------------
//
//   Author:   Phil Pfeiffer
//   Date:     August 1999
//   Last modified:  April 2020
//
//   Simplify management of a UDP communication
//   by implementing a UDP client endpoint as an object
//
//   A UDP client socket combines
//     the functionality of a UDP socket (i.e., the initialization methods)
//     with the functionality of client sockets (i.e., connect())
//-------------------------------------------------------------------------------

#include "udp_client.h"

// ======================================
//  constructor
// ======================================

// ---------------------------------------------------------------
//  constructor, first form - standard constructor
// ---------------------------------------------------------------
//
udp_client_socket_class::udp_client_socket_class(void)
    : udp_socket_class(), client_socket_class()
{
}

// ---------------------------------------------------------------
//  constructor, second form - copy constructor
// ---------------------------------------------------------------
//
udp_client_socket_class::udp_client_socket_class(const udp_client_socket_class& source)
    : udp_socket_class(source), client_socket_class(source)
{
}

// ======================================
//  destructor
// ======================================
//
udp_client_socket_class::~udp_client_socket_class(void)
{
}