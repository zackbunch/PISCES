//--------------udp_fixed_message_length_client.cpp----------------------------
//
//   Author:   Phil Pfeiffer
//   Date:     August 1999
//   Last modified:  April 2020
//
//   Simplify management of a UDP communication by implementing a UDP client
//   endpoint that communicates via fixed length messages
//-----------------------------------------------------------------------------

#include "udp_fixed_message_length_client.h"

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
//      -.  arg_block_[ socket_class::P_BUFFER_ADDRESS ] set to reference buffer_.p_buffer(), the first byte in the buffer.
//      -.  arg_block_[ socket_class::P_BUFFER_SIZE ] set to reference buffer_.p_size(), the number of bytes in the buffer.
//
//    rationale:  arg block structure used by socket wrapper class sendto() and
//    recvfrom() methods to identify buffering parameters for message exchange.
//
//-------------------------------------------------------------------------
//
udp_fixed_message_length_client_class::udp_fixed_message_length_client_class(const unsigned& payload_size)
    : udp_peer_socket_class(), buffer_(payload_size)
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
//  request() - send request for action to server
//
//  preconditions:
//
//  -.  arg_block_ references member items in the object's internal
//      message buffer, buffer_:
//
//      -.  arg_block_[ socket_class::P_BUFFER_ADDRESS ] references buffer_.p_buffer(),
//          the first byte in the buffer.
//      -.  arg_block_[ socket_class::P_BUFFER_SIZE ] references buffer_.p_size(),
//          the number of bytes in the buffer.
//
//  effects:
//
//  -.   request() stores user-supplied message in buffer_.
//  -.   request() then stores reference to target SAP in
//          arg_block_[ socket_class::P_SERVER_ACCESS_POINT ], as required by sendto().
//  -.   request() then calls send() to send the message, which uses
//          arg_block_[ socket_class::P_BUFFER_ADDRESS ],
//          arg_block_[ socket_class::P_BUFFER_SIZE ], and
//          arg_block_[ socket_class::P_SERVER_ACCESS_POINT ]
//          as buffer and address parameters for sending the message.
//
//-------------------------------------------------------------------------
//
void udp_fixed_message_length_client_class::request(const string& message, const socket_service_access_point_class& server_sap)
{
    //  throw exception if length of specified message is too big to fit header
    //
    validate_outgoing_message_length(message);

    //  *** fill buffer with blank-padded message ***
    //
    buffer_.reset();   // set buffer back to start for filling
    unsigned long i = 0;
    while (i < (*buffer_.p_size()) && i < message.length()) buffer_ << message[i++];
    while (i++ < (*buffer_.p_size())) buffer_ << ' ';

    //  *** do the send ***
    //
    arg_block_[socket_class::P_SERVER_ACCESS_POINT] = static_cast<void*>(const_cast<socket_service_access_point_class*>(&server_sap));
    sendto(arg_block_);
}

//-------------------------------------------------------------------------
//  confirm() - receive confirmation of action from server
//
//  preconditions:
//
//  -.  arg_block_ references member items in the object's internal
//      message transmission data structure, buffer_:
//
//      -.  arg_block_[ socket_class::P_BUFFER_ADDRESS ] references buffer_.p_buffer(), the first byte in the buffer.
//      -.  arg_block_[ socket_class::P_BUFFER_SIZE ] references buffer_.p_size(), the number of bytes in the buffer.
//
//  effects:
//
//  -.   confirm() sets arg_block_[ socket_class::P_SERVER_ACCESS_POINT ],
//         the address where recvfrom() will store the transmitting host's SAP,
//         to the address of the caller's SAP capture variable.
//  -.   confirm() then calls recvfrom(), which uses
//          arg_block_[ socket_class::P_BUFFER_ADDRESS ],
//          arg_block_[ socket_class::P_BUFFER_SIZE ], and
//          arg_block_[ socket_class::P_SERVER_ACCESS_POINT ]
//          as buffer and address parameters for sending the message.
//  -.   confirm() then loads reply into user-supplied reply string.
//
//-------------------------------------------------------------------------
//
void udp_fixed_message_length_client_class::confirm(string& message, socket_service_access_point_class& server_sap)
{
    //  *** get the server's confirmation ***
    //
    arg_block_[socket_class::P_SERVER_ACCESS_POINT] = static_cast<void*>(const_cast<socket_service_access_point_class*>(&server_sap));
    recvfrom(arg_block_);

    //  *** move reply to user-supplied string
    //
    buffer_.reset();   // set buffer back to start for filling

    message = string((*buffer_.p_size()), ' ');
    for (unsigned long i = 0; i < (*buffer_.p_size()); i++) buffer_ >> message[i];
}

// ======================================
//  destructor
// ======================================

udp_fixed_message_length_client_class::~udp_fixed_message_length_client_class(void)
{
    closesocket();         // just in case socket is still open
}

// ======================================
//  private methods
// ======================================
//
// check if received message length parameter is valid, throwing exception if not

void udp_fixed_message_length_client_class::validate_outgoing_message_length(const string& message)
{
    //  *** set up static variables for class error messages ***
    //
    static const string my_name = "udp_fixed_message_length_client_class::request";
    static const string too_big = "invalid length specified for outgoing message";

    if (message.length() > (*buffer_.p_size()))
    {
        stringstream p_error_message;
        p_error_message << "specified message (" << message << ") exceeds maximum message (" << (*buffer_.p_size()) << ")";
        throw socket_exception(my_name, too_big, p_error_message.str());
    }
}