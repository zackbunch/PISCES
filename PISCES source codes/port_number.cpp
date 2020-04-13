//--------------port_number.cpp----------------------------------------------------------
//
//  Authors:       Phil Pfeiffer
//  Last Updated:  August 1999
//  Last modified:  April 2020
//
//  Wrapper class that encapsulates sockets API operations on port numbers:
//  integers in the range 0..65535 that TCP and UDP associate with service end points
//  for a given host.
//
//  This module defines two notions of a service port:
//  -.   an unsigned value in the range 0..65535 (any port)
//  -.   an unsigned value in the range 2000..65535 (for user-defined services)
//       standard practice:  systems reserve lower-numbered ports for system services
//---------------------------------------------------------------------------------------

#include "port_number.h"

#include <sstream>   // stringstream

#ifndef EXCEPTION_H
#include "exception.h"   // general exception class
#endif

#ifndef STREAM_EXCEPTION_H
#include "stream_exception.h"   // stream exceptions class
#endif

//***************************************************************
//  port_number_class - for standard range of port numbers
//***************************************************************

//=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
//  port_number_class proper
//=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*

//================================================
//  constructors
//================================================

//---------------------------------------------------------------
// constructor, first form - set port number to specified number
//---------------------------------------------------------------
//
port_number_class::port_number_class(const port_number_type port)
    : port_(port)
{
}

//---------------------------------------------------------------
// constructor, second form - standard copy constructor
//---------------------------------------------------------------
//
port_number_class::port_number_class(const port_number_class& source)
    : port_(source.port_)
{
}

//===============================================
//  assignment operator
//===============================================

//--------------------------------------------------------------------------
// first form - standard object copy
//--------------------------------------------------------------------------
//
port_number_class& port_number_class::operator=(const port_number_class& source)
{
    port_ = source.port_;
    return *this;
}

//--------------------------------------------------------------------------
// second form - set port number to specified value
//--------------------------------------------------------------------------
//
port_number_class& port_number_class::operator=(const port_number_type port)
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
int port_number_class::operator==(const port_number_class& other) const
{
    return (port_ == other.port_);
}

int port_number_class::operator!=(const port_number_class& other) const
{
    return (port_ != other.port_);
}

//===============================================
//  inspector
//===============================================

//--------------------------------------------------------------------------
// return port number value proper
//--------------------------------------------------------------------------
//
port_number_type port_number_class::port(void) const
{
    return port_;
}

//===============================================
//  facilitators
//===============================================

//--------------------------------------------------------------------------
// extract() - parsing method
//--------------------------------------------------------------------------
//
void port_number_class::extract(istream& is)
{
    // **** instantiate static objects for error messages ****

    static const string my_name = "port_number_class::extract";
    static const string no_port = "couldn't get port number from input stream";

    // **** get port number from stream ****

    if (!(is >> port_)) throw instream_format_exception(is, my_name, no_port);
}

//--------------------------------------------------------------------------
// insert() - unparsing method
//--------------------------------------------------------------------------
//
void port_number_class::insert(ostream& os) const
{
    // **** instantiate static objects for error messages ****

    static const string my_name = "port_number_class::insert";
    static const string bad_out = "couldn't output port number to stream";

    // **** put port number to head of stream ****

    if (!(os << port_))
        throw outstream_io_error_exception(os, my_name, bad_out);
}

//===============================================
//   destructor
//===============================================

port_number_class::~port_number_class(void)
{
}

//=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
//  port_number_class auxiliary operators
//=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*

istream& operator>> (istream& is, port_number_class& target)
{
    target.extract(is);
    return is;
}

ostream& operator<< (ostream& os, const port_number_class& source)
{
    source.insert(os);
    return os;
}

//***************************************************************
//  allocatable_port_number_class - for ports bound by users
//***************************************************************

//=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
//  allocatable_port_number_class proper
//=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*

//================================================
//  constructors
//================================================
//
// first constructor - set port number to specified number
// second constructor - standard copy constructor

allocatable_port_number_class::allocatable_port_number_class(const port_number_type port)
    : port_number_class(port)
{
    // audit value obtained
    //
    throw_exception_if_invalid_port(port, "standard constructor");
}

allocatable_port_number_class::allocatable_port_number_class(const allocatable_port_number_class& source)
    : port_number_class(source)
{
    // audit value obtained
    //
    throw_exception_if_invalid_port(source.port_, "copy constructor");
}

//===============================================
//  assignment operator
//===============================================
//
// first form - standard object copy
// second form - set port number to specified value

allocatable_port_number_class& allocatable_port_number_class::operator=(const allocatable_port_number_class& source)
{
    port_ = source.port_;
    return *this;
}

allocatable_port_number_class& allocatable_port_number_class::operator=(const port_number_type port)
{
    throw_exception_if_invalid_port(port, "operator=");  // audit argument
    port_ = port;
    return *this;
}

//===============================================
//  facilitators
//===============================================

//--------------------------------------------------------------------------
// extract() - parsing method
//--------------------------------------------------------------------------
//
void allocatable_port_number_class::extract(istream& is)
{
    port_number_class::extract(is);                    // get port number
    throw_exception_if_invalid_port(port_, "insert");  // audit value obtained
}

//===============================================
//   destructor
//===============================================

allocatable_port_number_class::~allocatable_port_number_class()
{
}

//===============================================
//   private member functions
//===============================================

//--------------------------------------------------------------------------
// throw_exception_if_invalid_port() -
//       throw exception on bad (i.e., system) port number.
//       use supplied string to identify logic generating exception.
//--------------------------------------------------------------------------
//
void allocatable_port_number_class::throw_exception_if_invalid_port(const port_number_type port, const string& caller_name)  const
{
    // *** initialize static data ***

    static unsigned const LOWEST_NONRESERVED_PORT_NUMBER = 2000;

    static string const my_name_prefix = "allocatable_port_number_class::";
    static string const problem = "port number out of range";

    // *** validate port, and complain if out of range ***

    if (port < LOWEST_NONRESERVED_PORT_NUMBER)
    {
        stringstream p_message;
        p_message << "specified port number (" << port << ") too low for user defined port (min = " << LOWEST_NONRESERVED_PORT_NUMBER << ")";
        throw bad_port_exception(my_name_prefix + caller_name, problem, p_message.str());
    }
}

//=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
//  allocatable_port_number_class auxiliary operators
//=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*

// ==========================================================================
// parsing and unparsing methods
// ==========================================================================

istream& operator>> (istream& is, allocatable_port_number_class& target)
{
    target.extract(is);
    return is;
}

ostream& operator<< (ostream& os, const allocatable_port_number_class& source)
{
    source.insert(os);
    return os;
}

// =*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
// port_number_class exceptions  [boilerplate]
// =*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*

bad_port_exception::bad_port_exception(const string& name, const string& short_message, const string& long_message, const basic_exception& prior_exception)
    : basic_exception(name, short_message, long_message, prior_exception) {}

bad_port_exception::bad_port_exception(const bad_port_exception& source)
    : basic_exception(source) {}

bad_port_exception::~bad_port_exception(void) {}

// *===*====*===*====*===*====*===*====*===*====*===*====*===*====*===*====
// auxiliary operators  [boilerplate]
// *===*====*===*====*===*====*===*====*===*====*===*====*===*====*===*====

ostream& operator<<(ostream& os, const bad_port_exception& ex)
{
    ex.insert(os);
    return os;
}