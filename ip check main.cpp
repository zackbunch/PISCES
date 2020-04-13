// ------------------------- ip check main.cpp --------
//
//  Author:   Phil Pfeiffer
//  Date:     August 1999
//  Last modified:  April 2020
//
//  Show IP address for specified DNS name
//-----------------------------------------------------

#include "platform.h"

#include <iostream>
#include <string>

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

#ifndef DNS_NAME_H
#include "dns_name.h"
#endif

// =====================================================================
//  issue program's lead message
// =====================================================================
//
void introductoryMessage(ostream& feedback_stream)
{
    feedback_stream << "Current time: " << daytimeClass() << endl;
    feedback_stream << endl;

    feedback_stream << "This program returns DNS data on a specified host";
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

    string thisIP = "";              // IP adr for which to obtain DNS name
    int interactWithUser = TRUE;     // TRUE if program should query user

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
        // -a - IP address to check
        // -i - interact with user, if present

        if (argc > 1)
        {
            //  **** go parse content of command line ****

            //  third argument -> list of recognized options; options with colons accept parameters.
            //  lack of fourth argument -> tolerate invalid options
            //
            getopt_class getOptions(argc, argv, "a:i");

            interactWithUser = getOptions.option_present('i');

            if (getOptions.option_present('a'))
                getOptions.get_parameter('a', thisIP);
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
                    // if IP specified via command line, use on first pass
                    if (thisIP == "")
                        queryObject.get_string("please enter IP address", thisIP);
                }

                const dns_name_class thisHost = dns_name_class(ip_address_class(thisIP));
                cout << "DNS name = " << thisHost << endl;
            }
            catch (const basic_exception& message)
            {
                cout << message << endl << endl;
            }

            thisIP = "";      // force new IP on next pass
        } while (interactWithUser && queryObject.yes_unless_n("do another IP address"));
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