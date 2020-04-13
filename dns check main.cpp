// ------------------------- dns check main.cpp --------------
//
//  Author:  Phil Pfeiffer
//  Date:    August 1999
//  Last modified:   April 2020
//
//  Shows IP address for specified DNS name
//------------------------------------------------------------

#include <iostream>
#include <string>

#ifndef PLATFORM_H
#include "platform.h"
#endif

#ifndef STD_H
#include "std.h"
#endif

#ifndef QUERY_H
#include "query.h"
#endif

#ifndef GETOPT_H
#include "getopt.h"
#endif

#ifndef DAYTIME_H
#include "daytime.h"
#endif

#ifndef IP_ADDRESS_H
#include "ip_address.h"
#endif

// =====================================================================
//  issue program's lead message
// =====================================================================
//
void introductoryMessage(ostream& feedback_stream)
{
    feedback_stream << "Current time: " << daytimeClass() << endl;
    feedback_stream << endl;

    feedback_stream << "This program returns the IP address of a specified host";
    feedback_stream << endl;
}

// *********************************************************
// MAIN PROGRAM
// *********************************************************

int main(const int argc, const char* argv[])
{
    // *****************************************************
    // ****  enumerate objects in environment to init   ****
    // *****************************************************

    string thisDNS = "";                   // DNS name for which to obtain IP adr
    int dns_nameFromCommandLine = FALSE;   // TRUE => name from cmd line param
    int interactWithUser = TRUE;           // TRUE if program should query user

    //  ---- streams for interacting with user ----
    //
    //  feedback stream-  stream for sending unsolicited info to user
    //  query stream-     stream for sending queries to user
    //  response stream-  stream for receiving query responses from user
    //
    ostream& feedback_stream = clog;
    ostream& query_stream = cout;
    istream& response_stream = cin;

    //  ---- objects for interacting with user ---
    //
    //  queryObject:  for query/response with program user.
    //     uses cout, cin, cout for query, response, feedback streams.
    //
    query_class queryObject(response_stream, query_stream, feedback_stream);

    //  ---- return status codes ----
    //
    const int BAD_RETURN = -1;
    const int GOOD_RETURN = 0;

    int returnStatus = GOOD_RETURN;

    try
    {
        // ******************************************************
        // *****  gather command-line-supplied parameters   *****
        // ******************************************************

        // if command line present (argc > 1), look for following options
        //
        // -d - DNS name to check  (empty for self)
        // -i - interact with user, if present

        if (argc > 1)
        {
            //  **** go parse content of command line ****

            //  third argument -> list of recognized options; options with colons accept parameters.
            //  lack of fourth argument -> tolerate invalid options
            //
            getopt_class getOptions(argc, argv, "d:i");

            interactWithUser = getOptions.option_present('i');

            if (getOptions.option_present('d'))
            {
                getOptions.get_parameter('d', thisDNS);
                dns_nameFromCommandLine = TRUE;
            }
        }

        // ******************************************************************
        // *** introduce program's operation, if called for ***
        // ******************************************************************

        if (interactWithUser) introductoryMessage(feedback_stream);

        // ****************************
        // *** try out IP addresses ***
        // ****************************

        do
        {
            try
            {
                if (interactWithUser)
                {
                    // if string specified via command line, use on first pass
                    if (!dns_nameFromCommandLine) queryObject.get_string("please enter DNS name", thisDNS);
                }
                const ip_address_class thisHost(thisDNS, ipAddressType::DNS_NAME);
                cout << "IP address = " << thisHost.ip_address_as_string() << endl;    // and show DNS name
            }
            catch (const basic_exception& message)
            {
                cout << message << endl << endl;
            }

            dns_nameFromCommandLine = FALSE;   // force new DNS on next pass
        } while (interactWithUser && queryObject.yes_unless_n("do another DNS name"));
    }
    catch (const basic_exception& message)
    {
        cout << "?? unexpected exception in program main " << endl;
        cout << message << endl << endl;
        returnStatus = BAD_RETURN;
    }
    catch (...)
    {
        cout << "?? caught exception of undetermined type" << endl;
        returnStatus = BAD_RETURN;
    }

    return returnStatus;
}