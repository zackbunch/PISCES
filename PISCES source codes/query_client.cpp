//--------------------query_client.cpp-------------------------------------------
//
//  Author:         Phil Pfeiffer
//  Date:           September 1999
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
//--------------------------------------------------------------------------------

#include "query_client.h"

#ifndef BOOL_H
#include "bool.h"
#endif

//=======================================
//  constructors
//=======================================

//---------------------------------------------------------------------
//  first  form-- sets istream, ostream to user-specified streams
//---------------------------------------------------------------------
//
client_query_class::client_query_class(istream& response_stream, ostream& query_stream, ostream& feedback_stream)
    : socket_service_access_point_query_class(response_stream, query_stream, feedback_stream)
{
}

//---------------------------------------------------------------------
//  second form-- copy constructor
//---------------------------------------------------------------------
//
client_query_class::client_query_class(client_query_class& source)
    : socket_service_access_point_query_class(source)
{
}

//===============================================
//   manipulators
//===============================================

// ----------------------------------------------------------------
// establish_connection() -
//     repeatedly
//         query user for access point for server computer,
//         attempt to establish connection,
//     until connection established, or user gives up
//
// ----------------------------------------------------------------
//
void client_query_class::establish_connection(client_socket_class& sock)
{
    //  *** connect to user-specified service access point,
    //      allow usering to retry connect operation on failure ***

    socket_service_access_point_class access_point;

    while (TRUE)    // loop exits with return
    {
        get_access_point(access_point);    // ask user for service access point
        try                               // attempt the connect
        {
            sock.connect(access_point);
            return;
        }
        catch (const basic_exception& message)
        {
            feedback_stream_ << message << endl;
        }
    }
}

//===============================================
//  destructor
//===============================================

client_query_class::~client_query_class(void)
{
}