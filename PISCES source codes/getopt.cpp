//==========================  getopt.cpp  =================================================================================
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
//==========================================================================================================================

#include "getopt.h"

//*****************************************************************
//  option parsing class proper
//*****************************************************************

//=======================================
//  constructors
//=======================================

//--------------------------------------------------------------------------
//  constructor, first form-- retrieve options from option string,
//      throwing exception on unexpected options on user request
//
//  this constructor is the class's workhorse method.  see documentation in
//  header for details of protocol.
//--------------------------------------------------------------------------
//
getopt_class::getopt_class(const unsigned argc, const char* argv[], const string& known_options_list, const int throw_on_unknown)
    : next_argv_to_parse_(0)
{
    // **** set up static data ****
    //
    // option_has_parameters_flag - follows names of option in known_options_list that accept params

    static const char option_has_parameters_flag = ':';
    static const string my_name = "getopt_class:: constructor";
    static const string unknown_opt = "unrecognized options in arguments list";

    // **** initialize loop for scanning arguments ****

    string unknown_options = "";       // no unrecognized options, initially

    while (++next_argv_to_parse_ < argc)   // don't overrun vector
    {
        // **** If this argv[] isn't an option, there are no more options ****

        const string arg_string = argv[next_argv_to_parse_];

        if (arg_string.length() < 1) break;    // needs 2 chars to be an option
        if (arg_string[0] != '-')    break;    // initial char must be a '-'

        // **** this argv[] is an option--determine sense of option ****

        if (arg_string.length() == 1) break;   // "end of options" option ('-')

        const char option_name = arg_string[1];  // recover name for option
        if (option_name == '-') break;         // "end of options" option ('--')

        // **** determine option's properties ****
        //
        // is this a recognized option?
        // if so, does it accept a parameter?

        const int option_name_index = known_options_list.find(option_name);
        const int options_list_length = known_options_list.length();

        int can_accept_next_argv_as_parameter;

        if (option_name_index < 0 || option_name_index >= options_list_length)
        {
            //  unknown option--add to list of unknown options found
            //  note that this options's param can't spill over into next argv

            if (unknown_options != "")  unknown_options += ",";
            unknown_options += option_name;

            can_accept_next_argv_as_parameter = FALSE;
        }
        else
        {
            //  recognized option--determine if option also accepts arguments

            const int option_accepts_arguments_flag_index = option_name_index + 1;

            if (option_accepts_arguments_flag_index >= options_list_length)
                can_accept_next_argv_as_parameter = FALSE;
            else
                can_accept_next_argv_as_parameter =
                (known_options_list[option_accepts_arguments_flag_index] == option_has_parameters_flag);
        }

        // **** determine option's parameter, if there ****

        int parameter_is_present;
        string parameter;

        if (arg_string.length() > 2)
        {
            parameter_is_present = TRUE;
            parameter = arg_string.substr(2);    // copy from 2..end of string
        }
        else
        {
            parameter_is_present = can_accept_next_argv_as_parameter;

            if (can_accept_next_argv_as_parameter)
            {
                if (next_argv_to_parse_ < argc)    // more arguments?
                {
                    parameter = argv[next_argv_to_parse_];
                    next_argv_to_parse_++;
                }
                else
                    parameter = "";    // parameter null by default if no args
            }
        }

        // **** add <isPresent,parameter> to option's vector of occurrences ****

        // ==== create iterator that, we hope, points to the desired vector ====

        option_to_occurrence_vector_type::iterator p_option_vector_pair = options_to_parameter_data_vector_.find(option_name);

        // ==== see if item pointed to by iterator is empty ====
        //
        if (p_option_vector_pair == options_to_parameter_data_vector_.end())
        {
            //  --- empty => first occurrence of this option.
            //  --- extend map for option, adding parameter instance information
            //
            occurrence_vector_type initial_vector(1);
            initial_vector[0] = occurrence_type(parameter_is_present, parameter);
            options_to_parameter_data_vector_[option_name] = initial_vector;
        }
        else
        {
            //  --- nonempty => second or later occurrence of this option.
            //  ---- extend vector for this option, adding param  instance information
            //
            occurrence_vector_type& this_option_data_vector = (*p_option_vector_pair).second;

            const int vector_size = this_option_data_vector.size();
            this_option_data_vector.resize(vector_size + 1);
            this_option_data_vector[vector_size] = occurrence_type(parameter_is_present, parameter);
        }
    }
    if (unknown_options != "" && throw_on_unknown == TRUE)
    {
        unknown_options = "unknown options found: " + unknown_options;
        throw unrecognized_option_exception(my_name, unknown_opt, unknown_options);
    }
}

//--------------------------------------------------------------------------
//  constructor, second form--copy constructor
//--------------------------------------------------------------------------
//
getopt_class::getopt_class(const getopt_class& source) : options_to_parameter_data_vector_(source.options_to_parameter_data_vector_),
next_argv_to_parse_(source.next_argv_to_parse_)
{
}

//===============================================
//   inspectors
//===============================================

//-----------------------------------------------------------------------------
//  option_present() - identify if nth instance of option was present
//-----------------------------------------------------------------------------
//
int getopt_class::option_present(const char option, const unsigned instance)
{
    // *** create iterator that, we hope, points to the desired option's vector
    option_to_occurrence_vector_type::iterator p_option_vector_pair = options_to_parameter_data_vector_.find(option);

    //  *** if item pointed to by iterator is empty, option wasn't found ***
    if (p_option_vector_pair == options_to_parameter_data_vector_.end()) return FALSE;

    //  *** otherwise, get vector associated with option.  Requested instance
    //  *** exists iff vector's length exceeds requested instance number.
    //
    const occurrence_vector_type& this_option_data_vector = (*p_option_vector_pair).second;

    return (this_option_data_vector.size() >= instance);
}

//-----------------------------------------------------------------------------
//  get_parameter() -
//       return TRUE iff nth instance of option had an argument,
//       setting string to this argument.
//-----------------------------------------------------------------------------
//
int getopt_class::get_parameter(const char option, string& parameter, const unsigned instance)
{
    // *** create iterator that, we hope, points to the desired option's vector
    option_to_occurrence_vector_type::iterator p_option_vector_pair = options_to_parameter_data_vector_.find(option);

    //  *** if item pointed to by iterator is empty, option wasn't found ***
    if (p_option_vector_pair == options_to_parameter_data_vector_.end()) return FALSE;

    //  *** otherwise, get vector associated with option.  Requested instance
    //  *** exists iff vector's length exceeds requested instance number.
    //
    const occurrence_vector_type& this_option_data_vector = (*p_option_vector_pair).second;
    if (this_option_data_vector.size() < instance) return FALSE;

    //  *** finally, check first element in instance's associated pair.
    //  *** argument was there iff that element is TRUE.
    //
    if (this_option_data_vector[instance].first == FALSE) return FALSE;

    parameter = this_option_data_vector[instance].second;
    return TRUE;
}

//-----------------------------------------------------------------------------
//  first_nonoption_argv() - return index, in argv, of first non-option argument
//-----------------------------------------------------------------------------
//
unsigned getopt_class::first_nonoption_argv(void) const
{
    return next_argv_to_parse_;
}

//===============================================
//  destructor
//===============================================

getopt_class::~getopt_class(void)
{
}

// **************************************************************************
// exception classes for getopt class   [boilerplate]
// **************************************************************************

unrecognized_option_exception::unrecognized_option_exception(const string& name, const string& short_message, const string& long_message)
    : basic_exception(name, short_message, long_message) {}

unrecognized_option_exception::unrecognized_option_exception(const unrecognized_option_exception& source)
    : basic_exception(source) {}

unrecognized_option_exception::~unrecognized_option_exception(void)
{}

// *===*====*===*====*===*====*===*====*===*====*===*====*===*====*===*====
// auxiliary operators  [boilerplate]
// *===*====*===*====*===*====*===*====*===*====*===*====*===*====*===*====

ostream& operator<<(ostream& os, const unrecognized_option_exception& ex)
{
    ex.insert(os);
    return os;
}