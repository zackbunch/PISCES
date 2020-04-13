//--------------tcp_fixed_message_length_selectable_rollover.cpp------------------------------------------------
//
//   Author:   Phil Pfeiffer
//   Date:     August 1999
//   Last modified:  April 2020
//
//   Simplify management of a TCP communication by implementing a selectable TCP rollover
//   endpoint that communicates via fixed length messages
//
//   Design notes:
//   -.  clients must support same buffer size as the size that's hard-coded into this program
//       -.  suggested improvement: make buffer size a command-line parameter
//   -.  The pointer to clientConnectionCount is passed as auxiliary parameter to the selectable
//       TCP classes due to issues with interaction between STL::map and MS's runtime
//       -.  when a static_cast< void * > of &clientConnectionCount is stored as
//             socketMessageBlock[ tcp_fixed_message_length_selectable_server_class::P_FEEDBACK_STREAM ]
//           downcasting that reference to unsigned long consistently returns a corrupted reference
//       -.  when a static_cast< void * > of a function that returns either
//             -.  *clientConnectionCount
//             -.  or a reference to clientConnectionCount
//           is stored as
//             socketMessageBlock[ tcp_fixed_message_length_selectable_server_class::P_FEEDBACK_STREAM ]
//           attempting to downcast the void * value to a function pointer, then execute that function,
//           consistently generates an access violation - even though socketMessageBlock holds the
//           correct address for that function.
//      Why this is happening is beyond me.  One plausible explanation can be found at
//          https://stackoverflow.com/questions/37221529/why-do-i-get-access-violation-when-trying-to-use-a-map
//      The author of the lone reply, kmdreko,
//      -.  states that the construct is creating an infinite recursion,
//      -.  adding that
//          "All in all, std::map is a bad internal structure to use for this. (Actually most of the
//           standard containers would cause similar problems)."
//   -. other problems involving a read lock access conflict involving feedback_stream were fixed by
//      eliminated by passing feedback_stream as a constructor parameter, rather than via socketMessageBlock.
//------------------------------------------------------------------------------------------------------------------

#include "tcp_fixed_message_length_selectable_rollover.h"     // tcp server socket class

// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=
// communications handler class definitions
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

#ifndef BOOL_H
#include "bool.h"
#endif

#ifndef TCP_FIXED_MESSAGE_LENGTH_SELECTABLE_SERVER_H
#include "tcp_fixed_message_length_selectable_server.h"
#endif

#ifndef MULTIPLE_SOCKET_SELECT_H
#include "multiple_socket_select.h"   // multiple socket select
#endif

// *===*===*===*===*===*===*===*===*===*===*===*===*===*===*===*===*===*===
// selectable rollover socket class
// *===*===*===*===*===*===*===*===*===*===*===*===*===*===*===*===*===*===

// =====================================
//  constructor
// =====================================

//-------------------------------------------------------------------------
// constructor - initialize send-receive processing
//-------------------------------------------------------------------------
//
tcp_fixed_message_length_selectable_rollover_class::tcp_fixed_message_length_selectable_rollover_class(
    ostream& feedback_stream, const unsigned& buffer_size, unsigned& connected_client_count)
    : tcp_fixed_message_length_rollover_class(buffer_size),
    selectable_socket_class(),
    feedback_stream_(feedback_stream),
    connected_client_count_(connected_client_count)
{
}

// =====================================
//  manipulators
// =====================================

//----------------------------------------------------------------------------------------------
//  read_event() - receive message from client, reverse it, and return the reversed string
//----------------------------------------------------------------------------------------------
//
void tcp_fixed_message_length_selectable_rollover_class::read_event(argument_block_type& arg_block)
{
    try
    {
        //  *** get the client's message ***
        //  this parameter can be ignored - it's only provided for intermediate message capture
        indication(message_);

        //  then respond; the parent class keeps a buffered copy of the received message
        response();
    }

    catch (...)    // === if indication() failed, end this connection ===
    {
        terminate_connection(arg_block);
    }
}

//---------------------------------------------------------------------------------------------------------------
//  write_event() - should not happen; respond by throwing an exception that should not be caught
//---------------------------------------------------------------------------------------------------------------
//
void tcp_fixed_message_length_selectable_rollover_class::write_event(argument_block_type& arg_block)
{
    static const string my_name = "tcp_fixed_message_length_selectable_server_class::write_event";
    static const string write_ok_detected = "unexpected notification of write channel availability";
    static const string action = "ending program";

    throw socket_exception(my_name, write_ok_detected, action);
}

//-------------------------------------------------------------------------------------------------------------------------
//  exception_event() - process select() exception event:  i.e., server socket problem
//  reduce socket count, throw exception, shut down socket
//-------------------------------------------------------------------------------------------------------------------------
//
void tcp_fixed_message_length_selectable_rollover_class::exception_event(argument_block_type& arg_block)
{
    terminate_connection(arg_block);
}

//-------------------------------------------------------------------------------------------------------------------------
//  terminate_connection() - close the connection, notifying client and removing socket from the select() pool
//-------------------------------------------------------------------------------------------------------------------------
//
void tcp_fixed_message_length_selectable_rollover_class::terminate_connection(argument_block_type& arg_block)
{
    // ***  access parameters to manipulate for connection termination  ***
    //
    multiple_socket_select_class& monitorObject =
        *static_cast<multiple_socket_select_class*>(arg_block[tcp_fixed_message_length_selectable_server_class::P_SOCKET_SELECT_OBJECT]);

    try       // rethrow last exception to get correct error-printing logic
    {
        throw;
    }
    catch (const basic_exception& message)
    {
        feedback_stream_ << message << endl;
    }
    catch (...)
    {
    }

    // *** drop current socket from scan set ***
    //
    monitorObject.update_scan(*this, socket_event_flags_class::no_event_flags_);
    connected_client_count_--;
    feedback_stream_ << "dropping connection from monitored sockets list.  active connection count now " << connected_client_count_ << endl;

    shutdown();
    closesocket();
}

// ======================================
//  destructor
// ======================================
//
tcp_fixed_message_length_selectable_rollover_class::~tcp_fixed_message_length_selectable_rollover_class(void)
{
    shutdown();
    closesocket();
}