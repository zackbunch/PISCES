//--------------tcp_fixed_message_length_client.cpp---------------------------------------
//
//  Author:  Phil Pfeiffer
//  Date:    Fall 1999
//  Last modified:  April 2020
//
//   Simplify management of a TCP communication by implementing a TCP client
//   endpoint that communicates via variable length messages
//----------------------------------------------------------------------------------------

#include "tcp_fixed_message_length_client.h"

#include <sstream>   // stringstream

// =====================================
//  constructor
// =====================================

//---------------------------------------------------------------------------------
// constructor - initialize send-receive processing
//
// -.  initialize internal variables used for transmission, i.e.--
//     -.  buffers for receiving message headers
//     -.  argument blocks for passing buffer parameters to send() and recv()
//     -.  a variable that holds the number of chars in the header buffer
//---------------------------------------------------------------------------------
//
tcp_fixed_message_length_client_class::tcp_fixed_message_length_client_class(const unsigned& payload_size)
    : tcp_client_socket_class(), outgoing_message_buffer_(payload_size), incoming_message_buffer_(payload_size), message_length_(payload_size)
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

//-------------------------------------------------------------------------
//  request() - send request for action to server
//
//  effects:
//
//  -.   request() stores user-supplied message in buffer_, with padding
//  -.   request() then calls send() to send message, which uses
//         outgoing_message_arg_block_[ socket_class::P_BUFFER_ADDRESS ] and
//         outgoing_message_arg_block_[ socket_class::P_BUFFER_SIZE ]
//         as buffer parameters for sending message.
//-------------------------------------------------------------------------
//
void tcp_fixed_message_length_client_class::request(const string& message)
{
    //  *** pad the message, then do the send ***
    //
    outgoing_message_buffer_.reset();

    unsigned i;
    for (i = 0; i < message.length(); i++) outgoing_message_buffer_ << message[i];
    for (i = message.length(); i < message_length_; i++) outgoing_message_buffer_ << '*';
    send(outgoing_message_arg_block_);
}

//-------------------------------------------------------------------------
//  confirm() - receive confirmation of action from server
//
//  effects:
//
//  -.   confirm() calls recv() to receive message payload, which uses
//            incoming_message_arg_block_[ socket_class::P_BUFFER_ADDRESS ] and
//            incoming_message_arg_block_[ socket_class::P_BUFFER_SIZE ]
//            as buffer parameters for receiving message.
//  -.   confirm() then loads reply into user-supplied reply string.
//-------------------------------------------------------------------------
//
void tcp_fixed_message_length_client_class::confirm(string& reply)
{
    //  == do the receive, then extract message from buffer for client  ==
    //
    incoming_message_buffer_.reset();

    recv(incoming_message_arg_block_);
    reply = string(message_length_, ' ');
    for (unsigned i = 0; i < message_length_; i++) incoming_message_buffer_ >> reply[i];
}

// ======================================
//  destructor
// ======================================

tcp_fixed_message_length_client_class::~tcp_fixed_message_length_client_class(void)
{
    shutdown();          // close connection gracefully
    closesocket();       // just in case the socket is still open
}

// ======================================
//  private methods
// ======================================
//
// check if received message length parameter is valid, throwing exception if not

void tcp_fixed_message_length_client_class::validate_outgoing_message_length(const string& message)
{
    //  *** set up static variables for class error messages ***
    //
    static const string my_name = "tcp_fixed_message_length_client_class::request";
    static const string too_big = "invalid length specified for outgoing message";

    if (message.length() > message_length_)
    {
        stringstream p_message;
        p_message << "specified message (" << message << ") exceeds maximum message length (" << message_length_ << ")";
        throw socket_exception(my_name, too_big, p_message.str());
    }
}