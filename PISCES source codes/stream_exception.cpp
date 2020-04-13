// -------stream_exception.cpp----------------------------------------------------------
//
//   Author:   Phil Pfeiffer
//   Date:     August 1999
//   Last modified:  April 2020
//
//   Classes that define stream exceptions as an alternative to stream status codes.
//   Key exceptions defined:
//   -.  instream_io_error_exception()
//   -.  instream_format_exception()
//   -.  instream_eof_exception()
//   -.  outstream_exception()
// --------------------------------------------------------------------------------------

#include "stream_exception.h"

#include <iomanip>
#include <string>
#include <sstream>   // stringstream
#include "ctype.h"   // for isprint()--test if val is printable ASCII code

#ifndef STD_H
#include "std.h"
#endif

// ***************************************************************
// classes that define stream i/o error conditions
//
//   comments are omitted because the code is near-boilerplate.
//   key difference between code below and code in basic exception class:
//       diagnostic message composition method [ operator() ]
//       also returns information on stream state
// ***************************************************************

// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=
// zero-order stream exceptions: instream_exception, outstream_exception
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

// *===*====*===*====*===*====*===*====*===*====*===*====*===*====*===*====
// exceptions proper
// *===*====*===*====*===*====*===*====*===*====*===*====*===*====*===*====

instream_exception::instream_exception(
    const string& name,
    const string& short_description,
    const string& detailed_description,
    const basic_exception& prior_exception
)
    : basic_exception(name, short_description, detailed_description, prior_exception) {}

instream_exception::instream_exception(const instream_exception& source)
    : basic_exception(source) {}

instream_exception::~instream_exception(void) {}

outstream_exception::outstream_exception(
    const string& name,
    const string& short_description,
    const string& detailed_description,
    const basic_exception& prior_exception
)
    : basic_exception(name, short_description, detailed_description, prior_exception) {}

outstream_exception::outstream_exception(const outstream_exception& source)
    : basic_exception(source) {}

outstream_exception::~outstream_exception(void) {}

// *===*====*===*====*===*====*===*====*===*====*===*====*===*====*===*====
// auxiliary operators
// *===*====*===*====*===*====*===*====*===*====*===*====*===*====*===*====

ostream& operator<<(ostream& os, const  instream_exception& ex)
{
    ex.insert(os);
    return os;
}

ostream& operator<<(ostream& os, const outstream_exception& ex)
{
    ex.insert(os);
    return os;
}

// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=
// first-order stream exceptions:
//       instream_processing_exception, outstream_processing_exception
//
// these exceptions add stream position data to their diagnostic messages
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

// *===*===*===*===*===*===*===*===*===*===*===*===*===*===*===*===*===*===
//   instream_processing_exception --
//       instream exception w. support for error position diagnostics
// *===*===*===*===*===*===*===*===*===*===*===*===*===*===*===*===*===*===

//  ==============================
//   constructors
//  ==============================

//------------------------------------------------------------------
//  first form of constructor -
//      set object to contain specified routine name, etc.
//------------------------------------------------------------------
//
instream_processing_exception::instream_processing_exception(
    const istream& is,
    const string& name,
    const string& short_description,
    const string& detailed_description,
    const basic_exception& prior_exception
)
    : instream_exception(name, short_description, detailed_description, prior_exception)
{
    // ==== const cast alert ====
    //
    // this function is logically const, even though methods being called--
    // peek(), tellg()-- aren't declared as const
    //
    // use casted reference to is to defeat compiler messages about
    // failure to keep this object const

    istream& istream_reference = const_cast<istream&>(is);

    // ==== recover details of stream state for error message ====
    //
    // must temporarily clear state to recover state data

    state_ = istream_reference.rdstate();

    istream_reference.clear();               // without clear(), tellg() returns -1
    failure_position_ = istream_reference.tellg();
    next_value_ = istream_reference.peek();

    istream_reference.clear(state_);       // reset state variable to incoming value
}

//------------------------------------------------------------------
//  second form of constructor -  copy constructor
//------------------------------------------------------------------
//
instream_processing_exception::instream_processing_exception(const instream_processing_exception& source)
    : instream_exception(source),
    failure_position_(source.failure_position_),
    next_value_(source.next_value_),
    state_(source.state_)
{
}

//  ==============================
//   facilitators
//  ==============================

//------------------------------------------------------------------
//  insert() --
//    inserts string into stream, capturing exception properties, string state
//------------------------------------------------------------------
//
void instream_processing_exception::insert(ostream& os) const
{
    //  ** start with message computed by basic_exception's version of insert() **
    //
    basic_exception::insert(os);

    //  ** add information pertinent to stream state at time of exception **
    //
    stringstream p_stream_details;

    p_stream_details << "error occurred at stream position " << failure_position_ << endl;

    if (next_value_ >= 0)   //  == then: next value, if any
    {
        p_stream_details << "next value in stream was " << next_value_;

        if (isprint(next_value_))
        {
            p_stream_details << " (ASCII " << static_cast<char>(next_value_) << ")";
        }
        else if (next_value_ == '\n')
        {
            p_stream_details << " (ASCII \\n) ";
        }
        else if (next_value_ == '\t')
        {
            p_stream_details << " (ASCII \\t) ";
        }

        p_stream_details << endl;
    }

    //  == finally: error flags, if any active
    //
    p_stream_details << "active error flags: ";

    unsigned active_flag_count = 0;
    if (state_ & ios::failbit)
    {
        p_stream_details << "format error";
        active_flag_count++;
    }
    if (state_ & ios::badbit)
    {
        if (active_flag_count > 0) p_stream_details << ", ";
        p_stream_details << "i/o error";
        active_flag_count++;
    }
    if (state_ & ios::eofbit)
    {
        if (active_flag_count > 0) p_stream_details << ", ";
        p_stream_details << "end of file";
        active_flag_count++;
    }
    if (active_flag_count == 0) p_stream_details << "<no error bits active>";

    //  ** write balance of generated message **
    //
    // ignore << errors (what else can we do?)

    os << "\n" << p_stream_details.str();
}

//=====================================
//  destructor
//=====================================
//
instream_processing_exception::~instream_processing_exception(void)
{
}

// *===*===*===*===*===*===*===*===*===*===*===*===*===*===*===*===*===*===
//    outstream_processing_exception --
//       outstream exception w. support for error position diagnostics
// *===*===*===*===*===*===*===*===*===*===*===*===*===*===*===*===*===*===

//  ==============================
//   constructors
//  ==============================

//------------------------------------------------------------------
//  first form of constructor -
//      set object to contain specified routine name, etc.
//------------------------------------------------------------------
//
outstream_processing_exception::
outstream_processing_exception(
    const ostream& os,
    const string& name,
    const string& short_description,
    const string& detailed_description,
    const basic_exception& prior_exception
)
    : outstream_exception(name, short_description, detailed_description, prior_exception)
{
    // ==== const cast alert ====
    //
    // this function is logically const, even though methods being called--
    // tellp()-- aren't declared as const
    //
    // use casted reference to is to defeat compiler messages about
    // failure to keep this object const

    ostream& ostream_reference = const_cast<ostream&>(os);

    // also recover details of stream state for error message
    //
    failure_position_ = ostream_reference.tellp();
}

//------------------------------------------------------------------
//  second form of constructor -  copy constructor
//------------------------------------------------------------------
//
outstream_processing_exception::outstream_processing_exception(const outstream_processing_exception& source)
    : outstream_exception(source),
    failure_position_(source.failure_position_)
{
}

//  ==============================
//   facilitators
//  ==============================

//------------------------------------------------------------------
//  insert() --
//    inserts string into stream, capturing exception properties, string state
//------------------------------------------------------------------
//
void outstream_processing_exception::insert(ostream& os) const
{
    // ** start with message computed by basic_exception's version of insert() **
    //
    basic_exception::insert(os);

    //  add information pertinent to stream state at time of exception.
    //
    stringstream p_stream_details;
    p_stream_details << "error occurred at stream position " << failure_position_;

    //  ** write balance of generated message **
    //
    // ignore << errors (what else can we do?)

    os << "\n" << p_stream_details.str();
}

//=====================================
//  destructor
//=====================================
//
outstream_processing_exception::~outstream_processing_exception(void)
{
}

// *===*====*===*====*===*====*===*====*===*====*===*====*===*====*===*====
// auxiliary operators
// *===*====*===*====*===*====*===*====*===*====*===*====*===*====*===*====

ostream& operator<<(ostream& os, const  instream_processing_exception& ex)
{
    ex.insert(os);
    return os;
}

ostream& operator<<(ostream& os, const outstream_processing_exception& ex)
{
    ex.insert(os);
    return os;
}

// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=
// second-order stream exceptions:  specific errors  (boilerplate code)
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=
//
//  instream_open_exception:      can't open instream
//  instream_format_exception:    (parsed) stream has unexpected format
//  instream_eof_exception:       EOF encountered when read attempted on stream
//  instream_io_error_exception:   bad() flag set by attempted read
//
//  outstream_open_exception:     can't open outstream
//  outstream_io_error_exception:  bad() flag set by attempted write

instream_open_exception::
instream_open_exception(
    const string& name,
    const string& short_description,
    const string& detailed_description,
    const basic_exception& prior_exception
)
    : instream_exception(name, short_description, detailed_description, prior_exception) {}

instream_open_exception::instream_open_exception(const instream_open_exception& source)
    : instream_exception(source) {}

instream_open_exception::~instream_open_exception(void) {}

instream_format_exception::
instream_format_exception(
    const istream& is,
    const string& name,
    const string& short_description,
    const string& detailed_description,
    const basic_exception& prior_exception
)
    : instream_processing_exception(is, name, short_description, detailed_description, prior_exception) {}

instream_format_exception::instream_format_exception(const instream_format_exception& source)
    : instream_processing_exception(source) {}

instream_format_exception::~instream_format_exception(void) {}

instream_eof_exception::
instream_eof_exception(
    const istream& is,
    const string& name,
    const string& short_description,
    const string& detailed_description,
    const basic_exception& prior_exception
)
    : instream_processing_exception(is, name, short_description, detailed_description, prior_exception) {}

instream_eof_exception::instream_eof_exception(const instream_eof_exception& source)
    : instream_processing_exception(source) {}

instream_eof_exception::~instream_eof_exception(void) {}

instream_io_error_exception::
instream_io_error_exception(
    const istream& is,
    const string& name,
    const string& short_description,
    const string& detailed_description,
    const basic_exception& prior_exception
)
    : instream_processing_exception(is, name, short_description, detailed_description, prior_exception) {}

instream_io_error_exception::instream_io_error_exception(const instream_io_error_exception& source)
    : instream_processing_exception(source) {}

instream_io_error_exception::~instream_io_error_exception(void) {}

outstream_open_exception::
outstream_open_exception(
    const string& name,
    const string& short_description,
    const string& detailed_description,
    const basic_exception& prior_exception
)
    : outstream_exception(name, short_description, detailed_description, prior_exception) {}

outstream_open_exception::outstream_open_exception(const outstream_open_exception& source)
    : outstream_exception(source) {}

outstream_open_exception::~outstream_open_exception(void) {}

outstream_io_error_exception::
outstream_io_error_exception(
    const ostream& os,
    const string& name,
    const string& short_description,
    const string& detailed_description,
    const basic_exception& prior_exception
)
    : outstream_processing_exception(os, name, short_description, detailed_description, prior_exception) {}

outstream_io_error_exception::outstream_io_error_exception(const outstream_io_error_exception& source)
    : outstream_processing_exception(source) {}

outstream_io_error_exception::~outstream_io_error_exception(void) {}

// *===*====*===*====*===*====*===*====*===*====*===*====*===*====*===*====
// auxiliary operators  [boilerplate]
// *===*====*===*====*===*====*===*====*===*====*===*====*===*====*===*====

ostream& operator<<(ostream& os, const instream_open_exception& ex)
{
    ex.insert(os);
    return os;
}

ostream& operator<<(ostream& os, const instream_format_exception& ex)
{
    ex.insert(os);
    return os;
}

ostream& operator<<(ostream& os, const instream_eof_exception& ex)
{
    ex.insert(os);
    return os;
}

ostream& operator<<(ostream& os, const instream_io_error_exception& ex)
{
    ex.insert(os);
    return os;
}

ostream& operator<<(ostream& os, const outstream_open_exception& ex)
{
    ex.insert(os);
    return os;
}

ostream& operator<<(ostream& os, const outstream_io_error_exception& ex)
{
    ex.insert(os);
    return os;
}