//--------------tcp_variable_message_length_rollover.cpp--------------------------------
//
//  Author:  Phil Pfeiffer
//  Date:    Fall 1999
//  Last modified:  April 2020
//
//   Simplify management of a TCP communication by implementing a TCP rollover
//   endpoint that communicates via variable length messages
//---------------------------------------------------------------------------------------

#include "tcp_variable_message_length_rollover.h"

// =====================================
//  constructor
// =====================================

//-------------------------------------------------------------------------
// constructor - initialize send-receive processing
//
// functions:
//
// -.  constructor initializes internal variables used for transmission,
//     i.e.--
//     -.  a buffer for receiving message headers
//     -.  a variable that holds the number of chars in the header buffer
//
// -.  constructor initializes internal variables that characterize
//     transmission parameters, i.e.--
//     -.  the number of characters in a message header buffer
//     -.  the largest value that can be written into a message header buffer.
//         this value, MAX_MESSAGE_LENGTH, constrains length of message payload.
//
//     Note: MAX_MESSAGE_LENGTH set to ( 2^(# bits in message_length_type) ) -1.
//     this computation arranged in a way to prevent overflow on exponentiation.
//
// -.  constructor initializes references to the object's internal message
//     transmission buffer,recv_header_buf_.  References are initialized within
//     internal argument block data structure, arg_block_:
//
//      -.  arg_block_[ socket_class::P_BUFFER_ADDRESS ] 
//              set to reference recv_header_buf_.p_buffer(), the first byte in the buffer.
//      -.  arg_block_[ socket_class::P_BUFFER_SIZE ] 
//              set to reference recv_header_buf_.p_size(), the number of bytes in the buffer.
//
//      rationale:  arg block structure used by socket wrapper class send() and
//      recv() methods to identify buffering parameters for message exchange.
//-------------------------------------------------------------------------
//
tcp_variable_message_length_rollover_class::tcp_variable_message_length_rollover_class(const unsigned& payload_size)
    :
    tcp_rollover_socket_class(),
    recv_header_buf_(sizeof(message_length_type)),
    messageHeaderSize_(sizeof(message_length_type)),
    max_message_length_(
        (1 << (8 * sizeof(message_length_type) - 1))
        - 1
        + (1 << (8 * sizeof(message_length_type) - 1))
    )
{
    //  *** set up parameters for message transmission that won't change ***
    //
    recv_header_arg_block_[socket_class::P_BUFFER_ADDRESS] = static_cast<void*>(recv_header_buf_.p_buffer());
    recv_header_arg_block_[socket_class::P_BUFFER_SIZE] = static_cast<void*>(recv_header_buf_.p_size());
}

// =====================================
//  manipulators
// =====================================

//---------------------------------------------------------------------------------------------------------------
//  indication() - receive request (i.e., service indication) from client
//
//  preconditions:
//
//  -.  recv_header_arg_block_ references member items in the object's internal
//      message transmission data structure, recv_header_buf_:
//
//      -.  recv_header_arg_block_[ socket_class::P_BUFFER_ADDRESS ] references
//          recv_header_buf_.p_buffer(), the first byte in the buffer.
//      -.  recv_header_arg_block_[ socket_class::P_BUFFER_SIZE ] references
//          recv_header_buf_.p_size(), the number of bytes in the buffer.
//
//  effects:
//
//  -.   indication() calls recv() to receive message header, which uses
//            recv_header_arg_block_[ socket_class::P_BUFFER_ADDRESS ]
//            and recv_header_arg_block_[ socket_class::P_BUFFER_SIZE ]
//       as buffer parameters for receiving message.
//  -.   indication() then calls recv() to receive message payload, which uses
//            arg_block[ socket_class::P_BUFFER_ADDRESS ] and arg_block[ socket_class::P_BUFFER_SIZE ]
//       as buffer parameters for receiving message.
//  -.   indication() then loads reply into user-supplied message string, and local message_ variable.
//---------------------------------------------------------------------------------------------------------------
//
void tcp_variable_message_length_rollover_class::indication(string& message)
{
    //  *** get first part of client's indication--incoming message length ***

    recv(recv_header_arg_block_);

    //  == extract payload size from header ===
    //
    message_length_type payload_size;

    recv_header_buf_.reset();   // set buffer back to start for emptying
    recv_header_buf_ >> payload_size;
    payload_size = ntohs(payload_size);  // convert network->local byte order

    //  *** receive balance of message ***

    if (payload_size == 0)
    {
        message_ = "";
        return;
    }

    //  == set up for receive ==
    //
    fixed_message_length_buffer_class buffer(payload_size);

    argument_block_type arg_block;

    arg_block[socket_class::P_BUFFER_ADDRESS] = static_cast<void*>(buffer.p_buffer());
    arg_block[socket_class::P_BUFFER_SIZE] = static_cast<void*>(buffer.p_size());

    recv(arg_block);

    //  *** extract message from buffer ****
    //
    message_ = string(payload_size, ' ');
    for (unsigned i = 0; i < payload_size; i++) buffer >> message_[i];

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
//  -.  response() reverses message from client, storing in buffer_, including initial length-count header
//  -.  response() then calls send() to send message, which uses
//         arg_block_[ socket_class::P_BUFFER_ADDRESS ] and arg_block_[ socket_class::P_BUFFER_SIZE ]
//         as buffer parameters for sending message.
//-------------------------------------------------------------------------
//
void tcp_variable_message_length_rollover_class::response(void)
{
    //  *** create buffer, and fill with received message--reversed ***
    //
    const message_length_type message_length = static_cast<message_length_type>(message_.length());

    fixed_message_length_buffer_class buffer(messageHeaderSize_ + message_length);

    buffer << htons(message_length);   // convert local->network byte order
    for (unsigned i = 0; i < message_length; i++)
        buffer << message_[message_length - 1 - i];

    //  *** do the send ***

    //  set up to pass send()
    //  the addresses of the routine's buffer, buf length items
    //
    argument_block_type arg_block;

    arg_block[socket_class::P_BUFFER_ADDRESS] = static_cast<void*>(buffer.p_buffer());
    arg_block[socket_class::P_BUFFER_SIZE] = static_cast<void*>(buffer.p_size());

    send(arg_block);
}

// ======================================
//  destructor
// ======================================

tcp_variable_message_length_rollover_class::~tcp_variable_message_length_rollover_class(void)
{
    shutdown();          // close connection gracefully
    closesocket();       // just in case the socket is still open
}