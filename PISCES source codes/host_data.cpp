//------------------------------ host_data.cpp --------------------
//
//   Authors:  Phil Pfeiffer
//   Date:     September 1999
//   Last modified:  April 2020
//
//   Capture IP address, dns name for specified host.
//------------------------------------------------------------------

#include "host_data.h"

#ifndef BOOL_H
#include "bool.h"
#endif

#include <sstream>   // stringstream

#ifndef EXCEPTION_H
#include "exception.h"   // general exception class
#endif

#ifndef DNS_EXCEPTION_H
#include "dns_exception.h"      // DNS name exceptions class
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
//    instantiate object with specified ipHost object
//------------------------------------------------------------------------
//
host_data_class::host_data_class(const ip_address_class& ip_address)
    : ip_address_(ip_address)
{
    try
    {
        dns_name_ = dns_name_class(ip_address);
    }
    catch (const missing_hostname_exception&)
    {
        dns_name_ = dns_name_class("<name not obtainable>");
    }
}

//------------------------------------------------------------------------
// second form:
//    instantiate object with specified port, ip_address object.
//------------------------------------------------------------------------
//
host_data_class::host_data_class(const dns_name_class& dns_name)
    : ip_address_(dns_name.get_dns_name(), ipAddressType::DNS_NAME),
    dns_name_(dns_name)
{
}

//------------------------------------------------------------------------
// third form:   copy constructor
//------------------------------------------------------------------------
//
host_data_class::host_data_class(const host_data_class& source)
    : ip_address_(source.ip_address_),
    dns_name_(source.dns_name_)
{
}

//========================================================================
//  assignment operators
//========================================================================

//------------------------------------------------------------------------
// first form - standard object copy
//------------------------------------------------------------------------
//
host_data_class& host_data_class::operator=(const host_data_class& source)
{
    ip_address_ = source.ip_address_;
    dns_name_ = source.dns_name_;
    return *this;
}

// ======================================
//  manipulators
// ======================================

//------------------------------------------------------------------------------
//  insert() - output host name, IP address to specified stream
//------------------------------------------------------------------------------

void host_data_class::insert(ostream& os) const
{
    // **** instantiate static objects for error messages ****

    static const string my_name = "ip_address_class::insert";
    static const string bad_out = "couldn't output DNS name, IP address to stream";

    // **** put host address, port to head of stream ****
    try
    {
        if (!(os << "host name = "))
        {
            throw outstream_io_error_exception(os, my_name, bad_out);
        }
        os << dns_name_;

        if (!(os << ", ip address = "))
        {
            throw outstream_io_error_exception(os, my_name, bad_out);
        }
        os << ip_address_;
    }
    catch (const basic_exception& message)
    {
        throw outstream_io_error_exception(os, my_name, bad_out, "", message);
    }
}

//========================================================================
//  destructor
//========================================================================
//
host_data_class::~host_data_class(void)
{
}

// =*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
// host_data_class auxiliary operators
// =*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*

ostream& operator<<(ostream& os, const host_data_class& source)
{
    source.insert(os);
    return os;
}