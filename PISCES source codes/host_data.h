//------------------------------ host_data.h ------------------
//
//   Authors:  Phil Pfeiffer
//   Date:     September 1999
//   Last modified:  April 2020
//
//   Capture IP address, dns name for specified host.
//--------------------------------------------------------------

#ifndef HOST_DATA_H
#define HOST_DATA_H

#include <iostream>
#include <string>

#ifndef PLATFORM_H
#include "platform.h"
#endif

#ifndef STD_H
#include "std.h"
#endif

#ifndef IP_ADDRESS_H
#include "ip_address.h"         // IP address object
#endif

#ifndef DNS_NAME_H
#include "dns_name.h"       // DNS name object
#endif

// **************************************************************************
// host_data_class object
// **************************************************************************

// =*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
// host_data_class proper
// =*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*

class host_data_class
{
public:
    // ======================================
    //  constructors
    // ======================================
    //
    // first form:
    //    instantiate object with specified ip_address object, port.
    //    address defaults to current host.
    //
    // second form:
    //    instantiate object with specified DNS name.
    //
    // third form:   copy constructor

    host_data_class(const ip_address_class & = ip_address_class("", ipAddressType::SELF));
    host_data_class(const dns_name_class&);
    host_data_class(const host_data_class&);

    // ======================================
    //  assignment operators
    // ======================================
    //
    // first form:   standard object copy

    host_data_class& operator=(const host_data_class&);

    // ======================================
    //  manipulators
    // ======================================
    //
    //  unparsing method  (key use of class: reporting host info)

    void insert(ostream & = cout) const;

    // ======================================
    //  destructor
    // ======================================
    //
    virtual ~host_data_class();

private:
    // ============================================================
    // member data
    // ============================================================

    //   IP address, DNS name corresponding to specified host
    //
    ip_address_class   ip_address_;
    dns_name_class     dns_name_;
};

// =*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
// socket_service_access_point_class auxiliary methods
// =*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*

ostream& operator<<(ostream&, const host_data_class&);

#endif
