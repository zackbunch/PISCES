//--------------tcp_rollover.cpp----------------------------------------------------------
//
//   Author:   Phil Pfeiffer
//   Date:     August 1999
//   Last modified:  April 2020
//
//   Simplify management of a TCP communication by implementing
//   server-side endpoint for ongoing communication as an object
//
//   A TCP rollover socket captures the secondary connection returned
//   by a TCP accept() primitive. This connection transfer provides concurrency:
//   using two sockets allows the incoming call to be fielded on the rollover socket
//   and the original, service socket to be freed to receive more requests.
//---------------------------------------------------------------------------------------

#include "tcp_rollover.h"

// ======================================
//  constructor
// ======================================

// ---------------------------------------------------------------
//  constructor, first form - standard constructor
// ---------------------------------------------------------------
//
tcp_rollover_socket_class::tcp_rollover_socket_class(void)
    : tcp_socket_class()
{
}

// ---------------------------------------------------------------
//  constructor, second form - copy constructor
// ---------------------------------------------------------------
//
tcp_rollover_socket_class::tcp_rollover_socket_class(const tcp_rollover_socket_class& source)
    : tcp_socket_class(source)
{
}

// ========================================================================
//  assignment
// ========================================================================

//------------------------------------------------------------------------
//  standard object copy
//------------------------------------------------------------------------
//
void tcp_rollover_socket_class::operator=(const tcp_rollover_socket_class& source)
{
    socket_ = source.socket_;
}

//------------------------------------------------------------------------
//  assignment operator that updates socket_ variable contained in class
//------------------------------------------------------------------------
//
void tcp_rollover_socket_class::operator=(const SOCKET& socket)
{
    socket_ = socket;
}

// ======================================
//  destructor
// ======================================
//
tcp_rollover_socket_class::~tcp_rollover_socket_class(void)
{
}