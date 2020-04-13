//--------------port_number.h----------------------------------------------------------------
//
//  Authors:        Phil Pfeiffer
//  Date:           August 1999
//  Last modified:  April 2020
//
//  Wrapper class that encapsulates sockets API operations on **port numbers**:
//  integers in the range 0..65535 that TCP and UDP associate with service end points
//  for a given host.
//
//  This module defines two notions of a service port:
//  -.   an unsigned value in the range 0..65535 (any port)
//  -.   an unsigned value in the range 2000..65535 (for user-defined services)
//       standard practice:  systems reserve lower-numbered ports for system services
//-------------------------------------------------------------------------------------------

#ifndef PORT_NUMBER_H
#define PORT_NUMBER_H

#include <iostream>
#include <string>

#ifndef PLATFORM_H
#include "platform.h"
#endif

#ifndef STD_H
#include "std.h"
#endif

#ifndef TYPES_H
#include "types.h"    // defines unsigned16
#endif

//***************************************************************
//  port_number_class - for standard range of port numbers
//***************************************************************

//=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
//  port_number_class supporting definitions
//=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*

typedef unsigned16 port_number_type;

//=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
//  port_number_class proper
//=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*

class port_number_class
{
public:
    //================================================
    //  constructors
    //================================================
    //
    // first constructor - set port number to specified number
    // second constructor - standard copy constructor

    port_number_class(const port_number_type = 0);
    port_number_class(const port_number_class&);

    //===============================================
    //  assignment operator
    //===============================================
    //
    // first form - standard object copy
    // second form - set port number to specified value

    port_number_class& operator=(const port_number_class&);
    port_number_class& operator=(const port_number_type);

    // ======================================
    //  other operators
    // ======================================
    //
    //  for equality testing

    int operator==(const port_number_class&) const;
    int operator!=(const port_number_class&) const;

    //===============================================
    //  inspector
    //===============================================
    //
    // return port number value proper

    port_number_type port(void) const;

    //===============================================
    //  facilitators
    //===============================================
    //
    // parsing, unparsing methods

    virtual void extract(istream&);
    void insert(ostream&) const;

    //===============================================
    //   destructor
    //===============================================

    virtual ~port_number_class();

protected:
    //===============================================
    //   member data
    //===============================================

    port_number_type port_;   // the port per se
};

//=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
//  port_number_class auxiliary operators
//=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*

// ==========================================================================
// parsing and unparsing methods
// ==========================================================================

istream& operator>> (istream&, port_number_class&);
ostream& operator<< (ostream&, const port_number_class&);

//***************************************************************
//  allocatable_port_number_class - for user-instantiated ports
//***************************************************************

//=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
//  allocatable_port_number_class proper
//=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*

class allocatable_port_number_class : public port_number_class
{
public:
    //================================================
    //  constructors
    //================================================
    //
    // first constructor - set port number to specified number
    // second constructor - standard copy constructor

    allocatable_port_number_class(const port_number_type = 0);
    allocatable_port_number_class(const allocatable_port_number_class&);

    //===============================================
    //  assignment operator
    //===============================================
    //
    // first form - standard object copy
    // second form - set port number to specified value

    allocatable_port_number_class& operator=(const allocatable_port_number_class&);
    allocatable_port_number_class& operator=(const port_number_type);

    //===============================================
    //  manipulators
    //===============================================
    //
    // parsing method

    void extract(istream&);

    //===============================================
    //   destructor
    //===============================================

    virtual ~allocatable_port_number_class();

private:
    //===============================================
    //   member functions
    //===============================================

    void throw_exception_if_invalid_port(const port_number_type, const string&) const;
};

//=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
//  allocatable_port_number_class auxiliary operators
//=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*

// ==========================================================================
// parsing and unparsing methods
// ==========================================================================

istream& operator>> (istream&, allocatable_port_number_class&);
ostream& operator<< (ostream&, const allocatable_port_number_class&);

//=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
//  port_number_class exceptions
//=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*

#ifndef EXCEPTION_H
#include "exception.h"
#endif

// sole exception--used for improper IP address argument format
//
class bad_port_exception : public basic_exception
{
public:
    bad_port_exception(
        const string & = "", const string & = "", const string & = "",
        const basic_exception & = basic_exception());
    bad_port_exception(const bad_port_exception&);

    virtual ~bad_port_exception(void);
};

// *===*====*===*====*===*====*===*====*===*====*===*====*===*====*===*====
// auxiliary operators
// *===*====*===*====*===*====*===*====*===*====*===*====*===*====*===*====

ostream& operator<<(ostream&, const bad_port_exception&);

#endif
