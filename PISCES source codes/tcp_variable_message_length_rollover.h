//--------------tcp_variable_message_length_rollover.h------------------------------------
//
//  Author:  Phil Pfeiffer
//  Date:    Fall 1999
//  Last modified:  April 2020
//
//   Simplify management of a TCP communication by implementing a TCP rollover 
//   endpoint that communicates via variable length messages
//----------------------------------------------------------------------------------------

#ifndef TCP_VARIABLE_LENGTH_MESSAGE_ROLLOVER_H
#define TCP_VARIABLE_LENGTH_MESSAGE_ROLLOVER_H 

#ifndef PLATFORM_H
#include "platform.h"   // platform-specific definitions
#endif

#include <iostream>     // cin, cout, clog
#include <string>

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

class tcp_variable_message_length_rollover_class: public tcp_rollover_socket_class
{
   public:
       // =====================================
       //  constructor
       // =====================================
       //
       // specify number of characters in message buffer payload
       //
       tcp_variable_message_length_rollover_class( const unsigned& );

       // =====================================
       //  manipulators
       // =====================================
       //
       void indication( string& );   // get string from client
       void response( void );        // return response to client (assumes indication present in buffer)

       // ======================================
       //  destructor
       // ======================================
       //
       virtual ~tcp_variable_message_length_rollover_class( void );

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
       const unsigned long  max_message_length_;

       // size of a message header
       const unsigned messageHeaderSize_;

       // ======== recv message buffering ======

       // internal object for header buffering, with size
       fixed_message_length_buffer_class   recv_header_buf_;
       argument_block_type                 recv_header_arg_block_;

       // internal object for holding received string
       string   message_;
};

#endif