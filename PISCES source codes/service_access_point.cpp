//------------------------------ service_access_point.cpp -----------------------------
//
//   Author:   Phil Pfeiffer
//   Date:     August 1999
//   Last modified:  April 2020
//
//   Capture a TCP/UDP service access point, consisting of
//   -.  a host specification (IP address),
//   -.  a port (unsigned integer between 0 and 65535, inclusive)
//   along with display and initialization methods, as an object.
//--------------------------------------------------------------------------------------

#include "service_access_point.h"

#ifndef BOOL_H
#include "bool.h"
#endif

#ifndef SOCKET_ERROR_MESSAGE_H
#include "socket_error_message.h"   // socket error message class
#endif

#include <sstream>   // stringstream

#ifndef EXCEPTION_H
#include "exception.h"   // general exception class
#endif

#ifndef STREAM_EXCEPTION_H
#include "stream_exception.h"   // stream exceptions class
#endif

// **************************************************************************
// socket_service_access_point_class
// **************************************************************************

// =*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
// socketServiceAccessPoint Class proper
// =*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*

//========================================================================
//  constructors
//========================================================================

//------------------------------------------------------------------------
// first form:
//    instantiate object with specified ipHost object, port.
//------------------------------------------------------------------------
//
socket_service_access_point_class::socket_service_access_point_class(const ip_address_class& ip_address, const port_number_class& port)
    : ip_address_(ip_address),
    port_(port)
{
}

//------------------------------------------------------------------------
// second form:
//    instantiate object with specified port, ip_address object.
//------------------------------------------------------------------------
//
socket_service_access_point_class::socket_service_access_point_class(const port_number_class& port, const ip_address_class& ip_address)
    : ip_address_(ip_address),
    port_(port)
{
}

//------------------------------------------------------------------------
// third form:   copy constructor
//------------------------------------------------------------------------
//
socket_service_access_point_class::socket_service_access_point_class(const socket_service_access_point_class& source)
    : ip_address_(source.ip_address_),
    port_(source.port_)
{
}

//========================================================================
//  assignment operators
//========================================================================

//------------------------------------------------------------------------
// first form - standard object copy
//------------------------------------------------------------------------
//
socket_service_access_point_class&
socket_service_access_point_class::
operator=(const socket_service_access_point_class& source)
{
    ip_address_ = source.ip_address_;
    port_ = source.port_;
    return *this;
}

//------------------------------------------------------------------------
// second form - update object's IP host member
//------------------------------------------------------------------------
//
socket_service_access_point_class&
socket_service_access_point_class::operator=(const ip_address_class& ip_address)
{
    ip_address_ = ip_address;
    return *this;
}

//------------------------------------------------------------------------
// third form - update object's port number
//------------------------------------------------------------------------
//
socket_service_access_point_class&
socket_service_access_point_class::operator=(const port_number_class& port)
{
    port_ = port;
    return *this;
}

// ======================================
//  other operators
// ======================================

//-----------------------------------------------------------------------
// logical operators
//-----------------------------------------------------------------------
//
int socket_service_access_point_class::
operator==(const socket_service_access_point_class& other) const
{
    return (ip_address_ == other.ip_address_) && (port_ == other.port_);
}

int socket_service_access_point_class::
operator!=(const socket_service_access_point_class& other) const
{
    return (ip_address_ != other.ip_address_) || (port_ != other.port_);
}

// ======================================
//  manipulators
// ======================================

//------------------------------------------------------------------------------
//  extract() -
//     try to initialize SAP parameters, based on user-supplied data.
//     check first for host identifier, then whitespace, then port number.
//------------------------------------------------------------------------------

void socket_service_access_point_class::extract(istream& is)
{
    // **** instantiate static objects for error messages ****

    static const string my_name = "socket_service_access_point_class::extract";
    static const string bad_string = "improperly formatted input stream";
    static const string no_access_point = "can't get access point from head of stream";

    // **** get host address, port number from head of stream ****

    try
    {
        is >> ip_address_;
    }
    catch (const basic_exception& message)
    {
        throw instream_format_exception(is, my_name, bad_string, no_access_point, message);
    }

    try
    {
        is >> port_;
    }
    catch (const basic_exception& message)
    {
        stringstream p_message;
        p_message << no_access_point << " ( for host " << ip_address_ << " )";
        throw instream_format_exception(is, my_name, bad_string, p_message.str(), message);
    }
}

//------------------------------------------------------------------------------
//  insert() - write contents of object to stream, as string
//------------------------------------------------------------------------------

void socket_service_access_point_class::insert(ostream& os) const
{
    // **** instantiate static objects for error messages ****

    static const string my_name = "ip_address_class::insert";
    static const string bad_out = "couldn't output access point to stream";

    // **** put host address, port to head of stream ****

    if (!(os << " IP "))
        throw outstream_io_error_exception(os, my_name, bad_out);

    try
    {
        os << ip_address_;
    }
    catch (const basic_exception& message)
    {
        throw outstream_io_error_exception(os, my_name, bad_out, "", message);
    }

    if (!(os << ",  port "))
        throw outstream_io_error_exception(os, my_name, bad_out);

    try
    {
        os << port_;
    }
    catch (const basic_exception& message)
    {
        throw outstream_io_error_exception(os, my_name, bad_out, "", message);
    }
}

//========================================================================
//  inspectors
//========================================================================

//------------------------------------------------------------------------
// ip_address() - return host identifier stored in object
//------------------------------------------------------------------------
//
ip_address_class socket_service_access_point_class::ip_address(void) const
{
    return ip_address_;
}

//------------------------------------------------------------------------
// port_object() - return port stored in object
//------------------------------------------------------------------------
//
port_number_class socket_service_access_point_class::port_object(void) const
{
    return port_;
}

//========================================================================
//  destructor
//========================================================================
//
socket_service_access_point_class::~socket_service_access_point_class(void)
{
}

// =*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
// socket_service_access_point_class auxiliary operators
// =*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*

istream& operator>>(istream& is, socket_service_access_point_class& target)
{
    target.extract(is);
    return is;
}

ostream& operator<<(ostream& os, const socket_service_access_point_class& source)
{
    source.insert(os);
    return os;
}