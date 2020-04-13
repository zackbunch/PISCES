//--------------------n-way echo server main.cpp-------------------------------------------------------------
//
//   Author:  Phil Pfeiffer
//   Date:    Fall 1999
//   Last modified:  April 2020
//
//   Act as echo server for multiple, connected programs,
//   -.  dynamically adding connections to sockets and removing them as clients drop connections
//   -.  accepting fixed-format, fixed-length frames from clients, and
//   -.  reversing contents and returning.
//  This program's companion program is tcp fixed message length client main.cpp
//
//   Design notes:
//   -.  clients must support same buffer size as the size that's hard-coded into this program
//       -.  suggested improvement: make buffer size a command-line parameter
//   -.  The pointer to clientConnectionCount is passed as auxiliary parameter to the selectable
//       TCP classes due to issues with interaction between STL::map and MS's runtime
//       -.  when a static_cast< void * > of &clientConnectionCount is stored as
//             socketMessageBlock[ tcp_fixed_message_length_selectable_server_class::P_FEEDBACK_STREAM ]
//           downcasting that reference to unsigned long consistently returns a corrupted reference
//       -.  when a static_cast< void * > of a function that returns either
//             -.  *clientConnectionCount
//             -.  or a reference to clientConnectionCount
//           is stored as
//             socketMessageBlock[ tcp_fixed_message_length_selectable_server_class::P_FEEDBACK_STREAM ]
//           attempting to downcast the void * value to a function pointer, then execute that function,
//           consistently generates an access violation - even though socketMessageBlock holds the
//           correct address for that function.
//      Why this is happening is beyond me.  One plausible explanation can be found at
//          https://stackoverflow.com/questions/37221529/why-do-i-get-access-violation-when-trying-to-use-a-map
//      The author of the lone reply, kmdreko,
//      -.  states that the construct is creating an infinite recursion,
//      -.  adding that
//          "All in all, std::map is a bad internal structure to use for this. (Actually most of the
//           standard containers would cause similar problems)."
//   -. other problems involving a read lock access conflict involving feedback_stream were fixed by
//      eliminated by passing feedback_stream as a constructor parameter, rather than via socketMessageBlock.
//------------------------------------------------------------------------------------------------------------

#include <iostream>     // cin, cout, clog
#include <string>

#ifndef STD_H
#include "std.h"
#endif

#ifndef DAYTIME_H
#include "daytime.h"    // time of day class
#endif

#ifndef HOST_DATA_H
#include "host_data.h"    // TCP/IP host data class
#endif

#ifndef QUERY_SERVER_H
#include "query_server.h"   // server query-response class
#endif

#ifndef MULTIPLE_SOCKET_SELECT_H
#include "multiple_socket_select.h"   // multiple socket select
#endif

#ifndef TCP_FIXED_MESSAGE_LENGTH_SELECTABLE_SERVER_H
#include "tcp_fixed_message_length_selectable_server.h"
#endif

#ifndef TCP_FIXED_MESSAGE_LENGTH_SELECTABLE_ROLLOVER_H
#include "tcp_fixed_message_length_selectable_rollover.h"
#endif

// *===*===*===*===*===*===*===*===*===*===*===*===*===*===*===*===*===*===
// program environmental parameters
// *===*===*===*===*===*===*===*===*===*===*===*===*===*===*===*===*===*===

// **** streams for user communication ****

ostream& feedback_stream = clog;
ostream& query_stream = cout;
istream& response_stream = cin;

// **** size of buffer for communication ****

const unsigned MESSAGE_SIZE = 40;

void introductoryMessage(ostream& os)
{
    os << "Current time: " << daytimeClass() << endl << endl;

    os << "This program accepts messages from multiple clients on a port of your choice." << endl
        << "Messages are reversed and returned to the respective clients." << endl;

    os << "After any message, you will be allowed to stop communications " << endl
        << "with the current client, and stop the program. " << endl
        << endl;
}

// *********************************************************************
// program main
// *********************************************************************

int main(void)
{
    // *** *** maintain status variable for final exit code *** ***
    //
    const int GOOD_EXIT = 0;
    const int BAD_EXIT = 1;
    int finalStatus = GOOD_EXIT;    // assume all OK, for starters

    // *** *** advertise program's purpose, function *** ***
    //
    feedback_stream << endl << "current host information: " << host_data_class() << endl;
    introductoryMessage(feedback_stream);

    // **** instantiate objects for monitoring client and server ports ****
    //
    multiple_socket_select_class socketMonitoringObject;
    unsigned connectedClientCount = 0;

    // **** instantiate object for main <-> socket class communication ****
    //
    //  param. for allowing methods to send feedback to the user interface
    //  param. for counting sockets being monitored
    //  param. for checking sockets, using select()
    //
    argument_block_type socketMessageBlock;
    socketMessageBlock[tcp_fixed_message_length_selectable_server_class::P_SOCKET_SELECT_OBJECT] = static_cast<void*>(&socketMonitoringObject);

    try
    {
        // *********************************************
        //    instantiate sockets for communications
        // *********************************************
        //
        //  ****  keep it simple: one server socket, establish connections and communicate until connected socket count drops to zero
        //
        // ==== ask user for port/IP adr access; bind to port ====
        //
        server_query_class serverQueryObject(response_stream, query_stream, feedback_stream);

        tcp_fixed_message_length_selectable_server_class serverSocket(feedback_stream, MESSAGE_SIZE, connectedClientCount);
        serverQueryObject.establish_service_port(serverSocket);

        // ==== set backlog as high as we can go ===
        //
        serverSocket.listen();

        // **** set up to monitor client connections ****
        //
        // ==== server socket will add rollover sockets as they're instantiated

        socketMonitoringObject.update_scan(serverSocket);

        // *******************************************************
        //  loop to pass multiple messages between client, server
        // *******************************************************

        socket_event_set_type activeSockets;
        do
        {
            // *** wait for socket(s) to become active ***
            //
            socketMonitoringObject.check_for_events(activeSockets);

            // *** loop through active socket(s), echoing messages
            //
            socket_event_set_type::iterator p_socket_activity_descriptor;

            for (
                p_socket_activity_descriptor = activeSockets.begin();
                p_socket_activity_descriptor != activeSockets.end();
                p_socket_activity_descriptor++)
            {
                //-----------------------------------------------------------------------------
                // WARNING:
                //   Leave the first variable below as a selectable_socket_class* type.
                //   Converting pThisSocket to a variable of type selectable_socket_class
                //   kills any polymorphic bindings to the original object's
                //   read_event(), write_event(), and exception_event() methods
                //-----------------------------------------------------------------------------

                selectable_socket_class* pThisSocket = (*p_socket_activity_descriptor).first;
                socket_event_flags_class socket_activity_flags = (*p_socket_activity_descriptor).second;

                if (socket_activity_flags.read_flag())      pThisSocket->read_event(socketMessageBlock);
                if (socket_activity_flags.write_flag())     pThisSocket->write_event(socketMessageBlock);
                if (socket_activity_flags.exception_flag()) pThisSocket->exception_event(socketMessageBlock);
            }
        } while (connectedClientCount > 0);   // go until all client connections done
    }
    catch (const basic_exception& message)
    {
        feedback_stream << message << endl;
        feedback_stream << "?? unexpected exception: program ending" << endl;
        finalStatus = BAD_EXIT;
    }
    catch (...)
    {
        feedback_stream << "?? unexpected exception: program ending" << endl;
        finalStatus = BAD_EXIT;
    }
    exit(finalStatus);
}