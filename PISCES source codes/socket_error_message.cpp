// -------------------------socket_error_message.cpp ------------------------------------------
//
//   Author:   Phil Pfeiffer
//   Date:     August 1999
//   Last modified:  April 2020
//
//   Translate (WinSock's) socket API error codes into intelligible messages
//
//   The error messages for the WinSock interface were developed from
//   the Winsock 1.1 standard,  "Windows Sockets 1.1 Specification - Socket Library Reference"
//   (URL:  http://www.winsock.com/wsresource/winsock1/ws_c.html).
//----------------------------------------------------------------------------------------------

#include "socket_error_message.h"

// =====================================================
//  static class data
// =====================================================

// *** for ensuring that supporting socket DLL is loaded and unloaded ***

sockets_dll_init_class socket_error_message_class::dll_init_object_;

// =====================================================
//  constructors
// =====================================================

//-------------------------------------------------------------
//  first form of constructor -
//     generate error message for user-supplied routine,
//     relative to global error variable
//-------------------------------------------------------------
//
socket_error_message_class::socket_error_message_class(const string& caller_name, const long error_code)
    : error_message_(get_socket_error_message(error_code, caller_name))
{
}

//-------------------------------------------------------------
//  second form of constructor - copy constructor
//-------------------------------------------------------------
//
socket_error_message_class::socket_error_message_class(const socket_error_message_class& source)
    : error_message_(source.error_message_)
{
}

// ======================================
//  assignment operator -- standard object copy
// ======================================
//
socket_error_message_class& socket_error_message_class::operator =(const socket_error_message_class& source)
{
    error_message_ = source.error_message_;
    return *this;
}

// ======================================
//  inspector
// ======================================

//--------------------------------------------------------------
//  get_message() - get object's error message
//--------------------------------------------------------------
//
string socket_error_message_class::get_message(void) const
{
    return error_message_;
}

// ======================================
//  facilitator
// ======================================

//--------------------------------------------------------------
//  insert() - insert error message into stream
//--------------------------------------------------------------
//
void socket_error_message_class::insert(ostream& os) const
{
    os << error_message_;
}

// ====================================
// destructor
// ====================================
//
socket_error_message_class::~socket_error_message_class(void)
{
}

// *===*====*===*====*===*====*===*====*===*====*===*====*===*====*===*====
// auxiliary operators  [boilerplate]
// *===*====*===*====*===*====*===*====*===*====*===*====*===*====*===*====

ostream& operator<<(ostream& os, const socket_error_message_class& ex)
{
    ex.insert(os);
    return os;
}