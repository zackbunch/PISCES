//--------------tcp_fixed_message_length_rollover.h---------------------------------------
//
//  Author:  Phil Pfeiffer
//  Date:    Fall 1999
//  Last modified:  April 2020
//
//   Simplify management of a TCP communication by implementing a TCP rollover
//   endpoint that communicates via variable length messages
//----------------------------------------------------------------------------------------

#ifndef TCP_FIXED_MESSAGE_LENGTH_ROLLOVER_H
#define TCP_FIXED_MESSAGE_LENGTH_ROLLOVER_H

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

#ifndef TCP_ROLLOVER_H
#include "tcp_rollover.h"     // tcp handoff socket class
#endif

#ifndef MESSAGE_BUFFER_H
#include "message_buffer.h"    // message buffering class
#endif

// *===*===*===*===*===*===*===*===*===*===*===*===*===*===*===*===*===*===
// communications handler class declaration
// *===*===*===*===*===*===*===*===*===*===*===*===*===*===*===*===*===*===

class tcp_fixed_message_length_rollover_class : public tcp_rollover_socket_class
{
public:
    // =====================================
    //  constructor
    // =====================================
    //
    // specify number of characters in message buffer payload
    //
    tcp_fixed_message_length_rollover_class(const unsigned&);

    // =====================================
    //  manipulators
    // =====================================
    //
    void indication(string&);   // get string from client
    void response(void);        // return response to client (assumes indication present in buffer)

    // ======================================
    //  destructor
    // ======================================
    //
    virtual ~tcp_fixed_message_length_rollover_class(void);

protected:
    // ======== recv message buffering ======

    // internal objects for message buffering, processing
    //
    fixed_message_length_buffer_class   outgoing_message_buffer_;
    argument_block_type                 outgoing_message_arg_block_;
    fixed_message_length_buffer_class   incoming_message_buffer_;
    argument_block_type                 incoming_message_arg_block_;

private:
    // ======================================
    //  private member data
    // ======================================

    // ======== send and receive message buffering ======

    // client-specified specified message length
    //
    const unsigned long  message_length_;

    // internal object for holding received string
    //
    string   message_;
};

#endif