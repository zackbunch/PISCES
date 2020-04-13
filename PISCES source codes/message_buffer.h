//--------------------message_buffer.h---------------------------------------------------
//
//   Author:  Phil Pfeiffer
//   Date:    2 September 1999
//   Last modified:  April 2020
//
//   Data buffering classes for socket handlers.
//
//   Classes move basic C++ data into and out of a user-specified
//   buffer a byte at a time, thereby supporting buffer packing
//   and buffer unpacking in a way that does not depend on whether
//   the current host supports data alignment.
//
//   Design for class:
//   -.  base buffering class
//   -.  two derived classes that support fixed and variable-length buffers respectively.
//
//   Limitations:  Byte ordering concerns not addressed by class
//--------------------------------------------------------------------------------------

#ifndef MESSAGE_BUFFER_H
#define MESSAGE_BUFFER_H

#include <string>

#ifndef PLATFORM_H
#include "platform.h"
#endif

#ifndef STD_H
#include "std.h"
#endif

//******************************************************************
// abstract class -- defines form of standard buffering methods
//******************************************************************

//=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//  abstract message buffering class per se
//=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// class defined as abstract class by protecting constructor methods

class message_buffer_interface_class
{
public:
    //==================================================================
    // assignment operator
    //==================================================================
    //
    //  basic object copy

    message_buffer_interface_class& operator=(const message_buffer_interface_class&);

    //==================================================================
    // inspectors
    //==================================================================
    //
    // return pointers to buffer, buffer size for direct manipulation
    //
    char* p_buffer(void);
    unsigned long* p_size(void);

    //==================================================================
    // manipulators
    //==================================================================
    //
    // reposition cursor at 0th location in buffer

    void reset(void);

    //==================================================================
    // destructors
    //==================================================================

    virtual ~message_buffer_interface_class(void);

protected:
    //==================================================================
    // constructors
    //==================================================================
    //
    //  first form --
    //        allocate buffer of specified size,
    //        setting current character pointer to buffer head
    //
    //  second form --  copy constructor

    message_buffer_interface_class(const unsigned long&);
    message_buffer_interface_class(const message_buffer_interface_class&);

    //==================================================================
    // member data
    //==================================================================

    char* p_buffer_;              // buffer proper
    unsigned long buffer_size_;   // size of object pointed to by buffer, in chars
    unsigned long cursor_;        // next position to put/get char

    //==================================================================
    // member functions
    //==================================================================
    //
    // arguments:
    //    pointer to byte vector,
    //    count of bytes to move,
    //    name of type of datum being extracted/inserted (for diagnostics)

    void insert_into_buffer(const char* const, const unsigned long&, const string&);
    void extract_from_buffer(char* const, const unsigned long&, const string&);
};

//*****************************************************************************
// buffering class for fixed-length buffers
//
//  key property of class:
//  buffer size fixed by constructor; won't change after assignment, extraction
//*****************************************************************************

//=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
// fixed-length message buffering class per se
//=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+

class fixed_message_length_buffer_class : public message_buffer_interface_class
{
public:
    //==================================================================
    // constructors
    //==================================================================
    //
    //  first form --
    //        allocate buffer of specified size,
    //        setting current character pointer to buffer head
    //
    //  second form --  copy constructor

    fixed_message_length_buffer_class(const unsigned long&);
    fixed_message_length_buffer_class(const fixed_message_length_buffer_class&);

    //==================================================================
    // assignment operator
    //==================================================================
    //
    //  basic object copy

    fixed_message_length_buffer_class& operator=(const fixed_message_length_buffer_class&);

    //==================================================================
    // manipulators
    //==================================================================
    //
    // extract and insert manipulators for each C++ base type

    void insert(const char&);
    void insert(const unsigned char&);
    void insert(const signed char&);
    void insert(const short&);
    void insert(const unsigned short&);
    void insert(const int&);
    void insert(const unsigned int&);
    void insert(const long&);
    void insert(const unsigned long&);
    void insert(const float&);
    void insert(const double&);
    void insert(const long double&);

    void extract(char&);
    void extract(unsigned char&);
    void extract(signed char&);
    void extract(short&);
    void extract(unsigned short&);
    void extract(int&);
    void extract(unsigned int&);
    void extract(long&);
    void extract(unsigned long&);
    void extract(float&);
    void extract(double&);
    void extract(long double&);

    //==================================================================
    // destructors
    //==================================================================

    virtual ~fixed_message_length_buffer_class(void);

private:
    //==================================================================
    // member functions
    //==================================================================
    //
    // arguments:
    //    pointer to byte stream,
    //    count of bytes to move,
    //    name of type of datum being extracted/inserted (for diagnostics)

    void insert_into_buffer(const char* const, const unsigned long&, const string&);
};

//=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
// facilitators
//=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+

// one extract and insert operator for each C++ base type

fixed_message_length_buffer_class&
operator>>(fixed_message_length_buffer_class&, char&);
fixed_message_length_buffer_class&
operator>>(fixed_message_length_buffer_class&, unsigned char&);
fixed_message_length_buffer_class&
operator>>(fixed_message_length_buffer_class&, signed char&);
fixed_message_length_buffer_class&
operator>>(fixed_message_length_buffer_class&, short&);
fixed_message_length_buffer_class&
operator>>(fixed_message_length_buffer_class&, unsigned short&);
fixed_message_length_buffer_class&
operator>>(fixed_message_length_buffer_class&, int&);
fixed_message_length_buffer_class&
operator>>(fixed_message_length_buffer_class&, unsigned int&);
fixed_message_length_buffer_class&
operator>>(fixed_message_length_buffer_class&, long&);
fixed_message_length_buffer_class&
operator>>(fixed_message_length_buffer_class&, unsigned long&);
fixed_message_length_buffer_class&
operator>>(fixed_message_length_buffer_class&, float&);
fixed_message_length_buffer_class&
operator>>(fixed_message_length_buffer_class&, double&);
fixed_message_length_buffer_class&
operator>>(fixed_message_length_buffer_class&, long double&);

fixed_message_length_buffer_class&
operator<<(fixed_message_length_buffer_class&, const char&);
fixed_message_length_buffer_class&
operator<<(fixed_message_length_buffer_class&, const unsigned char&);
fixed_message_length_buffer_class&
operator<<(fixed_message_length_buffer_class&, const signed char&);
fixed_message_length_buffer_class&
operator<<(fixed_message_length_buffer_class&, const short&);
fixed_message_length_buffer_class&
operator<<(fixed_message_length_buffer_class&, const unsigned short&);
fixed_message_length_buffer_class&
operator<<(fixed_message_length_buffer_class&, const int&);
fixed_message_length_buffer_class&
operator<<(fixed_message_length_buffer_class&, const unsigned int&);
fixed_message_length_buffer_class&
operator<<(fixed_message_length_buffer_class&, const long&);
fixed_message_length_buffer_class&
operator<<(fixed_message_length_buffer_class&, const unsigned long&);
fixed_message_length_buffer_class&
operator<<(fixed_message_length_buffer_class&, const float&);
fixed_message_length_buffer_class&
operator<<(fixed_message_length_buffer_class&, const double&);
fixed_message_length_buffer_class&
operator<<(fixed_message_length_buffer_class&, const long double&);

//************************************************************************
// buffering class for variable-length buffers
//
// key properties of class:
// buffer may be enlarged, after instantiation, by assignment, extraction
//************************************************************************

//=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
// variable-length message buffering class per se
//=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+

class variable_message_length_buffer_class : public message_buffer_interface_class
{
public:
    //==================================================================
    // constructors
    //==================================================================
    //
    //  first form --
    //        allocate buffer of specified size,
    //        setting current character pointer to buffer head
    //
    //  second form --  copy constructor

    variable_message_length_buffer_class(const unsigned long&);
    variable_message_length_buffer_class(const variable_message_length_buffer_class&);

    //==================================================================
    // assignment operator
    //==================================================================
    //
    //  basic object copy

    variable_message_length_buffer_class& operator=(const variable_message_length_buffer_class&);

    //==================================================================
    // manipulators
    //==================================================================
    //
    // extract and insert manipulators for each C++ base type

    void insert(const char&);
    void insert(const unsigned char&);
    void insert(const signed char&);
    void insert(const short&);
    void insert(const unsigned short&);
    void insert(const int&);
    void insert(const unsigned int&);
    void insert(const long&);
    void insert(const unsigned long&);
    void insert(const float&);
    void insert(const double&);
    void insert(const long double&);

    void extract(char&);
    void extract(unsigned char&);
    void extract(signed char&);
    void extract(short&);
    void extract(unsigned short&);
    void extract(int&);
    void extract(unsigned int&);
    void extract(long&);
    void extract(unsigned long&);
    void extract(float&);
    void extract(double&);
    void extract(long double&);

    //  set buffer to specified size, if buffer smaller than requested size

    void resize(const unsigned long);

    //==================================================================
    // destructors
    //==================================================================

    virtual ~variable_message_length_buffer_class(void);

private:
    //==================================================================
    // member functions
    //==================================================================
    //
    // arguments:
    //    pointer to byte stream,
    //    count of bytes to move,
    //    name of type of datum being extracted/inserted (for diagnostics)

    void insert_into_buffer(const char* const, const unsigned long&, const string&);
};

//=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
// auxiliary operators
//=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+

// one extract and insert operator for each C++ base type

variable_message_length_buffer_class&
operator>>(variable_message_length_buffer_class&, char&);
variable_message_length_buffer_class&
operator>>(variable_message_length_buffer_class&, unsigned char&);
variable_message_length_buffer_class&
operator>>(variable_message_length_buffer_class&, signed char&);
variable_message_length_buffer_class&
operator>>(variable_message_length_buffer_class&, short&);
variable_message_length_buffer_class&
operator>>(variable_message_length_buffer_class&, unsigned short&);
variable_message_length_buffer_class&
operator>>(variable_message_length_buffer_class&, int&);
variable_message_length_buffer_class&
operator>>(variable_message_length_buffer_class&, unsigned int&);
variable_message_length_buffer_class&
operator>>(variable_message_length_buffer_class&, long&);
variable_message_length_buffer_class&
operator>>(variable_message_length_buffer_class&, unsigned long&);
variable_message_length_buffer_class&
operator>>(variable_message_length_buffer_class&, float&);
variable_message_length_buffer_class&
operator>>(variable_message_length_buffer_class&, double&);
variable_message_length_buffer_class&
operator>>(variable_message_length_buffer_class&, long double&);

variable_message_length_buffer_class&
operator<<(variable_message_length_buffer_class&, const char&);
variable_message_length_buffer_class&
operator<<(variable_message_length_buffer_class&, const unsigned char&);
variable_message_length_buffer_class&
operator<<(variable_message_length_buffer_class&, const signed char&);
variable_message_length_buffer_class&
operator<<(variable_message_length_buffer_class&, const short&);
variable_message_length_buffer_class&
operator<<(variable_message_length_buffer_class&, const unsigned short&);
variable_message_length_buffer_class&
operator<<(variable_message_length_buffer_class&, const int&);
variable_message_length_buffer_class&
operator<<(variable_message_length_buffer_class&, const unsigned int&);
variable_message_length_buffer_class&
operator<<(variable_message_length_buffer_class&, const long&);
variable_message_length_buffer_class&
operator<<(variable_message_length_buffer_class&, const unsigned long&);
variable_message_length_buffer_class&
operator<<(variable_message_length_buffer_class&, const float&);
variable_message_length_buffer_class&
operator<<(variable_message_length_buffer_class&, const double&);
variable_message_length_buffer_class&
operator<<(variable_message_length_buffer_class&, const long double&);

//******************************************************************
// exception classes
//******************************************************************

#ifndef EXCEPTION_H
#include "exception.h"
#endif

class message_buffer_exception : public basic_exception
{
public:
    message_buffer_exception(
        const string & = "", const string & = "", const string & = "",
        const basic_exception & = basic_exception()
    );
    message_buffer_exception(const message_buffer_exception& source);
    virtual ~message_buffer_exception(void);
};

class message_buffer_incomparable_sizes_exception : public message_buffer_exception
{
public:
    message_buffer_incomparable_sizes_exception(
        const string & = "", const string & = "", const string & = "",
        const basic_exception & = basic_exception()
    );
    message_buffer_incomparable_sizes_exception(
        const message_buffer_incomparable_sizes_exception& source);
    virtual ~message_buffer_incomparable_sizes_exception(void);
};

class message_buffer_exhausted_exception : public message_buffer_exception
{
public:
    message_buffer_exhausted_exception(
        const string & = "", const string & = "", const string & = "",
        const basic_exception & = basic_exception()
    );
    message_buffer_exhausted_exception(
        const message_buffer_exhausted_exception& source);
    virtual ~message_buffer_exhausted_exception(void);
};

class message_buffer_overflow_exception : public message_buffer_exception
{
public:
    message_buffer_overflow_exception(
        const string & = "", const string & = "", const string & = "",
        const basic_exception & = basic_exception()
    );
    message_buffer_overflow_exception(
        const message_buffer_overflow_exception& source);
    virtual ~message_buffer_overflow_exception(void);
};

// *===*====*===*====*===*====*===*====*===*====*===*====*===*====*===*====
// auxiliary operators
// *===*====*===*====*===*====*===*====*===*====*===*====*===*====*===*====

ostream& operator<<(ostream&, const message_buffer_exception&);
ostream& operator<<(ostream&, const message_buffer_incomparable_sizes_exception&);
ostream& operator<<(ostream&, const message_buffer_exhausted_exception&);
ostream& operator<<(ostream&, const message_buffer_overflow_exception&);

#endif
