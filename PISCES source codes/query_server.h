//--------------------query_server.h--------------------------------------
//
//  Author:  Phil Pfeiffer
//  Date:    September 1999
//  Last modified:  April 2020
//
//  Query user for server service access point, then allocate point
//  by binding to it.
//
//  Methods:
//   -.  establish_service_port( ) -
//          repeatedly
//              query user for access point for server computer,
//              attempt to establish connection,
//          until connection established, or user gives up
//
//  Design note:
//     Basic methods for interacting with users inherited from query class.
//----------------------------------------------------------------------------

#ifndef QUERY_SERVER_H
#define QUERY_SERVER_H

#include <iostream>
#include <set>
#include <string>

#ifndef PLATFORM_H
#include "platform.h"
#endif

#ifndef STD_H
#include "std.h"
#endif

#ifndef SERVICE_ACCESS_POINT_H
#include "service_access_point.h"       // tcp service access point class
#endif

#ifndef SOCKET_SERVER_H
#include "socket_server.h"       // server socket class--for forging binding
#endif

#ifndef QUERY_ACCESS_POINT_H
#include "query_access_point.h"      // base class--socket service access point query methods class
#endif

class server_query_class : public socket_service_access_point_query_class
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
    server_query_class(istream & = cin, ostream & = cout, ostream & = cout);
    server_query_class(server_query_class&);

    //===============================================
    //   manipulators
    //===============================================
    //
    // repeatedly
    //    query user for local service access point,
    //    and attempt to bind it using specified socket
    // until port allocated

    void establish_service_port(server_socket_class&);

    //===============================================
    //  destructor
    //===============================================

    virtual ~server_query_class(void);
};

#endif
