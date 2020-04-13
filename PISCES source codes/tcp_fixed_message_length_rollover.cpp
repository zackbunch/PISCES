//--------------tcp_fixed_message_length_rollover.cpp------------------------------------
//
//  Author:  Phil Pfeiffer
//  Date:    Fall 1999
//  Last modified:  April 2020
//
//   Simplify management of a TCP communication by implementing a TCP rollover
//   endpoint that communicates via variable length messages
//----------------------------------------------------------------------------------------

#include "tcp_fixed_message_length_rollover.h"

// =====================================
//  constructor
// =====================================

//-------------------------------------------------------------------------
// constructor - initialize send-receive processing
//
// -.  initialize internal variables used for transmission, i.e.--
//     -.  buffers for receiving message headers
//     -.  argument blocks for passing buffer parameters to send() and recv()
//     -.  a variable that holds the number of chars in the header buffer
//-------------------------------------------------------------------------
//
tcp_fixed_message_length_rollover_class::tcp_fixed_message_length_rollover_class(const unsigned& payload_size)
    : tcp_rollover_socket_class(), outgoing_message_buffer_(payload_size), incoming_message_buffer_(payload_size), message_length_(payload_size)
{
    //  *** set up parameters for message transmission that won't change ***
    //
    outgoing_message_arg_block_[socket_class::P_BUFFER_ADDRESS] = static_cast<void*>(outgoing_message_buffer_.p_buffer());
    outgoing_message_arg_block_[socket_class::P_BUFFER_SIZE] = static_cast<void*>(outgoing_message_buffer_.p_size());

    incoming_message_arg_block_[socket_class::P_BUFFER_ADDRESS] = static_cast<void*>(incoming_message_buffer_.p_buffer());
    incoming_message_arg_block_[socket_class::P_BUFFER_SIZE] = static_cast<void*>(incoming_message_buffer_.p_size());
}

// =====================================
//  manipulators
// =====================================

//---------------------------------------------------------------------------------------------------------------
//  indication() - receive request (i.e., service indication) from client
//
//  preconditions:
//  -.  incoming_message_arg_block_[ socket_class::P_BUFFER_ADDRESS ] references
//      incoming_message_buffer_.p_buffer(), the first byte in the buffer.
//  -.  incoming_message_arg_block_[ socket_class::P_BUFFER_SIZE ] references
//      incoming_message_buffer_.p_size(), the number of bytes in the buffer.
//
//  effects:
//  -.   indication() calls recv() to receive message payload, which uses
//            incoming_message_arg_block_[ socket_class::P_BUFFER_ADDRESS ] and
//            incoming_message_arg_block_[ socket_class::P_BUFFER_SIZE ]
//            as buffer parameters for receiving message.
//  -.   indication() then loads reply into user-supplied message string, and local message_ variable.
//
//---------------------------------------------------------------------------------------------------------------
//
void tcp_fixed_message_length_rollover_class::indication(string& message)
{
    incoming_message_buffer_.reset();
    recv(incoming_message_arg_block_);

    //  *** extract message from buffer ****
    //
    message_ = string(message_length_, ' ');
    for (unsigned i = 0; i < message_length_; i++) incoming_message_buffer_ >> message_[i];

    //  *** return message to caller ***
    //
    message = message_;
}

//-------------------------------------------------------------------------
//  response() - send response to client appropriate to required action
//
//  preconditions:
//  -.  last message from client currently stored in private object variable, message_.
//
//  effects:
//  -.  response() reverses message from client, storing in outgoing_message_buffer_
//  -.  response() then calls send() to send message, which uses
//         outgoing_message_arg_block_[ socket_class::P_BUFFER_ADDRESS ] and
//         outgoing_message_arg_block_[ socket_class::P_BUFFER_SIZE ]
//         as buffer parameters for sending message.
//
//-------------------------------------------------------------------------
//
void tcp_fixed_message_length_rollover_class::response(void)
{
    //  *** reverse the received message, then do the send ***
    //
    outgoing_message_buffer_.reset();
    for (unsigned i = 0; i < message_length_; i++)
        outgoing_message_buffer_ << message_[message_length_ - 1 - i];

    send(outgoing_message_arg_block_);
}

// ======================================
//  destructor
// ======================================

tcp_fixed_message_length_rollover_class::~tcp_fixed_message_length_rollover_class(void)
{
    shutdown();          // close connection gracefully
    closesocket();       // just in case the socket is still open
}