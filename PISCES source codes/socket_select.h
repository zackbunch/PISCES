// ----------------------socket_select.h-----------------------------------------------------------
//
//   Author:   Phil Pfeiffer   (idea thanks to Sridhar Koritala)
//   Date:     September 1999
//   Last modified:  April 2020
//
//   A **selectable socket** is an (abstract) socket with methods that support the
//   BSD sockets API and WinSock API select() method.  Specifically, a selectable
//   socket should do the following:
//   -.  manipulate an fd_set structure (the BSD sockets/WinSock structure for grouping
//       sockets) in the following three ways:
//        -.   set or clear a bit that requests that the socket be checked for activity
//        -.   check this bit's status
//        -.   specify methods for responding to read, write, and exception events
//   -.  support comparison methods for ordering sockets, thereby permitting
//       the use of map<> to organize socket-related information
// -------------------------------------------------------------------------------------------------

#ifndef SOCKET_SELECT_H
#define SOCKET_SELECT_H

#include <string>

#ifndef PLATFORM_H
#include "platform.h"
#endif

#ifndef STD_H
#include "std.h"
#endif

#ifndef SOCKET_APIS_H
#include "socket_apis.h"      // defines fd_set data type
#endif

#ifndef SOCKET_H
#include "socket.h"          // defines base class
#endif

// *******************************************************************
//   selectable socket class
// *******************************************************************

// =*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
//   selectable socket class proper
// =*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*

class selectable_socket_class : virtual public socket_class
{
    // ======================================================================
    //  friendship declarations
    // ======================================================================
    //
    //  socketEventCheckClass needs friendshp with selectable_socket_class
    //  because socket API select() method takes, as its first parameter,
    //  the ID number of a socket-- specifically, that socket with the
    //  largest socket number.  (ugh.)

    friend class multiple_socket_select_class;

public:
    // =====================================================
    //  constructor
    // =====================================================

    // first version -  specifies version of sockets dll in use
    // second version - standard copy constructor

    selectable_socket_class(const unsigned16 = SOCKETS_DLL_VERSION);
    selectable_socket_class(const selectable_socket_class&);

    // =============================================
    // inspectors
    // =============================================

    // return nonzero value if bit for self in fd_set data structure is set

    int  is_set(const fd_set&) const;

    // =============================================
    // manipulators
    // =============================================

    // set bit for self in fd_set data structure

    void set(fd_set&) const;

    // callbacks--one for each type of event
    //
    // argument_block_type defined as a map<> in socket.h

    virtual void read_event(argument_block_type&);
    virtual void write_event(argument_block_type&);
    virtual void exception_event(argument_block_type&);

    // ==============================================================
    // other operators
    // ==============================================================

    //  relational operators.  ordering needed by client classes,
    //  which use map<> to organize socket-relate data

    int operator< (const selectable_socket_class&) const;
    int operator<=(const selectable_socket_class&) const;
    int operator==(const selectable_socket_class&) const;
    int operator!=(const selectable_socket_class&) const;
    int operator>=(const selectable_socket_class&) const;
    int operator> (const selectable_socket_class&) const;

    // =============================================
    // destructor
    // =============================================

    virtual ~selectable_socket_class(void);

private:
    // =============================================
    // member functions
    // =============================================

    // return -1, 0, or 1 according to whether *this is less than, equal to, or greater than the parameter.
    int compare(const selectable_socket_class&) const;
};

#endif
