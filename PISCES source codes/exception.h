//--------------exception.h----------------------------------------------------------------
//
//   Author:  Phil Pfeiffer
//   Date:    August 1999
//   Last modified:  April 2020
//
//   Exception classes, as defined here, contain three strings:
//   -.  a message that names the routine in which the exception occurred
//   -.  a message that describes a short description of the problem
//   -.  a message that gives a detailed description of the problem
//
//   The  operator()  [compose message]  method is overloaded,
//   to allow derived classes to format error information in their own way.
//---------------------------------------------------------------------------------------

#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <string>

#ifndef PLATFORM_H
#include "platform.h"
#endif

#ifndef STD_H
#include "std.h"
#endif

// ****************************************************************************
//  basic_exception class
// ****************************************************************************

// =*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
//  basic_exception class proper
// =*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*

class basic_exception
{
public:
    //  ==============================
    //   constructors
    //  ==============================
    //
    //  first form:
    //     create exception with specified
    //        routine name, brief explanation, long explanation.
    //
    //  second form:
    //     like first, but appends information from
    //        (previously thrown) exception to long explanation.
    //
    //  third form:
    //     copy constructor
    //
    basic_exception(
        const string & = "",
        const string & = "",
        const string & = ""
    );
    basic_exception(
        const string&,
        const string&,
        const string&,
        const basic_exception&
    );
    basic_exception(const basic_exception&);

    //  ==============================
    //   assignment operator
    //  ==============================
    //
    //  first form:   copies exception objects
    //
    basic_exception& operator=(const basic_exception&);

    //  ==============================
    //   inspectors
    //  ==============================

    //  get_routine_name() -          gets routine that threw exception
    //  get_short_description() -     gets short description of problem
    //  get_detailed_description() -  gets problem detailed message

    void get_routine_name(string&) const;
    void get_short_description(string&) const;
    void get_detailed_description(string&) const;

    //  ==============================
    //   manipulators
    //  ==============================
    //
    //  set_routine_name() -          sets routine that threw exception
    //  set_short_description() -     sets short description of problem
    //  set_detailed_description() -  sets problem detailed message

    void set_routine_name(const string & = "");
    void set_short_description(const string & = "");
    void set_detailed_description(const string & = "");

    //  ==============================
    //   facilitators
    //  ==============================
    //
    //  insert:   inserts contents of exception object into specified stream

    virtual void insert(ostream&) const;

    //=====================================
    //  destructor
    //=====================================
    //
    virtual ~basic_exception(void);

protected:
    //=======================================
    //  member data
    //=======================================
    //
    // parts of message associated with the exception

    string routine_name_;
    string short_description_;
    string detailed_description_;
};

// =*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
//  basic_exception auxiliary operators
// =*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*

ostream& operator<<(ostream&, const basic_exception&);

#endif
