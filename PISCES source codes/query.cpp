//--------------------query.cpp---------------------------------------------------
//
//  Author:  Phil Pfeiffer
//  Date:    August 1999
//  Last modified:  April 2020
//
//  Base class for supporting interactive querying of users for various
//  content, via question/response stream pair.
//
//  Methods:
//
//   -.  yes_unless_n -
//           query for yes/no answer, defaulting to "Y"
//   -.  get_string -
//            get possibly empty, whitespace-free string from input stream,
//            up to tab, blank, or newline.
//   -.  get_stringUptoNewline -
//            get possibly empty string from cin, up to newline.
//   -.  get_unsigned -
//            get unsigned value from input stream,
//            within optional user-specified interval
//   -.  get_integer -
//            get integer value from input stream,
//            within optional user-specified interval
//---------------------------------------------------------------------------------

#include "query.h"

#ifndef BOOL_H
#include "bool.h"
#endif

#include <sstream>   // stringstream

#ifndef STREAM_EXCEPTION_H
#include "stream_exception.h"        // stream exceptions
#endif

#ifndef INTERVAL_TPL
#include "interval.tpl"        // intervals
#endif

#include "ctype.h"      // isprint()

// =====================
//  static member data
// =====================
//
// *** all structures below are initialized by class's constructor ***

// two sets of commonly used delimiters for string queries

set< char > query_class::whitespace_;
set< char > query_class::newline_;

//=======================================
//  constructors
//=======================================

//---------------------------------------------------------------------
//  first  form-- sets istream, ostream to user-specified streams
//---------------------------------------------------------------------
//
query_class::query_class(istream& response_stream, ostream& query_stream, ostream& feedback_stream)
    : response_stream_(response_stream),
    query_stream_(query_stream),
    feedback_stream_(feedback_stream)
{
    // **** ensure that following code executes once ***

    static unsigned constructor_has_been_called = FALSE;

    if (constructor_has_been_called) return;
    constructor_has_been_called = TRUE;

    // ****** two static supporting data structures to initialize *****

    whitespace_.insert(' ');
    whitespace_.insert('\t');
    whitespace_.insert('\n');

    newline_.insert('\n');
}

//---------------------------------------------------------------------
//  second form-- copy constructor
//---------------------------------------------------------------------
//
query_class::query_class(query_class& source)
    : response_stream_(source.response_stream_),
    query_stream_(source.query_stream_),
    feedback_stream_(source.feedback_stream_)
{
}

//===============================================
//   manipulators
//===============================================

// --------------------------------------------------------------------------
// yes_unless_n() -
//   query user for yes/no answer, using "Y" if 'N' or 'n' not entered.
//   throw exception if input stream fails or is exhausted during query.
// --------------------------------------------------------------------------
//
int query_class::yes_unless_n(const string& message)
{
    // **** set up static const data ****
    //
    // -.  number of characters max between line breaks
    // -.  number of characters to leave at right-hand margin

    static const unsigned LINE_LENGTH = 80;
    static const unsigned LINE_MARGIN = 5;

    static unsigned useable_line = LINE_LENGTH - LINE_MARGIN;

    static const string my_name = "query_class::yes_unless_n";
    static const string input_problem = "can't read from response stream";
    static const string output_problem = "can't write to query stream";

    static const string query_trailer = "? (N or n for no, any other char for yes):  ";

    // **** display user-supplied message, followed by query ****

    //  ==== display user's message, followed by the query itself ===
    //
    //  be careful not to overrun current line.
    //  also, check for output errors.
    //
    if (!(query_stream_ << message))
        throw outstream_io_error_exception(query_stream_, my_name, output_problem);

    if ((message.length() + query_trailer.length()) > useable_line)
        if (!(query_stream_ << endl << "   "))
            throw outstream_io_error_exception(query_stream_, my_name, output_problem);

    if (!(query_stream_ << query_trailer << flush))
        throw outstream_io_error_exception(query_stream_, my_name, output_problem);

    //  **** get and return user's reply ***
    //
    //  -.  sustain exceptions on input errors.
    //  -.  discard initial '\n', if left in stream from previous input
    //  -.  flush stream up to next '\n' or error, following input

    //  ==== get non-whitespace char, then consume chars to EOL ===
    //
    char user_answer;
    do
    {
        user_answer = get_one_char(my_name);
    } while (user_answer == ' ' || user_answer == '\t' || user_answer == '\n');

    int c;
    do
    {
        c = response_stream_.peek();
        if (c < 0)
        {
            response_stream_.clear();
            break;
        }
        response_stream_.ignore();
    } while (static_cast<char>(c) != '\n');

    //  **** return response ****

    return toupper(user_answer) != 'N';
}

// -------------------------------------------------------------------------------
// get_string() -
//
//    query user for string, returning string obtained as second parameter.
//    read up until char discovered falls in delimiter set, or failure.
//    throw exception if input stream fails or is exhausted during query.
// -------------------------------------------------------------------------------
//
void query_class::get_string(const string& message, string& result, set<char>& delimiter_set)
{
    // **** set up static const data ****

    static const string my_name = "query_class::get_string";
    static const string input_problem = "can't read from response stream";
    static const string query_problem = "can't write to query stream";

    // **** initialize processing ****

    //  see if whitespace ignored on entry  (for restoring on way out)
    //
    int whitespace_ignored_on_entry = !(response_stream_.flags() & ios::skipws);

    //  make whitespace significant by turning off skip bit
    //
    response_stream_.flags(response_stream_.flags() & (~ios::skipws));

    result = "";    //  clear result string (no input from user, yet)

    //  *** get string, resetting flags state on exit of routine ***

    try
    {
        // === display caller's message checking for errors ===
        //
        if (!(query_stream_ << message << ": " << flush))
            throw outstream_io_error_exception(query_stream_, my_name, query_problem);

        // === discard newline that ended last input, if still there ===

        if (response_stream_.peek() == '\n') response_stream_.ignore();

        // === acquire one character at a time, until delimiter ===

        while (TRUE)   // **** loop exits in middle ****
        {
            // get one char, throwing exception on stream failure
            //
            char c = get_one_char(my_name);

            // test for end of string by looking for c in delimiters
            //
            set<char>::iterator it = delimiter_set.find(c);
            if (it != delimiter_set.end()) break;

            // string continues--add char to end of string, continue
            //
            result += c;
        }

        // *** restore state of whitespace flag on way out ***

        if (whitespace_ignored_on_entry)
            response_stream_.flags(response_stream_.flags() | ios::skipws);
        else
            response_stream_.flags(response_stream_.flags() & (~ios::skipws));
    }
    catch (...)
    {
        // *** restore state of whitespace flag on way out ***

        if (whitespace_ignored_on_entry)
            response_stream_.flags(response_stream_.flags() | ios::skipws);
        else
            response_stream_.flags(response_stream_.flags() & (~ios::skipws));

        throw;    // rethrow last exception
    }
}

// --------------------------------------------------------------------------
// get_unsigned() -
//
//    routine for querying user for unsigned number within an interval.
//    throw exception if input stream fails or is exhausted during query.
// --------------------------------------------------------------------------
//
unsigned query_class::get_unsigned(const string& query, const interval_template<unsigned>& range)
{
    // **** set up static const data ****

    static const string my_name = "query_class::get_string";
    static const string input_problem = "can't read from response stream";
    static const string query_problem = "can't write to query stream";
    static const string feedback_problem = "can't write to feedback stream";

    static const string bad_interval = "?? value not in interval ";

    // **** get value from user ****

    int result;

    while (TRUE)     // *** single exit loop that exits in middle ***
    {
        if (!(query_stream_ << query << ": " << flush))
            throw outstream_io_error_exception(query_stream_, my_name, query_problem);

        try
        {
            result = get_one_int(my_name);
            if (result >= 0 && range.is_in_interval(result) == TRUE) break;

            if (!(feedback_stream_ << bad_interval << range << endl))
                throw outstream_io_error_exception(query_stream_, my_name, feedback_problem);
        }
        catch (const instream_format_exception&)
        {
            // on invalid typein, clear, display, and flush instream
            //
            response_stream_.clear();

            feedback_stream_ << endl << "?? nonnumeric input: ";
            display_and_flush_response_stream();
            feedback_stream_ << endl;
        }
    }
    return result;
}

// --------------------------------------------------------------------------
// get_integer() -
//
//    routine for querying user for integer within an interval.
//    throw exception if input stream fails or is exhausted during query.
// --------------------------------------------------------------------------
//
int query_class::get_integer(const string& query, const interval_template<int>& range)
{
    // **** set up static const data ****

    static const string my_name = "query_class::get_string";
    static const string input_problem = "can't read from response stream";
    static const string query_problem = "can't write to query stream";
    static const string feedback_problem = "can't write to feedback stream";

    static const string bad_interval = "?? value not in interval ";

    // **** get value from user ****

    int result;

    while (TRUE)     // *** single exit loop that exits in middle ***
    {
        if (!(query_stream_ << query << ": " << flush))
            throw outstream_io_error_exception(query_stream_, my_name, query_problem);

        try
        {
            result = get_one_int(my_name);
            if (range.is_in_interval(result) == TRUE) break;

            if (!(feedback_stream_ << bad_interval << range << endl))
                throw outstream_io_error_exception(query_stream_, my_name, feedback_problem);
        }
        catch (const instream_format_exception&)
        {
            // on invalid typein, clear, display, and flush instream
            //
            response_stream_.clear();

            feedback_stream_ << endl << "?? nonnumeric input: ";
            display_and_flush_response_stream();
            feedback_stream_ << endl;
        }
    }
    return result;
}

//===============================================
//  destructor
//===============================================

query_class::~query_class(void)
{
}

//=====================================================================
// private methods
//=====================================================================

// --------------------------------------------------------------
//  get_one_char() -
//      get one char from response stream,
//          throwing exception if char not available
// ---------------------------------------------------------------

char query_class::get_one_char(const string& caller_name)
{
    // **** initialize static data ****
    //
    static const string my_problem = "can't get character";

    // **** look for the character ****
    //
    int c = response_stream_.peek();    // get next item from stream

    if (c < 0)
    {
        // *** manage error ***

        //  throw appropriate exception

        if (response_stream_.fail())
            throw instream_format_exception(response_stream_, caller_name, my_problem);
        if (response_stream_.bad())
            throw instream_io_error_exception(response_stream_, caller_name, my_problem);
        if (response_stream_.eof())
            throw instream_eof_exception(response_stream_, caller_name, my_problem);
        throw instream_processing_exception(response_stream_, caller_name, my_problem);
    }

    response_stream_.ignore();      // skip examined item
    return static_cast<char>(c);
}

// --------------------------------------------------------------
//  get_one_int() -
//      get one integer from response stream,
//          throwing exception if char not available
// ---------------------------------------------------------------

int query_class::get_one_int(const string& caller_name)
{
    // **** initialize static data ****
    //
    static const string my_problem = "can't get int";

    // **** initialize processing ****

    //  see if whitespace ignored on entry  (for restoring on way out)
    //
    int whitespace_ignored_on_entry = !(response_stream_.flags() & ios::skipws);

    //  make whitespace insignificant by turning on skip bit
    //
    response_stream_.flags(response_stream_.flags() | ios::skipws);

    // **** read the integer ****
    //
    int result;
    response_stream_ >> result;

    // *** restore original whitespace flag ***
    //
    if (whitespace_ignored_on_entry)
        response_stream_.flags(response_stream_.flags() | ios::skipws);
    else
        response_stream_.flags(response_stream_.flags() & (~ios::skipws));

    // *** check for read errors ****
    //
    if (!response_stream_.good())
    {
        // *** manage error ***

        //  throw appropriate exception

        if (response_stream_.fail())
            throw instream_format_exception(response_stream_, caller_name, my_problem);
        if (response_stream_.bad())
            throw instream_io_error_exception(response_stream_, caller_name, my_problem);
        if (response_stream_.eof())
            throw instream_eof_exception(response_stream_, caller_name, my_problem);
        throw instream_processing_exception(response_stream_, caller_name, my_problem);
    }

    return result;
}

// ---------------------------------------------------------------------------
//  display_and_flush_response_stream() -
//
//     display to end of stream, flushing buffer in process.
//     useful for complaining about spurious user input.
//
//  Note:
//      There's a much simpler way to clear the input stream.
//      If we didn't want to display the buffer's current content,
//            we could use  "cin.seekg(ios::end)"  to flush offending input.
// ---------------------------------------------------------------------------

void query_class::display_and_flush_response_stream(void)
{
    //  Echo what's in the buffer for the user's benefit.
    //  What's going on here:
    //     the rdbuf() member function gets cin's streambuf;
    //     the in_avail() member function returns count of chars in streambuf
    //
    std::streamsize chars_in_response_stream = response_stream_.rdbuf()->in_avail();

    //  since the count includes whitespace characters,
    //      we need to temporarily suppress the "skip whitespace option"
    //      to use cout to present the output
    //
    //  save and temporarily alter the stream mode flags;
    //      will restore old modes on exit.
    //
    format_flags initial_flags = response_stream_.flags();
    response_stream_.flags(initial_flags & (~ios::skipws));

    //  now, display what's in the buffer
    //
    char thisChar;
    for (std::streamsize i = chars_in_response_stream; i > 0; i--)
    {
        // could check for cin and cout errors,
        // but routine is an error recovery routine--why bother?
        //
        response_stream_ >> thisChar;

        if (isprint(thisChar))
            feedback_stream_ << thisChar;
        else
        {
            // two-step logic needed to get values in range from 128..255
            // to print properly
            //
            const unsigned char this_value = static_cast<unsigned char>(thisChar);
            feedback_stream_ << "<" << static_cast<unsigned>(this_value) << ">";
        }
    }

    // restore cin flag statuses that held on entry to routine
    //
    response_stream_.flags(initial_flags);
}