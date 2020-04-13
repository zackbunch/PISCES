//--------------udp_peer.cpp-----------------------------------------------------------
//
//   Author:   Phil Pfeiffer
//   Date:     August 1999
//   Last modified:  April 2020
//
//   Simplify management of a UDP communication
//   by implementing a UDP peer endpoint as an object
//
//   A UDP server socket combines
//     the functionality of a UDP socket (i.e., the initialization methods)
//     with the functionality of client and server sockets i.e., connect(), bind())
//--------------------------------------------------------------------------------------

#include "udp_peer.h"

// ======================================
//  constructor
// ======================================

// ---------------------------------------------------------------
//  constructor, first form - standard constructor
// ---------------------------------------------------------------
//
udp_peer_socket_class::udp_peer_socket_class(void)
    : udp_socket_class(), client_socket_class(), server_socket_class()
{
}

// ---------------------------------------------------------------
//  constructor, second form - copy constructor
// ---------------------------------------------------------------
//
udp_peer_socket_class::udp_peer_socket_class(const udp_peer_socket_class& source)
    : udp_socket_class(source), client_socket_class(source), server_socket_class(source)
{
}

// ======================================
//  destructor
// ======================================
//
udp_peer_socket_class::~udp_peer_socket_class(void)
{
}