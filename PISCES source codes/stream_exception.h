// ---------stream_exception.h------------------------------------------------------------
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
// ---------------------------------------------------------------------------------------

#ifndef STREAM_EXCEPTION_H
#define STREAM_EXCEPTION_H

#include <iostream>
#include <string>

#ifndef PLATFORM_H
#include "platform.h"
#endif

#ifndef STD_H
#include "std.h"
#endif

#ifndef EXCEPTION_H
#include "exception.h"   // exception management
#endif

#ifndef SOCKET_APIS_H
#include "socket_apis.h"
#endif

// **********************************************************************
// classes that define stream i/o error conditions
// **********************************************************************
//
// create related exceptions in this module:
// -.  an overall class to serve as base class for all istream exceptions
// -.  a set of specific istream exceptions
// -.  an overall class to serve as base class for all ostream exceptions
// -.  a set of specific ostream exceptions
//
// code here is like code in base exception module, with two exceptions:
// -.  constructors use stream references to recover stream status data
// -.  operator() adds stream status data to composed messages

// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=
// zero-order stream exceptions: instream_exception, outstream_exception
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

// *===*====*===*====*===*====*===*====*===*====*===*====*===*====*===*====
// exceptions proper
// *===*====*===*====*===*====*===*====*===*====*===*====*===*====*===*====

class instream_exception : public basic_exception
{
public:
    instream_exception(
        const string & = "", const string & = "", const string & = "",
        const basic_exception & = basic_exception()
    );
    instream_exception(const instream_exception& source);
    virtual ~instream_exception(void);
};

class outstream_exception : public basic_exception
{
public:
    outstream_exception(
        const string & = "", const string & = "", const string & = "",
        const basic_exception & = basic_exception()
    );
    outstream_exception(const outstream_exception& source);
    virtual ~outstream_exception(void);
};

// *===*====*===*====*===*====*===*====*===*====*===*====*===*====*===*====
// auxiliary operators
// *===*====*===*====*===*====*===*====*===*====*===*====*===*====*===*====

ostream& operator<<(ostream&, const  instream_exception&);
ostream& operator<<(ostream&, const outstream_exception&);

// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=
// first-order stream exceptions:
//       instream_processing_exception, outstream_processing_exception
//
// these exceptions add stream position data to their diagnostic messages
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

// *===*====*===*====*===*====*===*====*===*====*===*====*===*====*===*====
// exceptions proper
// *===*====*===*====*===*====*===*====*===*====*===*====*===*====*===*====

class instream_processing_exception : public instream_exception
{
public:
    //  ==============================
    //   constructors
    //  ==============================
    //
    //  first form:
    //     create exception with
    //        stream state data from istream
    //        specified routine name, brief explanation, long explanation
    //
    //  second form:  copy constructor

    instream_processing_exception(
        const istream&,
        const string & = "",
        const string & = "",
        const string & = "",
        const basic_exception & = basic_exception()
    );
    instream_processing_exception(const instream_processing_exception&);

    //  ==============================
    //   facilitators
    //  ==============================
    //
    //  insert() -  write specified string to stream

    void insert(ostream&) const;   // compose description of problem

    //=====================================
    //  destructor
    //=====================================

    virtual ~instream_processing_exception(void);

private:
    //=======================================
    //  member data
    //=======================================
    //
    // additional stream state reported with this kind of exception

    long long int failure_position_;    // point in stream for failure
    int next_value_;                    // value in stream at failure point
    io_state_type state_;                // stream state at point of failure
};

class outstream_processing_exception : public outstream_exception
{
public:
    //  ==============================
    //   constructors
    //  ==============================
    //
    //  first form:
    //     create exception with
    //        stream state data from istream
    //        specified routine name, brief explanation, long explanation
    //
    //  second form:  copy constructor

    outstream_processing_exception(
        const ostream&,
        const string & = "",
        const string & = "",
        const string & = "",
        const basic_exception & = basic_exception()
    );
    outstream_processing_exception(const outstream_processing_exception&);

    //  ==============================
    //   facilitators
    //  ==============================
    //
    //  insert() -  write specified string to stream

    void insert(ostream&) const;   // compose description of problem

    //=====================================
    //  destructor
    //=====================================

    virtual ~outstream_processing_exception(void);

private:
    //=======================================
    //  member data
    //=======================================
    //
    // additional stream state reported with this kind of exception

    long long int failure_position_;     // point in stream for failure
};

// *===*====*===*====*===*====*===*====*===*====*===*====*===*====*===*====
// auxiliary operators
// *===*====*===*====*===*====*===*====*===*====*===*====*===*====*===*====

ostream& operator<<(ostream&, const  instream_processing_exception&);
ostream& operator<<(ostream&, const outstream_processing_exception&);

// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=
// second-order stream exceptions:  specific errors
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

//  instream_open_exception:      can't open instream
//  instream_format_exception:    (parsed) stream has unexpected format
//  instream_eof_exception:       EOF encountered when read attempted on stream
//  instream_io_error_exception:   bad() flag set by attempted read
//
//  outstream_open_exception:     can't open outstream
//  outstream_io_error_exception:  bad() flag set by attempted write

// *===*====*===*====*===*====*===*====*===*====*===*====*===*====*===*====
// exceptions proper
// *===*====*===*====*===*====*===*====*===*====*===*====*===*====*===*====

class instream_open_exception : public instream_exception
{
public:
    instream_open_exception(
        const string & = "", const string & = "", const string & = "",
        const basic_exception & = basic_exception()
    );
    instream_open_exception(const instream_open_exception& source);
    virtual ~instream_open_exception(void);
};

class instream_format_exception : public instream_processing_exception
{
public:
    instream_format_exception(
        const istream&,
        const string & = "", const string & = "", const string & = "",
        const basic_exception & = basic_exception()
    );
    instream_format_exception(const instream_format_exception& source);
    virtual ~instream_format_exception(void);
};

class instream_eof_exception : public instream_processing_exception
{
public:
    instream_eof_exception(
        const istream&,
        const string & = "", const string & = "", const string & = "",
        const basic_exception & = basic_exception()
    );
    instream_eof_exception(const instream_eof_exception& source);
    virtual ~instream_eof_exception(void);
};

class instream_io_error_exception : public instream_processing_exception
{
public:
    instream_io_error_exception(
        const istream&,
        const string & = "", const string & = "", const string & = "",
        const basic_exception & = basic_exception()
    );
    instream_io_error_exception(const instream_io_error_exception& source);
    virtual ~instream_io_error_exception(void);
};

class outstream_open_exception : public outstream_exception
{
public:
    outstream_open_exception(
        const string & = "", const string & = "", const string & = "",
        const basic_exception & = basic_exception()
    );
    outstream_open_exception(const outstream_open_exception& source);
    virtual ~outstream_open_exception(void);
};

class outstream_io_error_exception : public outstream_processing_exception
{
public:
    outstream_io_error_exception(
        const ostream&,
        const string & = "", const string & = "", const string & = "",
        const basic_exception & = basic_exception()
    );
    outstream_io_error_exception(const outstream_io_error_exception& source);
    virtual ~outstream_io_error_exception(void);
};

// *===*====*===*====*===*====*===*====*===*====*===*====*===*====*===*====
// auxiliary operators
// *===*====*===*====*===*====*===*====*===*====*===*====*===*====*===*====

ostream& operator<<(ostream&, const instream_open_exception&);
ostream& operator<<(ostream&, const instream_format_exception&);
ostream& operator<<(ostream&, const instream_eof_exception&);
ostream& operator<<(ostream&, const instream_io_error_exception&);
ostream& operator<<(ostream&, const outstream_open_exception&);
ostream& operator<<(ostream&, const outstream_io_error_exception&);

#endif
