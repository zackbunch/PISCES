//--------------------query_server.cpp----------------------------------------
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
//-----------------------------------------------------------------------------

#include "query_server.h"

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
server_query_class::server_query_class(istream& response_stream, ostream& query_stream, ostream& feedback_stream)
    : socket_service_access_point_query_class(response_stream, query_stream, feedback_stream)
{
}

//---------------------------------------------------------------------
//  second form-- copy constructor
//---------------------------------------------------------------------
//
server_query_class::server_query_class(server_query_class& source)
    : socket_service_access_point_query_class(source)
{
}

//===============================================
//   manipulators
//===============================================

// ----------------------------------------------------------------
// establish_service_port() -
//
// repeatedly
//    query user for local service access point,
//    and attempt to bind it using specified socket
// until port allocated
// ----------------------------------------------------------------
//
void server_query_class::establish_service_port(server_socket_class& sock)
{
    //  *** connect to user-specified service access point,
    //      allow user to retry connect operation on failure ***

    socket_service_access_point_class access_point;

    while (TRUE)    // loop exits with return
    {
        // ask user for local service access point for establishing service

        get_allocatable_access_point(access_point);
        try                               // attempt the bind
        {
            sock.bind(access_point);
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

server_query_class::~server_query_class(void)
{
}