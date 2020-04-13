//--------------tcp_fixed_message_length_selectable_rollover.h-----------------------------------------------------
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

#ifndef TCP_FIXED_MESSAGE_LENGTH_SELECTABLE_ROLLOVER_H
#define TCP_FIXED_MESSAGE_LENGTH_SELECTABLE_ROLLOVER_H

#ifndef SOCKET_SELECT_H
#include "socket_select.h"
#endif

#ifndef TCP_FIXED_MESSAGE_LENGTH_ROLLOVER_H
#include "tcp_fixed_message_length_rollover.h"
#endif

#ifndef MESSAGE_BUFFER_H
#include "message_buffer.h"
#endif

// *===*===*===*===*===*===*===*===*===*===*===*===*===*===*===*===*===*===
// communications handler class declarations
// *===*===*===*===*===*===*===*===*===*===*===*===*===*===*===*===*===*===

class tcp_fixed_message_length_selectable_rollover_class : public tcp_fixed_message_length_rollover_class, public selectable_socket_class
{
public:
    // =====================================
    //  constructor
    // =====================================

    // 1st form: specify number of characters in message buffer payload
    // 2nd form: copy constructor
    //
    tcp_fixed_message_length_selectable_rollover_class(ostream&, const unsigned&, unsigned&);
    tcp_fixed_message_length_selectable_rollover_class(const tcp_fixed_message_length_selectable_rollover_class&);

    // =====================================
    //  manipulators
    // =====================================

    // when select() detects read activity, reading, then responding to user message, closing the socket on error
    // when select() detects write possible, treat this as an error; message passing from this routine is synchronous
    // when select() detect exception activity, close the socket; the client has been disconnected
    //
    void read_event(argument_block_type&);
    void write_event(argument_block_type&);
    void exception_event(argument_block_type&);

    // ======================================
    //  destructor
    // ======================================
    //
    virtual ~tcp_fixed_message_length_selectable_rollover_class(void);

private:
    // ======================================
    //  private functions
    // ======================================

    // close the connection, notifying client and removing socket from the select() pool
    //
    void terminate_connection(argument_block_type&);

    // ======================================
    //  private member data
    // ======================================
    //
    // stream for directing messages to client program
    // reference to variable for counting connected clients
    // variable for capturing, manipulating client messages
    //
    ostream& feedback_stream_;
    unsigned& connected_client_count_;
    string message_;
};

#endif