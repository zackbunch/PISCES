//--------------tcp_variable_message_length_client.h----------------------------------
//
//  Author:  Phil Pfeiffer
//  Date:    Fall 1999
//  Last modified:  April 2020
//
//   Simplify management of a TCP communication by implementing a TCP client
//   endpoint that communicates via variable length messages
//------------------------------------------------------------------------------------

#ifndef TCP_VARIABLE_LENGTH_MESSAGE_CLIENT_H
#define TCP_VARIABLE_LENGTH_MESSAGE_CLIENT_H

#include <iostream>     // cin, cout, clog
#include <string>

#ifndef PLATFORM_H
#include "platform.h"   // platform-specific definitions
#endif

#ifndef STD_H
#include "std.h"
#endif

#ifndef HOST_DATA_H
#include "host_data.h"    // TCP/IP host data class
#endif

#ifndef TCP_CLIENT_H
#include "tcp_client.h"
#endif

#ifndef MESSAGE_BUFFER_H
#include "message_buffer.h"    // message buffering class
#endif

// *===*===*===*===*===*===*===*===*===*===*===*===*===*===*===*===*===*===
// communications handler class declaration
// *===*===*===*===*===*===*===*===*===*===*===*===*===*===*===*===*===*===
//
// ==== class below used to exchange messages with client ====

class tcp_variable_message_length_client_class : public tcp_client_socket_class
{
public:
    // =====================================
    //  constructor
    // =====================================
    //
    //  specify number of characters in message buffer payload
    //
    tcp_variable_message_length_client_class(void);

    // =====================================
    //  manipulators
    // =====================================
    //
    void request(const string&);    // send string to server
    void confirm(string&);          // recover response from server

    // ======================================
    //  destructor
    // ======================================
    //
    virtual ~tcp_variable_message_length_client_class(void);

private:
    // ======================================
    //  private message format parameters
    // ======================================
    //
    //  use 2-byte-long variables for storing count of chars in payload

    typedef unsigned16 message_length_type;

    // ======================================
    //  private member data
    // ======================================

    // ======== send and receive message buffering ======

    // maximum possible message length (computed from message_length_type)
    //
    const unsigned long  max_message_length_;

    // size of a message header
    //
    const unsigned message_header_size_;

    // ======== recv message buffering ======

    // internal object for header buffering, with size
    //
    fixed_message_length_buffer_class   recv_header_buf_;
    argument_block_type                 recv_header_arg_block_;

    // ======================================
    //  private methods
    // ======================================
    //
    // check if message to be sent is too large--throw exception if so

    void validate_outgoing_message_length(const string&);
};

#endif