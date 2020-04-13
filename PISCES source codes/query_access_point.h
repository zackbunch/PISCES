//--------------------query_access_point.h------------------------------------------
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

#ifndef QUERY_ACCESS_POINT_H
#define QUERY_ACCESS_POINT_H

#include <iostream>
#include <set>
#include <string>

#ifndef PLATFORM_H
#include "platform.h"
#endif

#ifndef STD_H
#include "std.h"
#endif

#ifndef QUERY_H
#include "query.h"         // base class--query methods class
#endif

#ifndef SERVICE_ACCESS_POINT_H
#include "service_access_point.h"       // tcp service access point class
#endif

class socket_service_access_point_query_class : public query_class
{
public:
    //=======================================
    //  constructors
    //=======================================
    //
    //  first form--
    //         sets response, query, feedback streams as specified
    //
    //  second form-- copy constructor
    //
    socket_service_access_point_query_class(istream & = cin, ostream & = cout, ostream & = cout);
    socket_service_access_point_query_class(socket_service_access_point_query_class&);

    //===============================================
    //   manipulators
    //===============================================
    //
    //  repeatedly
    //     query user for service access point
    //     until valid service access point returned
    //
    //  -.  first form:   get allocatable access point (e.g., for bind)
    //  -.  second form:  get any access point

    void get_allocatable_access_point(socket_service_access_point_class&);
    void get_access_point(socket_service_access_point_class&);

    //===============================================
    //  destructor
    //===============================================

    virtual ~socket_service_access_point_query_class(void);

private:
    //===============================================
    //   private member functions
    //===============================================
    //
    //   get port component of service access point
    //   (allocatable, restricted IP number versions)
    //   get host IP component of service access point

    allocatable_port_number_class get_allocatable_port(void);
    port_number_class get_port(void);
    ip_address_class get_host_ip(void);

    //===============================================
    //   private member data
    //===============================================
    //
    // standard ranges for all TCP/UDP port numbers, plus allocatable ports
    
    static const unsigned MIN_PORT_NUMBER_;
    static const unsigned MAX_PORT_NUMBER_;
    static const unsigned MIN_ALLOCATABLE_PORT_NUMBER_;
    static const unsigned MAX_ALLOCATABLE_PORT_NUMBER_;
};

#endif
