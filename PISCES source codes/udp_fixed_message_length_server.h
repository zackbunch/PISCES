//--------------udp_fixed_message_length_server.h----------------------------
//
//   Author:   Phil Pfeiffer
//   Date:     August 1999
//   Last modified:  April 2020
//
//   Simplify management of a UDP communication by implementing a UDP server
//   endpoint that communicates via fixed length messages
//---------------------------------------------------------------------------

#ifndef UDP_FIXED_MESSAGE_LENGTH_SERVER_H
#define UDP_FIXED_MESSAGE_LENGTH_SERVER_H

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
#include "udp_peer.h"     // udp server socket class
#endif

#ifndef MESSAGE_BUFFER_H
#include "message_buffer.h"    // message buffering class
#endif

// *===*===*===*===*===*===*===*===*===*===*===*===*===*===*===*===*===*===
// communications handler class declaration
// *===*===*===*===*===*===*===*===*===*===*===*===*===*===*===*===*===*===
//

class udp_fixed_message_length_server_class : public udp_peer_socket_class
{
public:
    // =====================================
    //  constructor
    // =====================================
    //
    // specify number of characters in message buffer payload
    //
    udp_fixed_message_length_server_class(const unsigned&);

    // =====================================
    //  manipulators
    // =====================================
    //
    // get string from client

    void indication(string&, socket_service_access_point_class&);
    void response(void);    // return response to client (assumes indication data present in buffer)

    // ======================================
    //  destructor
    // ======================================
    //
    virtual ~udp_fixed_message_length_server_class(void);

private:
    // ======================================
    //  private member data
    // ======================================

    // internal object for message buffering, with size
    //
    fixed_message_length_buffer_class buffer_;
    const unsigned buffer_size_;

    // for capturing, manipulating client messages
    //
    string message_;
    socket_service_access_point_class client_sap_;

    // internal object for exchanging data with send() routine
    //
    argument_block_type arg_block_;
};

#endif
