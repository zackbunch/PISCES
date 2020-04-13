// ---------dns_exception.h-----------------------------------------------
//
//  Author:  Phil Pfeiffer
//  Date:    September 1999
//  Last modified:  April 2020
//
//  Classes that define DNS API call exceptions.
//  Key exceptions defined:
//
//  -.  dns_exception:                 overall, base exception
//  -.  missing_ip_address_exception:  can't get required IP address
//  -.  missing_hostname_exception:    can't get hostname for IP address
//  -.  ip_address_format_exception:   can't get hostname for IP address
// ------------------------------------------------------------------------

#ifndef DNS_EXCEPTION_H
#define DNS_EXCEPTION_H

#include <string>

#ifndef PLATFORM_H
#include "platform.h"
#endif

#ifndef STD_H
#include "std.h"
#endif

#ifndef EXCEPTION_H
#include "exception.h"   // exception management
#endif

// ****************************************************
// classes that define DNS error conditions
// ****************************************************
//
// create related exceptions in this module:
// -.  an overall class to serve as base class for all exceptions
// -.  a set of specific DNS exceptions

// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=
// zero-order DNS exception: dns_exception
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

class dns_exception : public basic_exception
{
public:
    dns_exception(
        const string & = "", const string & = "", const string & = "",
        const basic_exception & = basic_exception());
    dns_exception(const dns_exception&);

    virtual ~dns_exception(void);
};

// *===*====*===*====*===*====*===*====*===*====*===*====*===*====*===*====
// auxiliary operators
// *===*====*===*====*===*====*===*====*===*====*===*====*===*====*===*====

ostream& operator<<(ostream&, const dns_exception&);

// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=
// first-order stream exceptions:
//
//  -.  missing_ip_address_exception:  can't get required IP address
//  -.  missing_hostname_exception:   can't get hostname for IP address
//  -.  ip_address_format_exception:   can't get hostname for IP address
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

class missing_ip_address_exception : public dns_exception
{
public:
    missing_ip_address_exception(
        const string & = "", const string & = "", const string & = "",
        const basic_exception & = basic_exception());
    missing_ip_address_exception(const missing_ip_address_exception&);

    virtual ~missing_ip_address_exception(void);
};

class missing_hostname_exception : public dns_exception
{
public:
    missing_hostname_exception(
        const string & = "", const string & = "", const string & = "",
        const basic_exception & = basic_exception());
    missing_hostname_exception(const missing_hostname_exception&);

    virtual ~missing_hostname_exception(void);
};

class ip_address_format_exception : public dns_exception
{
public:
    ip_address_format_exception(
        const string & = "", const string & = "", const string & = "",
        const basic_exception & = basic_exception());
    ip_address_format_exception(const ip_address_format_exception&);

    virtual ~ip_address_format_exception(void);
};

// *===*====*===*====*===*====*===*====*===*====*===*====*===*====*===*====
// auxiliary operators
// *===*====*===*====*===*====*===*====*===*====*===*====*===*====*===*====

ostream& operator<<(ostream&, const missing_ip_address_exception&);
ostream& operator<<(ostream&, const missing_hostname_exception&);
ostream& operator<<(ostream&, const ip_address_format_exception&);

#endif
