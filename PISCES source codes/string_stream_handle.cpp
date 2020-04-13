//--------------string_stream_handle.cpp--------------------------------------------
//
//   Author:  Phil Pfeiffer, July 1999
//
//   Purpose:  defines a **handle class** for string streams-- a class that -
//   -.   contains a single string stream object
//   -.   supports methods for manipulating the object that it contains
//   -.   (automatically) deletes the strstream object it contains
//        when that object is no longer referenced.
//-----------------------------------------------------------------------------------

#include "string_stream_handle.h"

#include <iostream>
#include <iomanip>          // need to access ios::beg for strstream copying

#ifndef PLATFORM_H
#include "platform.h"       // first: which platform are we using?
#endif

#ifndef STD_H
#include "std.h"
#endif

#ifndef SOCKET_APIS_H
#include "socket_apis.h"   // **needed for IO transparency,
#endif                       // hopefully an STL replacement here will improve this

//=======================================================
//  constructors
//=======================================================

//---------------------------------------------------------------
// constructor, first form - standard class constructor
//---------------------------------------------------------------
//
strstream_handle_class::strstream_handle_class(void)
    : p_string_stream_(new strstream)
{
}

//---------------------------------------------------------------
// constructor, second form - copy constructor
//---------------------------------------------------------------
//
strstream_handle_class::strstream_handle_class(const strstream_handle_class& source_string_stream)
    : p_string_stream_(new strstream)
{
    strstream_handle_copy(source_string_stream);
}

//=======================================================
//  assignment operators
//=======================================================

//---------------------------------------------------------------
// standard object copy
//---------------------------------------------------------------
//
strstream_handle_class& strstream_handle_class::operator=(const strstream_handle_class& source_object)
{
    //  transfer data from original to current, if the two are distinct objects
    //
    if ((&source_object) != this) strstream_handle_copy(source_object);
    return *this;
}

//=======================================================
//  inspectors
//=======================================================

//---------------------------------------------------------------
// operator * - return reference to internal string stream object
//---------------------------------------------------------------
//
strstream& strstream_handle_class::operator * (void)
{
    return *p_string_stream_;
}

//---------------------------------------------------------------
// operator -> - pointer to internal string stream object
//---------------------------------------------------------------
//
strstream* strstream_handle_class::operator -> (void)
{
    return p_string_stream_;
}

//=======================================================
//  destructor
//=======================================================

//---------------------------------------------------------------
//   class's key method--delete dynamically allocated strstream buffer
//---------------------------------------------------------------
//
strstream_handle_class::~strstream_handle_class(void)
{
    //    delete internal string stream buffer, then delete string stream per se
    //
    //    the code below, which ought to work, caused problems in VC++ 6.0
    //       delete p_string_stream_->str();
    //       delete p_string_stream_;
    //
    //    the following code, according to Bruce Eckel,
    //        (cf. http://piranha.inria.fr/~thiriet/Langages/tic_c.html)
    //    should also free the space allocated by insertions into *pStrstream
    //
    p_string_stream_->rdbuf()->freeze(0);
    delete p_string_stream_;
}

//========================================================
//  private member functions
//========================================================

//---------------------------------------------------------------
//  strstream_handle_copy() -
//      copy data from string stream inside specified source handle object
//      into string stream inside current handle object
//---------------------------------------------------------------
//
void strstream_handle_class::strstream_handle_copy(const strstream_handle_class& source_object)
{
    // === const cast alert
    //
    // problem:
    //    module leaves source string stream unchanged (logically const),
    //    but must manipulate internal stream pointers to copy source (i.e., not physically const)
    // workaround:
    //    eliminate compiler errors by creating auxiliary reference to source,
    //    and usint auxiliary reference to manipulate stream

    strstream_handle_class& source_ref = const_cast<strstream_handle_class&>(source_object);

    // **** set up for copy ****
    //
    // preserve current state of source string stream.
    // will restore this source state after copy is completed.

    const format_flags initial_source_flags = source_ref->flags();
    const long long initial_source_read_position = source_ref->tellg();
    const long long initial_source_write_position = source_ref->tellp();

    // **** do the copy ***

    source_ref->clear();              // clear error flags in source stream
    source_ref->seekg(0, ios::beg);   // move to start of stream to copy

    while (source_ref->good())
    {
        int c = source_ref->peek();    // get next char from source stream

        if (c < 0) break;     // error; stop transfer

        // add next char to *this's strstream
        (*p_string_stream_) << static_cast<char>(c);

        // bypass next character in src
        source_ref->ignore();
    }

    //  *** reset source stream's starting position, mode ***
    //
    source_ref->flags(initial_source_flags);          // reset flags
    source_ref->seekg(initial_source_read_position);   // reset input pos'n
    source_ref->seekp(initial_source_write_position);  // reset output pos'n

    //  *** give string stream in current object same mode, pos'n as source ***
    //
    p_string_stream_->flags(initial_source_flags);            // set flags
    p_string_stream_->seekg(initial_source_read_position);     // set input
    p_string_stream_->seekp(initial_source_write_position);    // set output
}