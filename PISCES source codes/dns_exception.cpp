// -------dns_exception.cpp----------------------------------------------------------
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
// ----------------------------------------------------------------------------------

#include "dns_exception.h"

// =*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
// Domain Name System exceptions  [boilerplate]
// =*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*

dns_exception::dns_exception(
    const string& name, const string& short_message, const string& long_message,
    const basic_exception& prior_exception)
    : basic_exception(name, short_message, long_message, prior_exception) {}

dns_exception::dns_exception(const dns_exception& source)
    : basic_exception(source) {}

dns_exception::~dns_exception(void) {}

missing_ip_address_exception::missing_ip_address_exception(
    const string& name, const string& short_message, const string& long_message,
    const basic_exception& prior_exception)
    : dns_exception(name, short_message, long_message, prior_exception) {}

missing_ip_address_exception::missing_ip_address_exception(const missing_ip_address_exception& source)
    : dns_exception(source) {}

missing_ip_address_exception::~missing_ip_address_exception(void) {}

missing_hostname_exception::missing_hostname_exception(
    const string& name, const string& short_message, const string& long_message,
    const basic_exception& prior_exception)
    : dns_exception(name, short_message, long_message, prior_exception) {}

missing_hostname_exception::missing_hostname_exception(const missing_hostname_exception& source)
    : dns_exception(source) {}

missing_hostname_exception::~missing_hostname_exception(void) {}

ip_address_format_exception::ip_address_format_exception(
    const string& name, const string& short_message, const string& long_message,
    const basic_exception& prior_exception)
    : dns_exception(name, short_message, long_message, prior_exception) {}

ip_address_format_exception::ip_address_format_exception(const ip_address_format_exception& source)
    : dns_exception(source) {}

ip_address_format_exception::~ip_address_format_exception(void) {}

// *===*====*===*====*===*====*===*====*===*====*===*====*===*====*===*====
// auxiliary operators  [boilerplate]
// *===*====*===*====*===*====*===*====*===*====*===*====*===*====*===*====

ostream& operator<<(ostream& os, const dns_exception& ex)
{
    ex.insert(os);
    return os;
}

ostream& operator<<(ostream& os, const missing_ip_address_exception& ex)
{
    ex.insert(os);
    return os;
}

ostream& operator<<(ostream& os, const missing_hostname_exception& ex)
{
    ex.insert(os);
    return os;
}

ostream& operator<<(ostream& os, const ip_address_format_exception& ex)
{
    ex.insert(os);
    return os;
}