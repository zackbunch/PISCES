//--------------string_stream_handle.h------------------------------------------------------------
//
//   Author:   Phil Pfeiffer
//   Date:     August 1999
//   Last modified:  April 2020
//
//   Defines a **handle class** for string streams-- a class that -
//   -.  contains a single string stream object
//   -.  supports methods for manipulating the object that it contains
//   -.  (automatically) deletes the object it contains when that object is no longer referenced.
//
//-------------------------------------------------------------------------------------------------

#ifndef STRING_STREAM_HANDLE_H
#define STRING_STREAM_HANDLE_H

#include <strstream>

#ifndef PLATFORM_H
#include "platform.h"
#endif

#ifndef STD_H
#include "std.h"
#endif

class strstream_handle_class
{
public:
    //=======================================================
    //  constructors
    //=======================================================
    //
    //  first form - standard class constructor
    //
    //  second form - copy constructor
    //
    strstream_handle_class(void);
    strstream_handle_class(const strstream_handle_class& source);

    //=======================================================
    //  assignment operators
    //=======================================================
    //
    strstream_handle_class& operator=(const strstream_handle_class& source);

    //=======================================================
    //  inspectors
    //=======================================================
    //
    //  return reference to internal string stream object
    //
    strstream& operator *  (void);
    strstream* operator -> (void);

    //=======================================================
    //  destructor
    //=======================================================
    //
    //   class's key method--delete dynamically allocated strstream buffer
    //
    virtual ~strstream_handle_class(void);

private:
    //========================================================
    //  private member data
    //========================================================
    //
    strstream* p_string_stream_;    // the string stream proper

    //========================================================
    //  private member functions
    //========================================================
    //
    //  copy data from specified string stream
    //            into string stream referenced by p_string_stream_
    //
    void strstream_handle_copy(const strstream_handle_class& source);
};

#endif
