//------------------------------ ip_address.cpp --------------------------------
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
//------------------------------------------------------------------------------

#include "ip_address.h"

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
// ip_address_class
// **************************************************************************

// =*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
// ip_address_class proper
// =*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*

// ======================================
//  constructors
// ======================================

//-----------------------------------------------------------------------------
// constructor, first form:
//    instantiate object using n-byte-long vector
//-----------------------------------------------------------------------------
//
ip_address_class::ip_address_class(const ip_address_vector_type& source_vector)
    : ip_address_(source_vector)
{
    // **** instantiate static objects for error messages ****

    static const string my_name = "ip_address_class::constructor";
    static const string bad_vector = "improperly formatted vector";

    // **** check if vector has right number of entries ****

    if (source_vector.size() != get_ip_address_length())
    {
        stringstream p_message;
        p_message << "vector has " << source_vector.size() << " fields, " << "should have " << get_ip_address_length();
        throw ip_address_format_exception(my_name, bad_vector, p_message.str());
    }
}

//-----------------------------------------------------------------------------
// constructor, second form:
//    convert string to IP address, throwing exception on failure
//
//  flags for controlling interpretation of string denoting IP address
//
//  -.  UNKNOWN -   invalid specifier (for catching programmer errors)
//  -.  IP_ADDRESS_STRING - interpret as adr in string format
//  -.  DNS_NAME -  interpret as dns name of form "
//                                      domain1.domain2. ... .domainn"
//  -.  SELF -      ignore string; construct object with IP for this host
//
//-----------------------------------------------------------------------------
//
ip_address_class::ip_address_class(const string& source_string, const ipAddressType output_format)
{
    // **** instantiate static objects for error messages ****

    static const string my_name = "ip_address_class::constructor";
    static const string bad_arg = "bad specifier argument for ip address mode type";
    static const string unknown = "value is unknown";

    // **** parse string if we can--check if format is sound ****

    // need n slots in vector for parse
    ip_address_.resize(get_ip_address_length());

    stringstream p_stream;  // for converting input string to stream and building diagnostic message

    switch (output_format)
    {        // copy string to stringstream for processing by subr
    case ipAddressType::IP_ADDRESS_STRING:  p_stream << source_string;
        p_stream.seekg(ios::beg);
        ip_address_string_to_ip_address(p_stream, my_name);   // do the parse
        break;

    case ipAddressType::DNS_NAME:           dns_string_to_ip_address(source_string, my_name);
        break;

    case ipAddressType::SELF:               my_ip_address(my_name);
        break;

    default:                                throw basic_exception(my_name, bad_arg, unknown);
    }
}

//-----------------------------------------------------------------------------
// constructor, third form:   copy constructor
//-----------------------------------------------------------------------------
//
ip_address_class::ip_address_class(const ip_address_class& source)
    : ip_address_(source.ip_address_)
{
}

//-----------------------------------------------------------------------------
// constructor, fourth form:  accept a socket_address structure that is defined
//    as either a sockaddr_in or sock_addr_in6 depending on version used
//-----------------------------------------------------------------------------
//
ip_address_class::ip_address_class(const socket_address& source_address)
{
    // **** instantiate static objects for error messages ****
    static const string my_name = "ip_address_class::constructor";

    // **** parse string if we can--check if format is sound ****

    // need n slots in vector for parse
    ip_address_.resize(get_ip_address_length());

    // for converting input string to stream also, for building
    // diagnostic message
    stringstream p_stream;

    //dummy param for IPv4, needed for IPv6

    char* buffer = new char[get_address_string_length()];

    p_stream << socket_address_numerical_to_string(source_address, buffer);
    p_stream.seekg(ios::beg);    // (0, ios::beg) ??

    // do the parse
    ip_address_string_to_ip_address(p_stream, my_name);

    delete[] buffer;
}

// ======================================
//  assignment operators
// ======================================

//-----------------------------------------------------------------------------
// first form:  standard object copy
//-----------------------------------------------------------------------------
//
ip_address_class& ip_address_class::operator=(const ip_address_class& source)
{
    ip_address_ = source.ip_address_;
    return *this;
}

// ======================================
//  inspectors
// ======================================

//-----------------------------------------------------------------------------
// ip_address() - return object's IP address as four-byte vector
//-----------------------------------------------------------------------------
//
ip_address_vector_type ip_address_class::ip_address(void) const
{
    return ip_address_;
}

//-----------------------------------------------------------------------------
// IpAddressAsString() - return object's IP address as a string
//-----------------------------------------------------------------------------
//
string ip_address_class::ip_address_as_string(void) const
{
    stringstream p_temp;
    p_temp << *this;
    return string(p_temp.str());
}

//==============================
//   operators
//==============================

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//   unary arithmetic
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

//---------------------------------------------------------------
//  prefix operator ++
//      for simplicity, 255.255.255.255 wraps to 0.0.0.0
//---------------------------------------------------------------
//
ip_address_class ip_address_class::operator ++ (void)
{
    for (int i = get_ip_address_length() - 1; i >= 0; --i)
    {
        if (ip_address_[i] != get_max_field_value())
        {
            ip_address_[i]++;
            break;
        }
        ip_address_[i] = 0;
    }
    return *this;
}

//------------------------------------------------------------------------------------
//  postfix operator ++
//     note: 2nd argument is a dummy argument, that distinguishes
//     postfix ++ from prefix ++
//
//     for simplicity, 255.255.255.255 wraps to 0.0.0.0
//------------------------------------------------------------------------------------
//
ip_address_class ip_address_class::operator ++ (int)
{
    ip_address_class result = *this;
    for (int i = get_ip_address_length() - 1; i >= 0; --i)
    {
        if (ip_address_[i] != get_max_field_value())
        {
            ip_address_[i]++;
            break;
        }
        ip_address_[i] = 0;
    }
    return result;
}

//---------------------------------------------------------------
//  prefix operator --
//      for simplicity, 0.0.0.0 wraps to 255.255.255.255
//---------------------------------------------------------------
//
ip_address_class ip_address_class::operator -- (void)
{
    for (int i = get_ip_address_length() - 1; i >= 0; --i)
    {
        if (ip_address_[i] != 0)
        {
            ip_address_[i]--;
            break;
        }
        ip_address_[i] = get_max_field_value();
    }
    return *this;
}

//---------------------------------------------------------------
//  postfix operator --
//     note: 2nd argument is a dummy argument, that distinguishes
//     postfix -- from prefix --
//---------------------------------------------------------------
//
ip_address_class ip_address_class::operator -- (int)
{
    ip_address_class result = *this;

    for (int i = get_ip_address_length() - 1; i >= 0; --i)
    {
        if (ip_address_[i] != 0)
        {
            ip_address_[i]--;
            break;
        }
        ip_address_[i] = get_max_field_value();
    }

    return result;
}

//=---=---=---=---=---=---=---=---=---=---=---=-
//   relational
//=---=---=---=---=---=---=---=---=---=---=---=-

//-------------------------------------------------------------------
//  operator <
//-------------------------------------------------------------------
bool ip_address_class::operator < (const ip_address_class& right_operand) const
{
    return (compare(right_operand) < 0);
}

//-------------------------------------------------------------------
//  operator <=
//-------------------------------------------------------------------
bool ip_address_class::operator <= (const ip_address_class& right_operand) const
{
    return (compare(right_operand) <= 0);
}

//-------------------------------------------------------------------
//  operator !=
//-------------------------------------------------------------------
bool ip_address_class::operator != (const ip_address_class& right_operand) const
{
    return (compare(right_operand) != 0);
}

//-------------------------------------------------------------------
//  operator ==
//-------------------------------------------------------------------
bool ip_address_class::operator == (const ip_address_class& right_operand) const
{
    return (compare(right_operand) == 0);
}

//-------------------------------------------------------------------
//  operator >=
//-------------------------------------------------------------------
bool ip_address_class::operator >= (const ip_address_class& right_operand) const
{
    return (compare(right_operand) >= 0);
}

//-------------------------------------------------------------------
//  operator >
//-------------------------------------------------------------------
bool ip_address_class::operator > (const ip_address_class& right_operand) const
{
    return (compare(right_operand) > 0);
}

// ======================================
//  manipulators
// ======================================

//------------------------------------------------------------------------------
//  extract() - look for "num.num.num.num" at head of stream for IPv4
//                or IPv6 formatted string for IPv6
//------------------------------------------------------------------------------
void ip_address_class::extract(istream& is)
{
    // **** instantiate static objects for error messages ****
    static const string my_name = "ip_address_class::extract";

    // **** scan head of stream, converting string  ****
    ip_address_string_to_ip_address(is, my_name);
}

//------------------------------------------------------------------------------
//  insert() - write contents of object to stream, as string
//------------------------------------------------------------------------------
void ip_address_class::insert(ostream& os) const
{
    // **** instantiate static objects for error messages ****
    static const string my_name = "ip_address_class::insert";
    static const string bad_out = "couldn't output IP adr to stream";

    //code here is specific to IP version, so it is in the isolation layer

    if (!ip_address_insert(os, ip_address_))
        throw outstream_io_error_exception(os, my_name, bad_out);

    //still need hex mode code for IPv6, is this better to leave in ioslation layer?
    //old
    /*for(int i = 0; i < get_ip_address_length(); ++i)
    {
        if( ! (os << ip_address_[i] << get_ip_address_separator()))
            throw outstream_io_error_exception( os, my_name, bad_out );
    }*/
}

// ======================================
//  destructor
// ======================================
ip_address_class::~ip_address_class(void)
{
}

//=================================
//   private methods
//=================================

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//   IP address translation
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

//----------------------------------------------------------------------------
//  ip_address_string_to_ip_address() -
//       get IP address for string from head of specified stream;
//       look for address of form "w.x.y.z"
//----------------------------------------------------------------------------
//
void ip_address_class::ip_address_string_to_ip_address(istream& is, const string& caller_name)    //FIXME: still coding in iso. layer
{
    // **** instantiate static objects for error messages ****
    static const string bad_ip = "improperly formatted IP address";

    // **** look for required string at head of specified stream ****
    ip_address_ = convert_ip_address_string(is);
}

//----------------------------------------------------------------------------
//  dns_string_to_ip_address() - get IP address corresponding to specified DNS name
//----------------------------------------------------------------------------
//
void ip_address_class::dns_string_to_ip_address(const string& dns_name, const string& caller_name)
{
    // **** **** define static data for managing routine **** ****
    static const string no_ip = "could not get IP address";

    //FIXME: gethostbyname2
    //Issue: gethostbyname2 can return IPv4 or IPv6 addresses
    //however, if the user has IPv4 only socket code, then this will not exist
    //solution: use gethostbyname for IPv4 and gethostbyname2 for IPv6

    //  **** **** use hostname to get data on specified host **** ****
    struct hostent* p_hostent_buffer = get_host_by_name(dns_name.c_str());

    if (p_hostent_buffer == NULL)
    {
        stringstream p_message;
        p_message << "could not access DNS entry for host " << dns_name << ":" << endl << socket_error_message_class("gethostbyname").get_message();
        throw missing_ip_address_exception(caller_name, no_ip, p_message.str());
    }

    //  **** **** insert IP address from hostent structure **** ****
    //
    //  first cast is from type char * to type unsigned char *

    unsigned char* required_address = reinterpret_cast<unsigned char*>(p_hostent_buffer->h_addr_list[0]);

    for (unsigned i = 0; i < get_ip_address_length(); i++)
        ip_address_[i] = required_address[i];
}

//----------------------------------------------------------------------------
//  my_ip_address() - get IP address corresponding to self
//----------------------------------------------------------------------------
//

void ip_address_class::my_ip_address(const string& caller_name)
{
    // **** **** define static data for managing routine **** ****
    //
    // ==== buffer parameters for accessing DNS ====

    //FIXME: checked, MS->254, RFC-> 255, DNS max storage = 253 (???)
    //255 includes the trailing null
    static const unsigned HOSTNAME_BUFFER_LENGTH = 254;      // allow for long host names

    // ==== error message strings ====
    //
    static const string no_name = "could not get name of current host";

    //  ==== first:  use gethostname() socket call to get host's DNS name
    char hostname_buffer[HOSTNAME_BUFFER_LENGTH];

    if (gethostname(hostname_buffer, HOSTNAME_BUFFER_LENGTH) == SOCKET_ERROR)    //FIXME? no?
        throw missing_hostname_exception(caller_name, no_name, socket_error_message_class("gethostname").get_message());

    //  ==== then:  use hostname to get data on current host ====
    dns_string_to_ip_address(hostname_buffer, caller_name);
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//   IP address comparison
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

//-----------------------------------------------------------------------------------------------------------
//  compare - check relative values of *this and right_operand,
//            returning -1/ 0/ 1 depending on result
//
//  comparison proceeds from most significant to least significant fields  (i.e., from left to right)
//-----------------------------------------------------------------------------------------------------------
//
int ip_address_class::compare(const ip_address_class& right_operand) const
{
    for (unsigned i = 0; i < get_ip_address_length(); ++i)
    {
        if (ip_address_[i] < right_operand.ip_address_[i]) return -1;
        if (ip_address_[i] > right_operand.ip_address_[i]) return  1;
    }
    return 0;
}

// =*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
// ip_address_class auxiliary operators
// =*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
istream& operator>> (istream& is, ip_address_class& target)
{
    target.extract(is);
    return is;
}

ostream& operator<< (ostream& os, const ip_address_class& source)
{
    source.insert(os);
    return os;
}