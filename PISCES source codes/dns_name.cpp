//------------------------------ dns_name.cpp ---------------------------------
//
//   Authors:  Sridhar Koritala, Phil Pfeiffer
//   Date:     August 1999
//   Last modified:  April 2020
//
//   Capture a Domain System Name, along with related functions,
//   including methods for domain name -> IP address conversion.
//-----------------------------------------------------------------------------

#include "dns_name.h"

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
// dns_name Class
// **************************************************************************

// =*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
// dns_name Class proper
// =*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*

//========================================================================
//  constructors
//========================================================================

//------------------------------------------------------------------------
// constructor, first form -
//
//  store DNS name, throwing exception on failure.
//  interpretation of string depends on mode parameter:
//  -.  unknown -   invalid specifier (for catching programmer errors)
//  -.  dns_name -  interpret as standard DNS name (no conversion done)
//  -.  self -      ignore string; construct object with IP for this host
//
//------------------------------------------------------------------------
//
dns_name_class::dns_name_class(const string& source_string, const dnsNameType name_mode)
{
    // **** instantiate static objects for error messages ****

    static const string my_name = "dns_name_class::constructor";
    static const string bad_arg = "bad specifier argument for dns string type";
    static const string unknown = "mode is unknown";

    // **** parse string if we can--check if format is sound ****

    stringstream p_stream;
    switch (name_mode)
    {
    case dnsNameType::DNS_NAME:  dns_name_ = source_string;
        break;

    case dnsNameType::SELF:      my_dns_name(my_name);
        break;

    default:                     p_stream << "unknown mode";
        throw basic_exception(my_name, bad_arg, unknown);
    }
}

//------------------------------------------------------------------------
// constructor, second form -
//    instantiate obj. with DNS name for specified IP address,
//    throwing exception on failure.
//------------------------------------------------------------------------
//
dns_name_class::dns_name_class(const ip_address_class& ip_address)
{
    // **** **** define static data for managing routine **** ****

    // ==== error message strings ====

    static const string my_name = "dns_name_class::constructor";
    static const string no_name = "could not get DNS name";

    // ==== convert IP address to form accepted by gethostbyaddr() ====
    struct in_addr query_ip_addr;
    query_ip_addr.s_addr = inet_addr(ip_address.ip_address_as_string().c_str());

    // ==== obtain DNS name for specified host ====
    // here, "AF_INET" signifies Internet protocol suite (i.e., IP) address

    const unsigned SIZEOF_IP_ADDRESS_STRUCT = 4;
    struct hostent* p_hostent_buffer = gethostbyaddr(reinterpret_cast<char*>(&query_ip_addr), SIZEOF_IP_ADDRESS_STRUCT, AF_INET);

    if (p_hostent_buffer == NULL)
        throw missing_hostname_exception(
            my_name,
            no_name + " for host " + ip_address.ip_address_as_string(),
            socket_error_message_class("gethostbyaddr").get_message()
        );

    dns_name_ = p_hostent_buffer->h_name;
}

//------------------------------------------------------------------------
// third form:   copy constructor
//------------------------------------------------------------------------
//
dns_name_class::dns_name_class(const dns_name_class& source)
    : dns_name_(source.dns_name_)
{
}

//========================================================================
//  assignment operators
//========================================================================

//------------------------------------------------------------------------
// standard object copy
//------------------------------------------------------------------------
//
dns_name_class& dns_name_class::operator=(const dns_name_class& source)
{
    dns_name_ = source.dns_name_;
    return *this;
}

// ======================================
//  inspectors
// ======================================

//------------------------------------------------------------------------------
//  get_dns_name() -return DNS name as string
//------------------------------------------------------------------------------
//
string dns_name_class::get_dns_name(void) const
{
    return dns_name_;
}

// ======================================
//  facilitators
// ======================================

//------------------------------------------------------------------------------
//  extract() -
//     try to initialize dns name, from identifier at head of stream
//------------------------------------------------------------------------------

void dns_name_class::extract(istream& is)
{
    static const string my_name = "dns_name_class::extract";
    static const string bad_string = "improperly formatted input stream";
    static const string no_data = "can't get string from head of stream";

    // **** get string at head of input stream  ****

    if (!(is >> dns_name_)) throw instream_format_exception(is, my_name, bad_string, no_data);
}

//------------------------------------------------------------------------------
//  insert() - write contents of object to stream, as string
//------------------------------------------------------------------------------

void dns_name_class::insert(ostream& os) const
{
    static const string my_name = "dns_name_class::insert";
    static const string bad_out = "couldn't output host data to stream";

    // *** unsigned char variables try to insert as chars.
    // *** must cast to ensure that values print as unsigneds.

    if (!(os << dns_name_)) throw outstream_io_error_exception(os, my_name, bad_out);
}

//========================================================================
//  destructor
//========================================================================

dns_name_class::~dns_name_class(void)
{
}

//========================================================================
//  private methods
//========================================================================

//----------------------------------------------------------------------------
//  my_dns_name() - get DNS name corresponding to self
//----------------------------------------------------------------------------
//
void dns_name_class::my_dns_name(const string& caller_name)
{
    // ==== buffer parameters for accessing DNS ====
    static const unsigned HOSTNAME_BUFFER_LENGTH = 254;   // allow for long host names

    // ==== error message strings ====
    static const string no_name = "could not get name of current host";

    //  ==== first:  use gethostname() socket call to get host's DNS name

    char hostname_buffer[HOSTNAME_BUFFER_LENGTH];
    if (gethostname(hostname_buffer, HOSTNAME_BUFFER_LENGTH) == SOCKET_ERROR)
        throw missing_hostname_exception(caller_name, no_name, socket_error_message_class("gethostname").get_message());

    //  ==== then:  store name obtained ====
    dns_name_ = hostname_buffer;
}

// =*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
// dns_name_class auxiliary operators
// =*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*

istream& operator>> (istream& is, dns_name_class& target)
{
    target.extract(is);
    return is;
}

ostream& operator<< (ostream& os, const dns_name_class& source)
{
    source.insert(os);
    return os;
}