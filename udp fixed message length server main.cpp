//--------------udp fixed message length server main.cpp-----------------------------------------
//
//  Author:  Phil Pfeiffer
//  Date:    Fall 1999
//  Last modified:  April 2020
//
//  Illustrate operation of PISCES socket class library's support for UDP communications
//  This program's companion program is sample udp client.cpp
//
//   Design notes:
//   -.  clients must support same buffer size as the size that's hard-coded into this program
//       -.  suggested improvement: make buffer size a command-line parameter
//------------------------------------------------------------------------------------------------

#include "udp_fixed_message_length_server.h"

#ifndef PLATFORM_H
#include "platform.h"   // platform-specific definitions
#endif

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

#ifndef DAYTIME_H
#include "daytime.h"    // time of day class
#endif

// *===*===*===*===*===*===*===*===*===*===*===*===*===*===*===*===*===*===
// definition of "introduce self" routine
// *===*===*===*===*===*===*===*===*===*===*===*===*===*===*===*===*===*===

void introductory_message(ostream& os)
{
    os << "Current time: " << /*replace*/daytimeClass() << endl << endl;

    os << "This program accepts messages from clients on a port of your choice." << endl
        << "Messages are displayed on the screen, reversed, and returned to the client."
        << endl;

    os << "After any message, you will be allowed to stop communications " << endl
        << "with the current client, and stop the program. " << endl
        << endl;
}

int main(void)
{
    // *** *** maintain status variable for final exit code *** ***
    //
    const int good_exit = 0;
    const int bad_exit = 1;
    int final_status = good_exit;    // assume all OK, for starters

    // **** streams for user communication ****

    ostream& feedback_stream = clog;
    ostream& query_stream = cout;
    istream& response_stream = cin;

    // **** size of buffer for communication ****

    const unsigned BUFFER_PAYLOAD_SIZE = 40;

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
        //feedback_stream << endl << "current " << host_data_class() << endl;

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
            udp_fixed_message_length_server_class server_socket(BUFFER_PAYLOAD_SIZE);

            try
            {
                // *********************************************
                //   set up one service port
                // *********************************************

                // ==== ask user for port/IP adr access; bind to port ====
                //
                query_object.establish_service_port(server_socket);

                // ***************************************************
                //  loop to exchange messages with multiple clients
                // ***************************************************

                do
                {
                    // *********************************************************
                    //  loop to exchange multiple messages with current client
                    // *********************************************************

                    try
                    {
                        do
                        {
                            string client_message;
                            socket_service_access_point_class client_sap;
                            server_socket.indication(client_message, client_sap);
                            feedback_stream << "input from " << client_sap << ": " << client_message << endl;
                            server_socket.response();
                        } while (query_object.yes_unless_n("another message?"));
                    }
                    catch (const basic_exception& message)
                    {
                        feedback_stream << message << endl << "breaking connection" << endl;
                    }
                } while (query_object.yes_unless_n("establish new session on current port?"));
            }
            catch (const socket_exception& message)
            {
                feedback_stream << message << endl << "ending use of current socket" << endl;
            }

            //  === end communications on current port ===
            //
            server_socket.closesocket();
        } while (query_object.yes_unless_n("start another session?"));
    }

    catch (const basic_exception& message)
    {
        feedback_stream << message << endl;
        feedback_stream << "?? unexpected exception: program ending" << endl;
        final_status = bad_exit;
    }
    catch (...)
    {
        feedback_stream << "?? unexpected exception: program ending" << endl;
        final_status = bad_exit;
    }
    return final_status;
}