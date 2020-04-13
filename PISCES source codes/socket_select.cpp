// ----------------------socket_select.cpp--------------------------------------------------
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
// -----------------------------------------------------------------------------------------

#include "socket_select.h"

#ifndef BOOL_H
#include "bool.h"       // not all C++ environments define TRUE, FALSE
#endif

// ====================
//    constructors
// ====================

//-----------------------------------------------------
// constructor, first form-- specifies socket version
//-----------------------------------------------------
//
selectable_socket_class::selectable_socket_class(const unsigned16 dll_version)
    : socket_class(dll_version)
{
}

//-----------------------------------------------------
// constructor, first form-- copy constructor
//-----------------------------------------------------
//
selectable_socket_class::selectable_socket_class(const selectable_socket_class& source)
    : socket_class(source)
{
}

// ====================
//    inspectors
// ====================

//---------------------------------------------------------------------
// is_set() -  return nonzero value if bit for self in fd_set data structure is set
//---------------------------------------------------------------------
//
int selectable_socket_class::is_set(const fd_set& socket_flags_vector) const
{
    return FD_ISSET(socket_, &socket_flags_vector);
}

// ====================
//    manipulators
// ====================

//-----------------------------------------------------
// set() - set bit for self in fd_set data structure
//-----------------------------------------------------
//
void selectable_socket_class::set(fd_set& socket_flags_vector) const
{
    FD_SET(socket_, &socket_flags_vector);
}

// ====================
//    manipulators
// ====================

//-------------------------------------------------------------------------------------------------
// read_event() - virtual function for managing incoming data event (i.e., read complete)
//-------------------------------------------------------------------------------------------------
//
void selectable_socket_class::read_event(argument_block_type&)
{
    static const string my_name = "selectable_socket_class::read_event";

    // callback not supported in base object
    throw socket_exception(my_name, "read callback for socket not implemented");
}

//-------------------------------------------------------------------------------------------------
// write_event() - virtual function for managing outgoing data event (i.e., write complete)
//-------------------------------------------------------------------------------------------------
//
void selectable_socket_class::write_event(argument_block_type&)
{
    static const string my_name = "selectable_socket_class::write event";

    // callback not supported in base object
    throw socket_exception(my_name, "write callback for socket not implemented");
}

//-------------------------------------------------------------------------------------------------
// exception_event() - virtual function for managing exception event (i.e., exception triggered)
//-------------------------------------------------------------------------------------------------
//
void selectable_socket_class::exception_event(argument_block_type&)
{
    static const string my_name = "selectable_socket_class::exception event";

    // callback not supported in base object
    throw socket_exception(my_name, "exception callback for socket not implemented");
}

// =============================================
// other operators
// =============================================

//-------------------------------------------------------------------
//  relational operators.
//     ordering needed by client classes,
//     which use map<> to organize socket-relate data
//-------------------------------------------------------------------

int selectable_socket_class::operator< (const selectable_socket_class& other) const
{
    return (compare(other) == -1);
}

int selectable_socket_class::operator<=(const selectable_socket_class& other) const
{
    return (compare(other) < 1);
}

int selectable_socket_class::operator==(const selectable_socket_class& other) const
{
    return (compare(other) == 0);
}

int selectable_socket_class::operator!=(const selectable_socket_class& other) const
{
    return (compare(other) != 0);
}

int selectable_socket_class::operator>=(const selectable_socket_class& other) const
{
    return (compare(other) > -1);
}

int selectable_socket_class::operator> (const selectable_socket_class& other) const
{
    return (compare(other) == 1);
}

// ====================
//     destructor
// ====================
//
selectable_socket_class::~selectable_socket_class(void)
{
}

// =============================================
// private member functions
// =============================================

// ------------------------------------------------------
//  compare() -
//      returns -1, 0, or 1 according to whether
//      *this is less than, equal to, or greater than the parameter.
// ------------------------------------------------------
//
int selectable_socket_class::compare(const selectable_socket_class& other) const
{
    if (socket_ <= other.socket_)
    {
        return (socket_ < other.socket_) ? -1 : 0;
    }
    else
        return 1;
}