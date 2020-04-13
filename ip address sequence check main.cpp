// ------------------------- ip address sequence check main.cpp -------
//
//  Author:   Phil Pfeiffer
//  Date:     August 1999
//  Last modified:  April 2020
//
//  Show IP address for sequence of IP addresses
//---------------------------------------------------------------------

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

    feedback_stream << "This program returns DNS data on a range of IP addresses";
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

    string startIP = "";             // first IP adr for which to get DNS name
    string finalIP = "";             // final IP adr for which to get DNS name
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
        // -s - starting IP address to check
        // -f - final IP address to check
        // -i - interact with user, if present

        if (argc > 1)
        {
            //  **** go parse content of command line ****

            //  third argument -> list of recognized options;
            //                    options with colons accept parameters.
            //  lack of fourth argument -> tolerate invalid options
            //
            getopt_class getOptions(argc, argv, "s:f:i");

            interactWithUser = getOptions.option_present('i');

            if (getOptions.option_present('s'))
                getOptions.get_parameter('s', startIP);
            if (getOptions.option_present('f'))
                getOptions.get_parameter('f', finalIP);
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
                    if (startIP == "")
                        queryObject.get_string("please enter first IP address for scan", startIP);
                    if (finalIP == "")
                        queryObject.get_string("please enter final IP address for scan", finalIP);
                }

                const ip_address_class startAdr(startIP);
                const ip_address_class finalAdr(finalIP);
                const ip_address_class firstAdr = (startAdr < finalAdr) ? startAdr : finalAdr;
                const ip_address_class lastAdr = (startAdr < finalAdr) ? finalAdr : startAdr;

                ip_address_class thisAdr;

                for (thisAdr = firstAdr; thisAdr <= lastAdr; thisAdr++)
                {
                    try
                    {
                        const dns_name_class thisHost = dns_name_class(thisAdr);
                        cout << "DNS name for IP address " << thisAdr << " = " << thisHost << endl;
                    }
                    catch (const basic_exception&)
                    {
                        cout << "?? can't get DNS name for " << thisAdr << endl;
                    }
                }
            }
            catch (const basic_exception& message)
            {
                cout << message << endl << endl;
            }

            startIP = "";      // force new IP range on next pass
            finalIP = "";
        } while (interactWithUser && queryObject.yes_unless_n("another IP address range"));
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