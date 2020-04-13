//------------------------------ ip_address.h ---------------------------------
//
//   Authors:  Sridhar Koritala, Phil Pfeiffer
//   Date:     September 1999
//   Last modified:  April 2020
//
//   Capture an Internet address, along with related operations.
//   Functions include methods for parsing and unparsing addresses,
//   and incrementing and decrementing addresses.
//
//   Design notes:
//       Support for iPv6 (i.e., 16-byte) quantities not fully implemented
//-----------------------------------------------------------------------------

#ifndef IP_ADDRESS_H
#define IP_ADDRESS_H

#include <iostream>
#include <string>
#include <vector>

#ifndef PLATFORM_H
#include "platform.h"
#endif

#ifndef STD_H
#include "std.h"
#endif

#ifndef TYPES_H
#include "types.h"         // unsigned32, etc.
#endif

#ifndef SOCKET_APIS_H
#include "socket_apis.h"         // ip version abstraction code
#endif

// **************************************************************************
// ip_address_class
// **************************************************************************

// =*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
// ip_address_class proper
// =*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*

// ======================================
//  mode flag values
// ======================================
//
//  flags for controlling interpretation of string denoting IP address
//
//  -.  UNKNOWN -   invalid specifier (for catching programmer errors)
//  -.  IP_ADDRESS_STRING - interpret as adr of form "nnn.nnn.nnn.nnn"
//  -.  DNS_NAME -  interpret as dns name of form "domain1.domain2. ... .domainn"
//  -.  SELF -      ignore string; construct object with IP for this host
//
enum class ipAddressType { UNKNOWN, IP_ADDRESS_STRING, DNS_NAME, SELF };

class ip_address_class
{
public:
    // ======================================
    //  constructors
    // ======================================
    //
    // first form:
    //    instantiate object using four-byte-long vector
    //
    // second form:
    //    convert string to IP address, throwing exception on failure.
    //    interpretation of string depends on mode parameter
    //
    // third form:   copy constructor
    //
    // fourth form:
    //     accept a socket_address structure that is defined
    //     as either a sockaddr_in or sock_addr_in6 depending on version used

    ip_address_class(const ip_address_vector_type & = ip_address_vector_type(get_ip_address_length()));
    ip_address_class(const string&, const ipAddressType = ipAddressType::IP_ADDRESS_STRING);
    ip_address_class(const ip_address_class&);
    ip_address_class(const socket_address&);

    // ======================================
    //  assignment operators
    // ======================================
    //
    // first form:  standard object copy
    // second form: assign value specified by pair to

    ip_address_class& operator=(const ip_address_class&);

    // ======================================
    //  inspectors
    // ======================================

    // return object IP address as n-byte vector
    // return object IP address as formatted string

    ip_address_vector_type ip_address(void) const;
    string ip_address_as_string(void) const;

    // ======================================
    //  operators
    // ======================================
    //
    //  *** unary arithmetic operators ***

    ip_address_class operator ++(void);  // prefix ++
    ip_address_class operator ++(int);   // postfix ++
    ip_address_class operator --(void);  // prefix --
    ip_address_class operator --(int);   // postfix --

    //  *** relational operators ***
    //
    bool operator < (const ip_address_class&) const;
    bool operator <=(const ip_address_class&) const;
    bool operator ==(const ip_address_class&) const;
    bool operator !=(const ip_address_class&) const;
    bool operator >=(const ip_address_class&) const;
    bool operator > (const ip_address_class&) const;

    // ======================================
    //  manipulators
    // ======================================
    //
    //  parsing and unparsing methods

    void extract(istream & = cin);
    void insert(ostream & = cout) const;

    // ======================================
    //  destructor
    // ======================================
    //
    virtual ~ip_address_class();

private:
    // ============================================================
    // member functions
    // ============================================================

    // **** IP address conversions ****
    //
    // -.  get IP address at stream head for string in form "w.x.y.z"
    // -.  get IP address corresponding to specified DNS name
    // -.  get IP address corresponding to self
    //
    // final parameter for each fn is name of caller (for diagnostics)

    void ip_address_string_to_ip_address(istream&, const string&);
    void dns_string_to_ip_address(const string&, const string&);
    void my_ip_address(const string&);

    // **** IP address comparison ****

    // for implementing overloaded ==, <, >, <=. >=, !=
    //
    // returns -1, 0, or 1, according to whether *this is >, ==, or < arg.
    // comparisons done on byte-by-byte basis, from left to right in IP adr.

    int compare(const ip_address_class&) const;

    // ============================================================
    // member data
    // ============================================================

    //   IP address, DNS name corresponding to specified host
    //
    ip_address_vector_type ip_address_;
};

// =*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
// ip_address_class auxiliary methods
// =*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*

// ==========================================================================
// parsing and unparsing methods
// ==========================================================================

istream& operator>> (istream&, ip_address_class&);
ostream& operator<< (ostream&, const ip_address_class&);

#ifndef DNS_EXCEPTION_H
#include "dns_exception.h"  //DNS exceptions (thrown by class methods on error)
#endif

#endif
