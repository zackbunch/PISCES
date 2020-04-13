//--------------------query.h----------------------------------------------
//
//  Author:  Phil Pfeiffer
//  Date:    August 1999
//  Last modified:  April 2020
//
//  Base class for supporting interactive querying of users for various
//  contents, via question/response stream pair.
//
//  Methods:
//
//   -.  yes_unless_n - query for yes/no answer, defaulting to TRUE ("Y")
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
//------------------------------------------------------------------------

#ifndef QUERY_H
#define QUERY_H

#include <iostream>
#include <set>
#include <string>

#ifndef PLATFORM_H
#include "platform.h"
#endif

#ifndef STD_H
#include "std.h"
#endif

#ifndef INTERVAL_H
#include "interval.h"         // interval class--for range queries
#endif

class query_class
{
public:
    //=======================================
    //  static data
    //=======================================
    //
    //  common delimeters for queries

    static set< char > whitespace_;
    static set< char > newline_;

    //=======================================
    //  constructors
    //=======================================
    //
    //  first form--
    //         sets response, query, feedback streams as specified
    //
    //  second form-- copy constructor
    //
    query_class(istream & = cin, ostream & = cout, ostream & = cout);
    query_class(query_class&);

    //===============================================
    //   manipulators
    //===============================================
    //
    //  query for yes/no answer, defaulting to "Y"

    int yes_unless_n(const string& query);

    // get possibly empty string up to delimiter
    //
    // third argument has list of valid delimiters

    void get_string(const string&, string&, set<char> & = whitespace_);

    // get unsigned from cin.  default: no limit on range of input.
    //
    unsigned get_unsigned(
        const string&,
        const interval_template<unsigned> & = interval_template<unsigned>(0, 0, intervalEndpointType::UNBOUNDED_ENDPOINT, intervalEndpointType::UNBOUNDED_ENDPOINT)
    );

    // get integer from cin.  default: no limit on range of input.
    //
    int get_integer(
        const string&,
        const interval_template<int> & = interval_template<int>(0, 0, intervalEndpointType::UNBOUNDED_ENDPOINT, intervalEndpointType::UNBOUNDED_ENDPOINT)
    );

    //===============================================
    //  destructor
    //===============================================

    virtual ~query_class(void);

protected:
    //========================================
    //   protected member data
    //========================================

    //  response, query, feedback streams
    //
    istream& response_stream_;
    ostream& query_stream_;
    ostream& feedback_stream_;

private:
    //==================================================================
    //   private methods
    //==================================================================

    //  get_one_char -
    //      get one char from response_stream,
    //      throwing exception if error occurred
    //
    //  get_one_int -
    //      get one int from response_stream,
    //      throwing exception if error occurred
    //
    //  display_and_flush_response_stream -
    //      display to end of stream, flushing buffer in process.

    char get_one_char(const string& caller_name);
    int  get_one_int(const string& caller_name);
    void display_and_flush_response_stream(void);
};

#endif
