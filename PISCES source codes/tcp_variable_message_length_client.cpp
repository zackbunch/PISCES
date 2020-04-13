//--------------tcp_variable_message_length_client.cpp------------------------------------
//
//  Author:  Phil Pfeiffer
//  Date:    Fall 1999
//  Last modified:  April 2020
//
//   Simplify management of a TCP communication by implementing a TCP client
//   endpoint that communicates via variable length messages
//----------------------------------------------------------------------------------------

#include "tcp_variable_message_length_client.h"

#include <sstream>   // stringstream

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
//     Note:  MAX_MESSAGE_LENGTH set to ( 2^(# bits in message_length_type) ) -1.
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
tcp_variable_message_length_client_class::tcp_variable_message_length_client_class(void)
    :
    tcp_client_socket_class(),
    recv_header_buf_(sizeof(message_length_type)),
    message_header_size_(sizeof(message_length_type)),
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

//-------------------------------------------------------------------------
//  request() - send request for action to server
//
//  effects:
//
//  -.   request() stores user-supplied message in buffer_,
//         including a header consisting of a message length.
//  -.   request() then calls send() to send message, which uses
//         arg_block_[ socket_class::P_BUFFER_ADDRESS ] and arg_block_[ socket_class::P_BUFFER_SIZE ]
//         as buffer parameters for sending message.
//-------------------------------------------------------------------------
//
void tcp_variable_message_length_client_class::request(const string& message)
{
    //  *** create buffer, and fill with incoming message ***

    //  throw exception if length of specified message is too big to fit header
    //
    validate_outgoing_message_length(message);

    //  allocate, then fill buffer
    //
    const message_length_type message_length = static_cast<message_length_type>(message.length());
    fixed_message_length_buffer_class buffer(message_header_size_ + message_length);

    buffer << htons(message_length);   // convert local->network byte order
    for (unsigned i = 0; i < message_length; i++) buffer << message[i];

    //  *** do the send ***

    //  set up to pass send()
    //  the addresses of the routine's buffer, buf length items
    //
    argument_block_type arg_block;

    arg_block[socket_class::P_BUFFER_ADDRESS] = static_cast<void*>(buffer.p_buffer());
    arg_block[socket_class::P_BUFFER_SIZE] = static_cast<void*>(buffer.p_size());

    send(arg_block);
}

//-------------------------------------------------------------------------
//  confirm() - receive confirmation of action from server
//
//  preconditions:
//
//  -.  recv_header_arg_block_ references member items in the object's internal
//      message transmission data structure, recv_header_buf_:
//
//      -.  recv_header_arg_block_[ socket_class::P_BUFFER_ADDRESS ] references recv_header_buf_.p_buffer(), the first byte in the buffer.
//      -.  recv_header_arg_block_[ socket_class::P_BUFFER_SIZE ] references recv_header_buf_.p_size(), the number of bytes in the buffer.
//
//  effects:
//
//  -.   confirm() calls recv() to receive message header, which uses
//            recv_header_arg_block_[ socket_class::P_BUFFER_ADDRESS ]
//            and recv_header_arg_block_[ socket_class::P_BUFFER_SIZE ]
//       as buffer parameters for receiving message.
//  -.   confirm() then calls recv() to receive message payload, which uses
//            arg_block[ socket_class::P_BUFFER_ADDRESS ] and arg_block[ socket_class::P_BUFFER_SIZE ]
//       as buffer parameters for receiving message.
//  -.   confirm() then loads reply into user-supplied reply string.
//-------------------------------------------------------------------------
//
void tcp_variable_message_length_client_class::confirm(string& reply)
{
    // *** get first part of server's confirmation--incoming message length ***

    recv(recv_header_arg_block_);

    //  == extract payload size from header ===
    //
    message_length_type payload_size;

    recv_header_buf_.reset();   // set buffer back to start for emptying
    recv_header_buf_ >> payload_size;
    payload_size = ntohs(payload_size);   // convert network->local byte order

    //  *** receive balance of message ***

    if (payload_size == 0)
    {
        reply = "";
        return;
    }

    //  == do the receive ==
    //
    fixed_message_length_buffer_class buffer(payload_size);
    argument_block_type arg_block;

    arg_block[socket_class::P_BUFFER_ADDRESS] = static_cast<void*>(buffer.p_buffer());
    arg_block[socket_class::P_BUFFER_SIZE] = static_cast<void*>(buffer.p_size());

    recv(arg_block);

    //  == extract message from buffer ==
    //
    reply = string(payload_size, ' ');
    for (unsigned i = 0; i < payload_size; i++) buffer >> reply[i];
}

// ======================================
//  destructor
// ======================================

tcp_variable_message_length_client_class::~tcp_variable_message_length_client_class(void)
{
    shutdown();          // close connection gracefully
    closesocket();       // just in case the socket is still open
}

// ======================================
//  private methods
// ======================================
//
// check if received message length parameter is valid, throwing exception if not

void tcp_variable_message_length_client_class::validate_outgoing_message_length(const string& message)
{
    //  *** set up static variables for class error messages ***
    //
    static const string my_name = "tcp_variable_message_length_client_class::request";
    static const string too_big = "invalid length specified for outgoing message";

    if (message.length() > max_message_length_)
    {
        stringstream p_message;
        p_message << "specified message (" << message << ") exceeds maximum message length (" << max_message_length_ << ")";
        throw socket_exception(my_name, too_big, p_message.str());
    }
}