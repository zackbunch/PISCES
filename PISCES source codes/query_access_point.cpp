//--------------------query_access_point.cpp------------------------------------------
//
//  Author:         Phil Pfeiffer
//  Date:           September 1999
//  Last modified:  April 2020
//
//  Query user for a service access point for socket communications,
//  via a pair of question/response streams.
//
//  Methods:
//   -.  get_allocatable_access_point( )-
//          repeatedly
//              query user for service access point (port)
//              until valid service access point returned
//
//   -.  get_access_point( ) -
//          repeatedly
//              query user for service access point (host/port)
//              until valid service access point returned
//
//  Design note:
//     Basic methods for interacting with users inherited from query class.
//------------------------------------------------------------------------------------

#include "query_access_point.h"

#ifndef INTERVAL_TPL
#include "interval.tpl"
#endif

#ifndef BOOL_H
#include "bool.h"
#endif

//=======================================
//  static member data
//=======================================

unsigned const socket_service_access_point_query_class::MIN_PORT_NUMBER_ = 1;
unsigned const socket_service_access_point_query_class::MAX_PORT_NUMBER_ = 65535;
unsigned const socket_service_access_point_query_class::MIN_ALLOCATABLE_PORT_NUMBER_ = 2000;
unsigned const socket_service_access_point_query_class::MAX_ALLOCATABLE_PORT_NUMBER_ = 65535;

//=======================================
//  constructors
//=======================================

//---------------------------------------------------------------------
//  first  form-- sets istream, ostream to user-specified streams
//---------------------------------------------------------------------
//
socket_service_access_point_query_class::socket_service_access_point_query_class(
    istream& response_stream, ostream& query_stream, ostream& feedback_stream
)
    : query_class(response_stream, query_stream, feedback_stream)
{
}

//---------------------------------------------------------------------
//  second form-- copy constructor
//---------------------------------------------------------------------
//
socket_service_access_point_query_class::socket_service_access_point_query_class(
    socket_service_access_point_query_class& source
)
    : query_class(source)
{
}

//===============================================
//   manipulators
//===============================================

// ----------------------------------------------------------------
// get_allocatable_access_point() -
//
// get from user service access point object consisting of local IP,
// user-allocatable port
// ----------------------------------------------------------------
//
void socket_service_access_point_query_class::get_allocatable_access_point(socket_service_access_point_class& access_point)
{
    // **** get components of indicated access point ****

    // the only allocatable ports, as a rule, are on the local host.
    // have seen exceptions in an NFS environment, but will ignore here.

    ip_address_class ip_address("", ipAddressType::SELF);
    allocatable_port_number_class  port_object = get_allocatable_port();

    // **** combine, return as service access point object ****

    access_point = socket_service_access_point_class(ip_address, port_object);
}

// ----------------------------------------------------------------
// getRemoteAccessPoint() -
//     repeatedly
//         query user for access point for server computer,
//         attempt to establish connection,
//     until connection established, or user gives up
// ----------------------------------------------------------------
//
void socket_service_access_point_query_class::get_access_point(socket_service_access_point_class& access_point)
{
    // **** get components of indicated access point ****

    ip_address_class   ip_address = get_host_ip();
    port_number_class  port_object = get_port();

    // **** combine, return as service access point object ****
    access_point = socket_service_access_point_class(ip_address, port_object);
}

//===============================================
//  destructor
//===============================================

socket_service_access_point_query_class::~socket_service_access_point_query_class(void)
{
}

//===============================================
//   private member functions
//===============================================

//------------------------------------------------------------------
// get_host_ip() - get host IP component of service access point
//------------------------------------------------------------------
//
ip_address_class socket_service_access_point_query_class::get_host_ip(void)
{
    //  *** set up a few static strings, for dialogue with user ***
    //
    static const string this_machine = "access this machine?";
    static const string which_host = "please enter host to access";

    // **** get host component of server access point ****

    //  === check first if local access desired ===

    if (yes_unless_n(this_machine))
    {
        return ip_address_class("", ipAddressType::SELF);
    }

    //  === otherwise, query until valid IP string obtained ===

    while (TRUE)      // loop exits with returns (IP found)
    {
        string server_string;
        get_string(which_host, server_string);

        //  first assume string is "num.num.num.num"
        try
        {
            try
            {
                return ip_address_class(server_string);
            }
            catch (const ip_address_format_exception&) {}

            // if string not "num.num.num.num", assume DNS name
            return ip_address_class(server_string, ipAddressType::DNS_NAME);
        }
        catch (const dns_exception& message)
        {
            feedback_stream_ << message << endl;
        }
    }
}

//---------------------------------------------------------------------------------------
// get_port() - get port component of service access point
//
//    support for accessing full range of port numbers
//---------------------------------------------------------------------------------------
//
port_number_class socket_service_access_point_query_class::get_port(void)
{
    //  *** set up a few static strings, for dialogue with user ***
    //
    static const string whichPort = "please enter port to access";

    while (TRUE)   // loop ends with return
    {
        const unsigned port_number =
            get_unsigned(whichPort, 
               interval_template<unsigned>(
                   MIN_PORT_NUMBER_, 
                   MAX_PORT_NUMBER_, 
                   intervalEndpointType::CLOSED_ENDPOINT, 
                   intervalEndpointType::CLOSED_ENDPOINT
            ));
        try
        {
            return port_number_class(port_number);
        }
        catch (const basic_exception& message)
        {
            feedback_stream_ << message << endl;
        }
    }
}

//---------------------------------------------------------------------------------------
// get_allocatable_port() - get port number component of service access point
//
//     support only for accessing ports in user-allocatable range
//---------------------------------------------------------------------------------------
//
allocatable_port_number_class socket_service_access_point_query_class::get_allocatable_port(void)
{
    //  *** set up strings for dialogue with user ***
    //
    static const string whichPort = "please enter communications port";

    //  *** query until valid port obtained ***
    //
    while (TRUE)   // loop ends with return
    {
        // NOTE:  g++ 2.8.1 gets confused if second (default) argument missing
        //
        const unsigned port_number =
            get_unsigned(whichPort, 
               interval_template<unsigned>(
                   MIN_ALLOCATABLE_PORT_NUMBER_, 
                   MAX_ALLOCATABLE_PORT_NUMBER_, 
                   intervalEndpointType::CLOSED_ENDPOINT, 
                   intervalEndpointType::CLOSED_ENDPOINT
            ));
        try
        {
            return allocatable_port_number_class(port_number);
        }
        catch (const basic_exception& message)
        {
            feedback_stream_ << message << endl;
        }
    }
}