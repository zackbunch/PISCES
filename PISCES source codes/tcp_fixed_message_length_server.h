//--------------tcp_fixed_message_length_server.h------------------------------------------
//
//  Author:  Phil Pfeiffer
//  Date:    Fall 1999
//  Last modified:  April 2020
//
//   Simplify management of a TCP communication by implementing a TCP server
//   endpoint that communicates via variable length messages
//
//   Design notes:
//     This is a dummy class, included for consistency with the PISCES architecture;
//     all message-related logic is implemented in the corresponding rollover class.
//----------------------------------------------------------------------------------------

#ifndef TCP_FIXED_MESSAGE_LENGTH_SERVER_H
#define TCP_FIXED_MESSAGE_LENGTH_SERVER_H

#ifndef TCP_SERVER_H
#include "tcp_server.h"
#endif

class tcp_fixed_message_length_server_class :virtual public tcp_server_socket_class
{
public:
    // =====================================
    //  constructor
    // =====================================
    //
    //  first form - standard constructor
    //  second form - copy constructor
    //
    tcp_fixed_message_length_server_class(void);
    tcp_fixed_message_length_server_class(const tcp_fixed_message_length_server_class&);

    // ======================================
    //  destructor
    // ======================================
    //
    virtual ~tcp_fixed_message_length_server_class(void);
};

#endif