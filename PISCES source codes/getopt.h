//==========================  getopt.h  ==========================================================================
//
//  Author:  Phil Pfeiffer
//  Date:    August 1999
//  Last modified:  April 2020
//
//  getopt() is a Posix-standard procedure for parsing command line **options**.
//  Options are essentially named arguments that appear at the head of a
//  Posix-compliant program's initial list of arguments:
//
//     *.  A Posix-compliant program receives **arguments**, typically from a
//         DOS-style command line, using two parameters to main, argc and argv:
//
//         -.  a program's arguments are the individual strings that make up
//             that program's comamnd line.
//
//         -.  argv is a vector of pointers to C-style strings that captures
//             the program's command line arguments.
//
//              -.  the argv vector orders the arguments in the order in which
//                  those arguments appear on the command line
//
//              -.  the string used to invoke the program, by convention, is
//                  assigned to argv[0]
//
//         -.  argc counts the number of items in argv
//
//         example:  the argv/argc pair for "foo -a -- b -c"
//         would be [argv = 4, argc = {"foo","-a","--","b","-c"}]
//
//     *.  A Posix-compliant program's argument argv[n] is an **option**
//         if argv[n] satisfies the following condition:
//
//         -.  argv[n] is argv[1], or argv[n] **immediately** follows an
//             option other than "--".
//
//         -.  argv[n] is a string that contains at least one character,
//             including an initial "-".
//             This second character is referred to as the option's **name**.
//
//         example:  in the previous example, "-a" and "--" are options.
//
//  These two conditions on options, in effect, state that
//
//     -.   a program's options are grouped at the head of that program's
//          command line list, and
//     -.   are terminated by either
//          -.  a non-option, or
//          -.  the designated option-list-terminating arguments "-" and "--"
//
//  Options may also be **recognized** or **unrecognized**.
//
//     *.  getopt() takes as one of its arguments a string of characters
//         representing **recognized** options.  Any option name that
//         appears in this list is a **recognized** option.  Any other
//         option is an **unrecognized** option
//
//         The difference between **recognized** and **unrecognized** options
//         is significant iff another parameter to getopt(), a global value,
//         is set to TRUE.  If this parameter is set to true, then
//         unrecognized options are parsed to "?".
//
//  Finally, options may have **parameters**.  An option is parameterized
//  if either of the following holds:
//
//     *.  The argument consists of more than two characters.  In this case,
//         the associated argument is all characters after the first two.
//
//     *.  The argument argv[n]
//         -.  consists of exactly two characters;
//         -.  appears in the list of recognized options; and
//         -.  the appearance in the list of recognized options is followed
//             immediately by a ":"
//         If these three conditions hold, then
//         -.  argv[n+1] is treated as argv[n]'s argument if argv[n+1] exists,
//         -.  or "" is treated as argv[n]'s argument, otherwise
//
//  Design notes:
//     getopt() is an iterative routine for parsing a program's command line
//     options.  This module contains a class that parses a getopt()-like
//     command line at one throw, then allows a program, through subsequent
//     method calls, to determine a command line's options and their arguments.
//
//     The getopt_class constructor accepts the following arguments:
//
//     -.  a vector of pointers to C-style strings, argv,
//         presumably supplied to a program via a command line argument.
//
//     -.  a count of the number of items in the argv vector, minus 1.
//         argv[0] by convention is the name under which the program was invoked.
//
//     -.  a string of one-letter-long option names, each of which may
//         be followed by a colon to indicate that the option accepts a parameter.
//
//      -.  a flag that indicates whether the presence of options that are
//          not in the option name string is to be treated as an error.
//
//      The constructor examines successive elements of argv[1...argc-1], and either
//
//      -.  processes the current element of argv[], if the current element is an option other than "--";  or
//      -.  discontinues processing, if the current element of argv is either
//          -.  the special option "--", or
//          -.  an ordinary command line argument (i.e., not an option)
//
//      After the argument list has been processed, the constructor
//      -.  throws an exception if any unrecognized options have been found, or
//      -.  returns normally.
//
//      The following methods can then be used to inspect the result of the  parse:
//      -.  option_present()        - returns TRUE iff nth instance of option was present
//      -.  get_parameter()         - retrieves argument for specified instance of
//                                    option, returning TRUE iff instance had an argument
//      -.  first_nonoption_argv()  - returns index of first non-option in argv
//====================================================================================================================
#ifndef GETOPT_H
#define GETOPT_H

#include <iostream>
#include <set>
#include <string>
#include <vector>
#include <map>

#ifndef PLATFORM_H
#include "platform.h"
#endif

#ifndef BOOL_H
#include "bool.h"
#endif

#ifndef STD_H
#include "std.h"
#endif

//*****************************************************************
//  option parsing class proper
//*****************************************************************

class getopt_class
{
public:
    //=======================================
    //  constructors
    //=======================================
    //
    //  first form--
    //        accepts argv, argc, recognized options list, and
    //        "throw exception on unrecognized option" flag.
    //
    //  second form-- copy constructor
    //
    getopt_class(
        const unsigned,
        const char* [],
        const string&,
        const int throw_exception_on_unrecognized_option = FALSE
    );
    getopt_class(const getopt_class&);

    //===============================================
    //   inspectors
    //===============================================
    //
    //  identify if nth instance of option was present.
    //  instance is a zero-offset value-- first instance is instance 0

    int option_present(const char, const unsigned = 0);

    //  return TRUE iff nth instance of option had an argument,
    //  setting string to this argument.
    //  instance is a zero-offset value-- first instance is instance 0

    int get_parameter(const char, string&, const unsigned = 0);

    //  return index, in argv, of first non-option argument

    unsigned first_nonoption_argv(void) const;

    //===============================================
    //  destructor
    //===============================================

    virtual ~getopt_class(void);

private:
    //========================================
    //   private type definitions
    //========================================

    // for specifying option occurrences data structure

    typedef pair< int, string > occurrence_type;
    typedef vector< occurrence_type > occurrence_vector_type;
    typedef map< char, occurrence_vector_type > option_to_occurrence_vector_type;

    //========================================
    //   private member data
    //========================================

    //  mapping from options -> vector of occurrences
    //
    option_to_occurrence_vector_type options_to_parameter_data_vector_;

    //  holds index of first unparsed argument when constructor finishes
    //
    unsigned next_argv_to_parse_;
};

// **************************************************************************
// exception classes for getopt class
// **************************************************************************

#ifndef EXCEPTION_H
#include "exception.h"
#endif

// sole exception.here:  unrecognized_option_exception

class unrecognized_option_exception : public basic_exception
{
public:
    unrecognized_option_exception(const string & = "", const string & = "", const string & = "");
    unrecognized_option_exception(const unrecognized_option_exception&);

    virtual ~unrecognized_option_exception(void);
};

// *===*====*===*====*===*====*===*====*===*====*===*====*===*====*===*====
// auxiliary operators
// *===*====*===*====*===*====*===*====*===*====*===*====*===*====*===*====

ostream& operator<<(ostream&, const unrecognized_option_exception&);

#endif
