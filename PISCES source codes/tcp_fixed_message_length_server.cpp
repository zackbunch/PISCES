//--------------tcp_fixed_message_length_server.cpp--------------------------------------
//
//  Author:  Phil Pfeiffer
//  Date:    Fall 1999
//  Last modified:  April 2020
//
//   Simplify management of a TCP communication by implementing a TCP server
//   endpoint that communicates via fixed length messages
//
//   Design notes:
//     This is a dummy class, included for consistency with the PISCES architecture;
//     all message-related logic is implemented in the corresponding rollover class.
//----------------------------------------------------------------------------------------

#include "tcp_fixed_message_length_server.h"

// ======================================
//  constructor
// ======================================

// ---------------------------------------------------------------
//  constructor, first form - standard constructor
// ---------------------------------------------------------------
//
tcp_fixed_message_length_server_class::tcp_fixed_message_length_server_class(void)
    : tcp_server_socket_class()
{
}

// ---------------------------------------------------------------
//  constructor, second form - copy constructor
// ---------------------------------------------------------------
//
tcp_fixed_message_length_server_class::tcp_fixed_message_length_server_class(const tcp_fixed_message_length_server_class& source)
    : tcp_server_socket_class(source)
{
}

// ======================================
//  destructor
// ======================================
//
tcp_fixed_message_length_server_class::~tcp_fixed_message_length_server_class(void)
{
}