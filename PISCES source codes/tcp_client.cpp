//--------------tcp_client.cpp----------------------------------------------------
//
//   Author:   Phil Pfeiffer
//   Date:     August 1999
//   Last modified:  April 2020
//
//   Simplify management of a TCP communication
//   by implementing a TCP client endpoint as an object
//
//   A TCP client socket combines
//     the functionality of a TCP socket (i.e., the initialization methods)
//     with the functionality of client sockets (i.e., connect())
//--------------------------------------------------------------------------------

#include "tcp_client.h"

// ======================================
//  constructor
// ======================================

// ---------------------------------------------------------------
//  constructor, first form - standard constructor
// ---------------------------------------------------------------
//
tcp_client_socket_class::tcp_client_socket_class(void)
    : tcp_socket_class(), client_socket_class()
{
}

// ---------------------------------------------------------------
//  constructor, second form - copy constructor
// ---------------------------------------------------------------
//
tcp_client_socket_class::tcp_client_socket_class(const tcp_client_socket_class& source)
    : tcp_socket_class(source), client_socket_class(source)
{
}

// ======================================
//  destructor
// ======================================
//
tcp_client_socket_class::~tcp_client_socket_class(void)
{
}