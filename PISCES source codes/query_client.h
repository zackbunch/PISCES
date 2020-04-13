//--------------------query_client.h--------------------------------------
//
//  Author:  Phil Pfeiffer
//  Date:    September 1999
//  Last modified:  April 2020
//
//  Query user for server service access point, then establish connection
//  specified server.
//
//  Methods:
//   -.  establish_connection( ) -
//          repeatedly
//              query user for access point for server computer,
//              attempt to establish connection,
//          until connection established, or user gives up
//
//  Design note:
//     Basic methods for interacting with users inherited from query class.
//--------------------------------------------------------------------------

#ifndef QUERY_CLIENT_H
#define QUERY_CLIENT_H

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

#ifndef SOCKET_CLIENT_H
#include "socket_client.h"       // client socket class--for forging connection
#endif

#ifndef QUERY_ACCESS_POINT_H
#include "query_access_point.h"      // base class--socket service access point
                           // query methods class
#endif

class client_query_class : public socket_service_access_point_query_class
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
    client_query_class(istream & = cin, ostream & = cout, ostream & = cout);
    client_query_class(client_query_class&);

    //===============================================
    //   manipulators
    //===============================================
    //
    // repeatedly
    //    query user for server service access point,
    //    and attempt to connect on specified socket,
    // until connection established

    void establish_connection(client_socket_class&);

    //===============================================
    //  destructor
    //===============================================

    virtual ~client_query_class(void);
};

#endif
