// ------------------------- selfchk.cpp ---------------------------
//
//  Author:   Phil Pfeiffer
//  Date:     August 1999
//  Last modified:  April 2020
//
//  Show DNS data for current host
//------------------------------------------------------------------

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

#ifndef HOST_DATA_H
#include "host_data.h"
#endif

// *********************************************************
// MAIN PROGRAM
// *********************************************************

// =====================================================================
//  issue program's lead message
//
void introductoryMessage(ostream& feedback_stream)
{
    feedback_stream << "Current time: " << daytimeClass() << endl;
    feedback_stream << endl;

    feedback_stream << "This program returns DNS data on the current host";
    feedback_stream << endl;
}

int main(const int argc, const char* argv[])
{
    // *****************************************************
    // ****  enumerate objects in environment to init   ****
    // *****************************************************

    //  ---- streams for interacting with user ----
    //
    //  feedback stream-  stream for sending unsolicited info to user
    //
    ostream& feedback_stream = clog;

    //  ---- return status codes ----
    //
    const int BAD_RETURN = -1;
    const int GOOD_RETURN = 0;

    int returnStatus = GOOD_RETURN;

    try
    {
        introductoryMessage(feedback_stream);
        cout << "current " << host_data_class() << endl;
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