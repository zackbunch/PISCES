//--------------tcp fixed message length client main.cpp-----------------------------------------------------
//
//  Author:  Phil Pfeiffer
//  Date:    April 2020
//
//  Illustrate operation of PISCES socket class library's support for TCP communications.
//  This program's companion program is tcp fixed message length client main.cpp
//
//   Design notes:
//   -.  clients must support same buffer size as the size that's hard-coded into this program
//       -.  suggested improvement: make buffer size a command-line parameter
//------------------------------------------------------------------------------------------------------------

#include <iostream>     // cin, cout, clog
#include <string>

#ifndef STD_H
#include "std.h"
#endif

#ifndef HOST_DATA_H
#include "host_data.h"    // TCP/IP host data class
#endif

#ifndef QUERY_SERVER_H
#include "query_server.h"   // client query-response class
#endif

#ifndef TCP_FIXED_MESSAGE_LENGTH_SERVER_H
#include "tcp_fixed_message_length_server.h"     // tcp server socket class
#endif

#ifndef TCP_FIXED_MESSAGE_LENGTH_ROLLOVER_H
#include "tcp_fixed_message_length_rollover.h"     // tcp server socket class
#endif

#ifndef DAYTIME_H
#include "daytime.h"    // time of day class
#endif

void introductory_message(ostream& os)
{
    os << "Current time: " << /*replace*/daytimeClass() << endl << endl;

    os << "This program receives data from a client on a port of your choice, reversing and returning it to the client." << endl
        << "You, the user, will first be asked to specify a service access point to use to receive content." << endl;

    os << "After any message, you will be allowed to stop communications " << endl
        << "with the current server, and stop the program. " << endl
        << endl;
}

int main(void)
{
    // *** *** maintain status variable for final exit code *** ***
    //
    const int GOOD_EXIT = 0;
    const int BAD_EXIT = 1;
    int final_status = GOOD_EXIT;    // assume all OK, for starters

    ostream& feedback_stream = clog;
    ostream& query_stream = cout;
    istream& response_stream = cin;

    const unsigned BUFFER_PAYLOAD_SIZE = 40;
    const unsigned MAX_CONNECTION_COUNT = 3;

    // *** *** advertise program's purpose, function *** ***
    //
    introductory_message(feedback_stream);

    try
    {
        //  *** instantiate object for dialoguing with user about stuff,
        //      including socket connection parameters
        //
        server_query_class query_object(response_stream, query_stream, feedback_stream);

        // *** display current host's name, IP address ***
        //
        feedback_stream << endl << "current " << host_data_class() << endl;

        // *********************************************
        //   loop to communicate with multiple servers
        // *********************************************

        do
        {
            // ==== instantiate a socket for communications ====
            //
            //  must reinstantiate socket on every pass through loop:
            //  a WinSock socket, once closed, may not be reopened.
            //
            tcp_fixed_message_length_server_class server_socket;

            try
            {
                // *********************************************
                //   set up one service port
                // *********************************************

                // ==== ask user for port/IP adr access; bind to port ====
                //
                query_object.establish_service_port(server_socket);

                // ==== await attempts to communicate ===
                //
                server_socket.listen(MAX_CONNECTION_COUNT);

                // ***************************************************
                //  loop to exchange messages with multiple clients
                // ***************************************************

                do
                {
                    // **** establish connection with client ****

                    //  when a server connects to a client using accept(),
                    //  the socket API "rolls the communication over" onto
                    //  another socket-- referred to here as a "rollover socket".

                    tcp_fixed_message_length_rollover_class rollover_socket(BUFFER_PAYLOAD_SIZE);
                    server_socket.accept(rollover_socket);

                    // ==== display port number for connection ====
                    //
                    try
                    {
                        const socket_service_access_point_class my_sap = rollover_socket.service_access_point();
                        feedback_stream << "connection estalished on port " << my_sap.port_object() << endl;
                    }
                    catch (const basic_exception& message)
                    {
                        feedback_stream << "?? could not get local port for connection" << endl << message << endl;
                    }

                    // ==== display client ID, if obtainable ====
                    //
                    try
                    {
                        const socket_service_access_point_class peer_sap = rollover_socket.peer_service_access_point();
                        feedback_stream << "client connection = " << peer_sap << endl;
                    }
                    catch (const basic_exception& message)
                    {
                        feedback_stream << "?? could not get data on client connection" << endl << message << endl;
                    }

                    // *********************************************************
                    //  loop to exchange multiple messages with current client
                    // *********************************************************

                    try
                    {
                        do
                        {
                            string client_message;
                            rollover_socket.indication(client_message);

                            feedback_stream << "client input: " << client_message << endl;

                            rollover_socket.response();
                        } while (query_object.yes_unless_n("another message?"));
                    }
                    catch (const basic_exception& message)
                    {
                        feedback_stream << message << endl << "breaking connection" << endl;
                    }

                    //  === end communications with current client ===
                    //
                    rollover_socket.shutdown();
                    rollover_socket.closesocket();
                } while (query_object.yes_unless_n("establish new session on current port?"));
            }
            catch (const socket_exception& message)
            {
                feedback_stream << message << endl << "ending use of current socket" << endl;
            }

            //  === end communications on current port ===
            //
            server_socket.shutdown();
            server_socket.closesocket();
        } while (query_object.yes_unless_n("start another session?"));
    }

    catch (const basic_exception& message)
    {
        feedback_stream << message << endl;
        feedback_stream << "?? unexpected exception: program ending" << endl;
        final_status = BAD_EXIT;
    }
    catch (...)
    {
        feedback_stream << "?? unexpected exception: program ending" << endl;
        final_status = BAD_EXIT;
    }
    return final_status;
}