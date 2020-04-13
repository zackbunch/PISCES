//--------------tcp_fixed_message_length_selectable_server.cpp--------------------------------------------
//
//   Author:   Phil Pfeiffer
//   Date:     August 1999
//   Last modified:  April 2020
//
//   Simplify management of a TCP communication by implementing a TCP server
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
//-------------------------------------------------------------------------------------------------------------

#include "tcp_fixed_message_length_selectable_server.h"     // tcp server socket class

#ifndef TCP_FIXED_MESSAGE_LENGTH_SELECTABLE_ROLLOVER_H
#include "tcp_fixed_message_length_selectable_rollover.h"
#endif

#ifndef MULTIPLE_SOCKET_SELECT_H
#include "multiple_socket_select.h"   // multiple socket select
#endif

// *===*===*===*===*===*===*===*===*===*===*===*===*===*===*===*===*===*===
// argument block type name declarations
// *===*===*===*===*===*===*===*===*===*===*===*===*===*===*===*===*===*===

//  +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=
//  static data items
//  +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=

//  strings below define keys for argument blocks.
//  part of protocol for class send/sendto/recv/recvfrom routines
//
//  P_FEEDBACK_STREAM - for allowing methods to send feedback to the user interface
//  P_SOCKET_SELECT_OBJECT - for supporting checking of sockets, using select()
//
const string tcp_fixed_message_length_selectable_server_class::P_FEEDBACK_STREAM;
const string tcp_fixed_message_length_selectable_server_class::P_SOCKET_SELECT_OBJECT;

// *===*===*===*===*===*===*===*===*===*===*===*===*===*===*===*===*===*===
// server socket class
// *===*===*===*===*===*===*===*===*===*===*===*===*===*===*===*===*===*===

// =====================================
//  constructor
// =====================================

//-------------------------------------------------------------------------
// constructor
//-------------------------------------------------------------------------
//
tcp_fixed_message_length_selectable_server_class::tcp_fixed_message_length_selectable_server_class(
    ostream& feedback_stream, const unsigned& message_size, unsigned& connected_client_count
)
    : tcp_fixed_message_length_server_class(),
    selectable_socket_class(),
    feedback_stream_(feedback_stream),
    message_size_(message_size),
    connected_client_count_(connected_client_count)
{
}

// =====================================
//  manipulators
// =====================================

//-------------------------------------------------------------------------------------------------------------------------
//  read_event() - process select() read event:  i.e., incoming connection request
//
//  preconditions:
//  -.  arg_block references the following supporting objects for n-way echo service management:
//      -. arg_block[ this->P_SOCKET_SELECT_OBJECT ] references an object for monitoring active connections
//
//  effects:
//  -.   read() accepts connection.
//  -.   read() then announces connection on arg_block-specified stream.
//  -.   read() then adds socket to set of sockets to monitor, incrementing count of accepted sockets.
//-------------------------------------------------------------------------------------------------------------------------
//
void tcp_fixed_message_length_selectable_server_class::read_event(argument_block_type& arg_block)
{
    // *** accept connection ***
    //
    // object used to accept connection must persist beyond method return

    tcp_fixed_message_length_selectable_rollover_class* pNewConnection =
        new tcp_fixed_message_length_selectable_rollover_class(feedback_stream_, message_size_, connected_client_count_);
    tcp_fixed_message_length_selectable_rollover_class& newConnection = *pNewConnection;
    tcp_server_socket_class::accept(newConnection);

    feedback_stream_ << "new client connection to " << newConnection.peer_service_access_point() << endl;

    // *** update socket monitoring variables ***

    // === add this socket to set of sockets being monitored ===
    // === increment count of sockets being monitored ===

    multiple_socket_select_class& monitorObject = *static_cast<multiple_socket_select_class*>(arg_block[this->P_SOCKET_SELECT_OBJECT]);

    monitorObject.update_scan(newConnection);
    connected_client_count_++;
}

//-------------------------------------------------------------------------------------------------------------------------
//  exception_event() - process select() exception event:  i.e., server socket problem
//
//  throw exception, shut down
//-------------------------------------------------------------------------------------------------------------------------
//
void tcp_fixed_message_length_selectable_server_class::exception_event(argument_block_type&)
{
    static const string my_name = "tcp_fixed_message_length_selectable_server_class::exception_event";
    static const string exception_detected = "exception detected on server socket";
    static const string action = "ending program";

    throw socket_exception(my_name, exception_detected, action);
}

// ======================================
//  destructor
// ======================================
//
tcp_fixed_message_length_selectable_server_class::~tcp_fixed_message_length_selectable_server_class(void)
{
    shutdown();      // gracefully terminate any pending connections
    closesocket();
}