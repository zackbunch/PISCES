//--------------udp_fixed_message_length_client.h----------------------------
//
//   Author:   Phil Pfeiffer
//   Date:     August 1999
//   Last modified:  April 2020
//
//   Simplify management of a UDP communication by implementing a UDP client
//   endpoint that communicates via fixed length messages
//-----------------------------------------------------------------------------

#ifndef UDP_FIXED_MESSAGE_LENGTH_CLIENT_H
#define UDP_FIXED_MESSAGE_LENGTH_CLIENT_H

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

#ifndef UDP_PEER_H
#include "udp_peer.h"    // TCP/IP host data class
#endif

#ifndef MESSAGE_BUFFER_H
#include "message_buffer.h"    // message buffering class
#endif

// *===*===*===*===*===*===*===*===*===*===*===*===*===*===*===*===*===*===
// communications handler class declaration
// *===*===*===*===*===*===*===*===*===*===*===*===*===*===*===*===*===*===

class udp_fixed_message_length_client_class : public udp_peer_socket_class
{
public:
    // =====================================
    //  constructor
    // =====================================
    //
    //  specify number of characters in message buffer payload
    //
    udp_fixed_message_length_client_class(const unsigned&);

    // =====================================
    //  manipulators
    // =====================================
    //
    // send string to server; recover string from server

    void request(const string&, const socket_service_access_point_class&);
    void confirm(string&, socket_service_access_point_class&);

    // ======================================
    //  destructor
    // ======================================
    //
    virtual ~udp_fixed_message_length_client_class(void);

private:
    // ======================================
    //  private member data
    // ======================================

    // internal object for message buffering
    //
    fixed_message_length_buffer_class buffer_;

    // internal object for exchanging data with send() routine
    //
    argument_block_type arg_block_;

    // ======================================
    //  private methods
    // ======================================
    //
    // check if message to be sent is too large--throw exception if so

    void validate_outgoing_message_length(const string&);
};

#endif
