//--------------tcp_fixed_message_length_selectable_server.h--------------------------------------------------
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

#ifndef TCP_FIXED_MESSAGE_LENGTH_SELECTABLE_SERVER_H
#define TCP_FIXED_MESSAGE_LENGTH_SELECTABLE_SERVER_H

//#ifndef SELECTABLE_TCP_SERVER_H
//#include "selectable_tcp_server.h"
//#endif

#ifndef SOCKET_SELECT_H
#include "socket_select.h"
#endif

#ifndef TCP_FIXED_MESSAGE_LENGTH_SERVER_H
#include "tcp_fixed_message_length_server.h"
#endif

class tcp_fixed_message_length_selectable_server_class : public tcp_fixed_message_length_server_class, public selectable_socket_class
{
public:
    // =====================================
    //  constructor
    // =====================================
    //
    // first argument: a stream for status message display.

    tcp_fixed_message_length_selectable_server_class(ostream&, const unsigned&, unsigned&);

    // =====================================
    //  manipulators
    // =====================================

    // when select() detects read activity, accept connection, entering socket into monitored socket set
    // when select() detects write activity, default to socket_select action - i.e., throw exception
    // when select() detect exception activity, throw (ideally) program-terminating exception
    //
    void read_event(argument_block_type&);
    void exception_event(argument_block_type&);

    //  strings below define keys for argument blocks.
    //
    //  P_FEEDBACK_STREAM - for allowing methods to send feedback to the user interface
    //  P_SOCKET_SELECT_OBJECT - for supporting checking of sockets, using select()
    //
    static const string P_FEEDBACK_STREAM;
    static const string P_SOCKET_SELECT_OBJECT;

    // ======================================
    //  destructor
    // ======================================
    //
    virtual ~tcp_fixed_message_length_selectable_server_class(void);

private:
    // ======================================
    //  private member data
    // ======================================
    //
    // stream for directing messages to client program
    // buffer size for rollover sockets that class instantiates in response to accept()
    // reference to variable for counting connected clients
    //
    ostream& feedback_stream_;
    const unsigned& message_size_;
    unsigned& connected_client_count_;
};

#endif