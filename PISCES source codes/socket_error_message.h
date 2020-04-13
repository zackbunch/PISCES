// -------------------------socket_error_message.cpp ----------------------------------------------
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
//-------------------------------------------------------------------------------------------------

#ifndef SOCKET_ERROR_MESSAGE_H
#define SOCKET_ERROR_MESSAGE_H

#include <string>

#ifndef PLATFORM_H
#include "platform.h"
#endif

#ifndef STD_H
#include "std.h"
#endif

#ifndef SOCKET_APIS_H
#include "socket_apis.h"
#endif

#ifndef DLLINIT_H
#include "dllinit.h"
#endif

//*****************************************************************************
//  socket error message class
//*****************************************************************************

//=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
//  socket error message class proper
//=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*

class socket_error_message_class
{
public:
    // ===========================================================================================
    //  constructors
    // ===========================================================================================
    //
    //  class supports for types of constructors
    //
    //   first  form:   generates error message object using
    //      -.  name of routine that precipitated error (caller-specified)
    //      -.  error code (default: global error variable)
    //   second form:   copy constructor
    //
    socket_error_message_class(const string & = "", const long = get_last_socket_error());
    socket_error_message_class(const socket_error_message_class&);

    // ======================================
    //  assignment
    // ======================================
    //
    // standard assignment operator

    socket_error_message_class& operator=(const socket_error_message_class&);

    // ======================================
    //  inspector
    // ======================================
    //
    //  get_message() - get object's error message
    //
    string get_message(void) const;

    // ======================================
    //  facilitator
    // ======================================
    //
    //  insert() - store message contained in class to stream
    //
    void insert(ostream&) const;

    // ======================================
    //  destructor
    // ======================================
    //
    virtual ~socket_error_message_class(void);

private:
    // ---------------------------------------------------------------------
    // member data
    // ---------------------------------------------------------------------

    // for ensuring that supporting socket DLL is loaded and unloaded
    // default (void) constructor =>  load default version of DLL
    // make this static since DLL only needs to be loaded once.
    //
    static sockets_dll_init_class dll_init_object_;

    // error message string
    //
    string error_message_;
};

//=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
//  auxiliary operators
//=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*

ostream& operator<<(ostream&, const  socket_error_message_class&);

#endif
