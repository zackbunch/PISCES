//------------------------------ dns_name.h ---------------------------------
//
//   Authors:  Sridhar Koritala, Phil Pfeiffer
//   Date:     August 1999
//   Last modified:  April 2020
//
//   Capture a Domain System Name, along with related functions,
//   including methods for domain name -> IP address conversion.
//
//---------------------------------------------------------------------------

#ifndef DNS_NAME_H
#define DNS_NAME_H

#include <iostream>
#include <string>

#ifndef PLATFORM_H
#include "platform.h"
#endif

#ifndef STD_H
#include "std.h"
#endif

#ifndef TYPES_H
#include "types.h"         // unsigned32, etc.
#endif

#ifndef DLLINIT_H
#include "dllinit.h"       // self-loading DLL module
#endif

#ifndef EXCEPTION_H
#include "exception.h"
#endif

#ifndef IP_ADDRESS_H
#include "ip_address.h"        // IP address object
#endif

// **************************************************************************
// dns_name_class
// **************************************************************************

// =*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
// dns_name_class proper
// =*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*

// ======================================
//  mode flag values
// ======================================
//
//  flags for controlling interpretation of string denoting IP address
//
//  -.  unknown -   invalid specifier (for catching programmer errors)
//  -.  dns_name -  interpret as dns name (no conversion needed)
//  -.  self -      ignore string; construct object with IP for this host
//
enum class dnsNameType { UNKNOWN, DNS_NAME, SELF };

class dns_name_class
{
public:

    // ======================================
    //  constructors
    // ======================================
    //
    // first form:
    //    store DNS name, throwing exception on failure.
    //    interpretation of string depends on mode parameter:
    //
    // second form:  access DNS name for given IP address
    //
    // third form:   copy constructor

    dns_name_class(const string & = "", const dnsNameType = dnsNameType::DNS_NAME);
    dns_name_class(const ip_address_class&);
    dns_name_class(const dns_name_class&);

    // ======================================
    //  assignment operators
    // ======================================
    //
    // first form:  standard object copy

    dns_name_class& operator=(const dns_name_class&);

    // ======================================
    //  inspectors
    // ======================================
    //
    //  return DNS name as string

    string get_dns_name(void) const;

    // ======================================
    //  facilitators
    // ======================================
    //
    //  parsing and unparsing methods

    void extract(istream & = cin);
    void insert(ostream & = cout) const;

    // ======================================
    //  destructor
    // ======================================
    //
    virtual ~dns_name_class();

private:
    // ============================================================
    // member functions
    // ============================================================
    //
    // -.  get DNS name corresponding to self

    void my_dns_name(const string&);

    // ============================================================
    // member data
    // ============================================================

    //  for ensuring that supporting socket DLL is loaded and unloaded
    //    default (void) constructor =>  load default version of DLL.
    //  make this static since DLL only needs to be loaded once.
    //
    sockets_dll_init_class dll_init_object_;

    //   specified DNS name
    //
    string dns_name_;
};

// =*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
// dns_name_class auxiliary methods
// =*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*

// ==========================================================================
// parsing and unparsing methods
// ==========================================================================

istream& operator>> (istream&, dns_name_class&);
ostream& operator<< (ostream&, const dns_name_class&);

#ifndef DNS_EXCEPTION_H
#include "dns_exception.h"   // DNS exceptions (thrown by class methods on error)
#endif

#endif
