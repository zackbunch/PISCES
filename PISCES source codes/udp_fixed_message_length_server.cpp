//--------------udp_fixed_message_length_server.cpp----------------------------
//
//   Author:   Phil Pfeiffer
//   Date:     August 1999
//   Last modified:  April 2020
//
//   Simplify management of a UDP communication by implementing a UDP server
//   endpoint that communicates via fixed length messages
//-----------------------------------------------------------------------------

#include "udp_fixed_message_length_server.h"

// *===*===*===*===*===*===*===*===*===*===*===*===*===*===*===*===*===*===
// communications handler class definition
// *===*===*===*===*===*===*===*===*===*===*===*===*===*===*===*===*===*===

// =====================================
//  constructor
// =====================================

//-------------------------------------------------------------------------
// constructor - initialize send-receive processing
//
// functions:
//
// -.  constructor sets number of characters in message buffer payload,
//     using payload size parameter.
//
// -.  constructor initializes references to the object's internal message
//     transmission buffer, buffer_.  References are initialized within
//     internal argument block data structure, arg_block_:
//
//      -.  arg_block_[ socket_class::P_BUFFER_ADDRESS ] set to reference buffer_.p_buffer(),  the first byte in the buffer.
//      -.  arg_block_[ socket_class::P_BUFFER_SIZE ] set to reference buffer_.p_size(), the number of bytes in the buffer.
//
//    rationale:  arg block structure used by socket wrapper class sendto() and
//    recvfrom() methods to identify buffering parameters for message exchange.
//
//-------------------------------------------------------------------------
//
udp_fixed_message_length_server_class::udp_fixed_message_length_server_class(const unsigned& buffer_size) :
    buffer_(buffer_size), buffer_size_(buffer_size), udp_peer_socket_class()
{
    //  pass send() the addresses of the routine's buffer, buf length items
    //
    arg_block_[socket_class::P_BUFFER_ADDRESS] = static_cast<void*>(buffer_.p_buffer());
    arg_block_[socket_class::P_BUFFER_SIZE] = static_cast<void*>(buffer_.p_size());
}

// =====================================
//  manipulators
// =====================================

//-------------------------------------------------------------------------
//  indication() - receive request (i.e., service indication) from client
//
//  preconditions:
//
//  -.  arg_block_ references member items in the object's internal
//      message transmission data structure, buffer_:
//
//      -.  arg_block_[ socket_class::P_BUFFER_ADDRESS ] references buffer_.p_buffer(),
//          the first byte in the buffer.
//      -.  arg_block_[ socket_class::P_BUFFER_SIZE ] references buffer_.p_size(),
//          the number of bytes in the buffer.
//
//  effects:
//
//  -.   indication() sets arg_block_[ p_server_access_point ],
//         the address where recvfrom() will store the transmitting host's SAP,
//         to the address of the caller's SAP capture variable.
//  -.   indication() calls recvfrom() to receive request, which uses
//         arg_block_[ socket_class::P_BUFFER_ADDRESS ] and arg_block_[ socket_class::P_BUFFER_SIZE ]
//         as buffer parameters for receiving message.
//  -.   indication() then loads reply into user-supplied reply string.
//
//-------------------------------------------------------------------------
//
void udp_fixed_message_length_server_class::indication(string& message, socket_service_access_point_class& client_sap)
{
    //  *** get the server's confirmation ***
    //
    arg_block_[socket_class::P_SERVER_ACCESS_POINT] = static_cast<void*>(const_cast<socket_service_access_point_class*>(&client_sap));
    recvfrom(arg_block_);

    //  *** keep client SAP locally for reply
    //
    client_sap_ = client_sap;

    //  *** move reply to user-supplied string
    //
    buffer_.reset();   // set buffer back to start for filling

    message_ = string((*buffer_.p_size()), ' ');
    for (unsigned i = 0; i < (*buffer_.p_size()); i++) buffer_ >> message_[i];

    message = message_;
}

//----------------------------------------------------------------------------------------------------------------------
//  response() - send response to client appropriate to required action
//
//  preconditions:
//
//  -.  arg_block_ references member items in the object's internal message buffer, buffer_:
//
//      -.  arg_block_[ socket_class::P_BUFFER_ADDRESS ] references buffer_.p_buffer(), the first byte in the buffer.
//      -.  arg_block_[ socket_class::P_BUFFER_SIZE ] references buffer_.p_size(), the number of bytes in the buffer.
//
//  -.  last message from client currently stored in private object variable, message_.
//
//  effects:
//
//  -.   response() stores reversed client-supplied message in buffer_.
//  -.   response() then stores reference to target SAP in arg_block_[ socket_class::P_SERVER_ACCESS_POINT ], as required by sendto().
//  -.   response() then calls send() to send the message, which uses
//          arg_block_[ socket_class::P_BUFFER_ADDRESS ],
//          arg_block_[ socket_class::P_BUFFER_SIZE ], and
//          arg_block_[ socket_class::P_SERVER_ACCESS_POINT ]
//          as buffer and address parameters for sending the message.
//
//----------------------------------------------------------------------------------------------------------------------
//
void udp_fixed_message_length_server_class::response(void)
{
    //  *** move reply to internal string ***
    //
    buffer_.reset();   // set buffer back to start for dumping

    //  *** reverse string in buffer ***
    //
    const unsigned message_length = message_.length();

    for (unsigned i = 0; i < message_length; i++) buffer_ << message_[message_length - 1 - i];

    //  *** do the send ***
    //
    arg_block_[socket_class::P_SERVER_ACCESS_POINT] = static_cast<void*>(&client_sap_);
    sendto(arg_block_);
}

// ======================================
//  destructor
// ======================================
//
udp_fixed_message_length_server_class::~udp_fixed_message_length_server_class(void)
{
    closesocket();        // just in case socket is still open
}