//--------------------message_buffer.cpp-----------------------------------------------------
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
//------------------------------------------------------------------------------------------

#include "message_buffer.h"

#include <sstream>   // stringstream

//******************************************************************
// message buffer (abstract) interface class
//******************************************************************

//=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//  class proper
//=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+

//==================================================================
// constructors
//==================================================================

//------------------------------------------------------------------
// constructor, first form --
//        allocate buffer of specified size, filling with ' 's,
//        and setting current character pointer to buffer head
//------------------------------------------------------------------
//
message_buffer_interface_class::message_buffer_interface_class(const unsigned long& buffer_size)
    : buffer_size_(buffer_size), cursor_(0), p_buffer_(new char[buffer_size])
{
}

//------------------------------------------------------------------
// constructor, second form --  copy constructor
//------------------------------------------------------------------
//
message_buffer_interface_class::message_buffer_interface_class(const message_buffer_interface_class& source)
    : buffer_size_(source.buffer_size_),
    cursor_(source.cursor_),
    p_buffer_(new char[source.buffer_size_])
{
    for (unsigned long i = 0; i < buffer_size_; i++)
        p_buffer_[i] = source.p_buffer_[i];
}

//==================================================================
// assignment operator
//==================================================================
//
//  basic object copy

message_buffer_interface_class& message_buffer_interface_class::operator=(const message_buffer_interface_class& source)
{
    // copy over as much as will fit from source
    // (derived classes will handle resize operation, as required)
    //
    cursor_ = source.cursor_;
    for (unsigned long i = 0; i < buffer_size_ && i < source.buffer_size_; i++)
        p_buffer_[i] = source.p_buffer_[i];
    return *this;
}

//==================================================================
// inspectors
//==================================================================

//-----------------------------------------------------------------------------
// p_buffer() -
//     return pointer to enclosed buffer
//     (to allow direct manipulation by system messaging primitives)
//-----------------------------------------------------------------------------
//
char* message_buffer_interface_class::p_buffer(void)
{
    return p_buffer_;
}

//-----------------------------------------------------------------------------
// p_size() -
//     return pointer to buffer size variable
//     (to allow direct manipulation by system messaging primitives)
//-----------------------------------------------------------------------------
//
unsigned long* message_buffer_interface_class::p_size(void)
{
    return &buffer_size_;
}

//==================================================================
// manipulators
//==================================================================

//---------------------------------------------------------------------
// reset() - put "current character" pointer back to head of buffer
//---------------------------------------------------------------------
//
void message_buffer_interface_class::reset(void)
{
    cursor_ = 0;
}

//==================================================================
// destructors
//==================================================================

message_buffer_interface_class::~message_buffer_interface_class(void)
{
    if (p_buffer_ != 0) delete[] p_buffer_;
}

//==================================================================
// private member functions
//==================================================================

//-------------------------------------------------------------------
// generalized data extraction routine  [i.e., *this >> object]
//-------------------------------------------------------------------
//
void message_buffer_interface_class::extract_from_buffer(
    char* const p_target,
    const unsigned long& size,
    const string& type_name
)
{
    // **** **** define data for error messages **** ****

    static const string my_name = "message_buffer_interface_class::extract";
    static const string no_room = "not enough data in buffer ";

    // **** **** check for underflow before attempting insertion **** ****

    if (size + cursor_ > buffer_size_)
    {
        stringstream p_message;
        p_message << "attempt to extract " << size << " bytes; ";
        switch (buffer_size_ - cursor_)
        {
        case 0:   p_message << " no bytes left in buffer ";
            break;
        case 1:   p_message << " only 1 byte left in buffer ";
            break;
        default:  p_message << " only " << (buffer_size_ - cursor_) << " bytes left in buffer ";
        }
        throw message_buffer_exhausted_exception(my_name, no_room + type_name, p_message.str());
    }

    // **** **** marshall object into buffer **** ****

    for (unsigned long i = 0; i < size; i++, cursor_++)
        p_target[i] = p_buffer_[cursor_];
}

//-------------------------------------------------------------------
// generalized data insertion routine    [i.e., *this << object]
//-------------------------------------------------------------------
//
void message_buffer_interface_class::insert_into_buffer(
    const char* const p_source,
    const unsigned long& size,
    const string& type_name
)
{
    // **** **** marshall object into buffer **** ****
    //
    // assumption: check for overflow is performed by derived class

    for (unsigned i = 0; i < size; i++, cursor_++)
        p_buffer_[cursor_] = p_source[i];
}

//******************************************************************
// fixed-length message buffer class
//******************************************************************

//=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//  class proper
//=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+

//==================================================================
// constructors
//==================================================================

//------------------------------------------------------------------
// constructor, first form --
//        allocate buffer of specified size, filling with ' 's,
//        and setting current character pointer to buffer head
//------------------------------------------------------------------
//
fixed_message_length_buffer_class::fixed_message_length_buffer_class(const unsigned long& buffer_size)
    : message_buffer_interface_class(buffer_size)
{
}

//------------------------------------------------------------------
// constructor, second form --  copy constructor
//------------------------------------------------------------------
//
fixed_message_length_buffer_class::fixed_message_length_buffer_class(const fixed_message_length_buffer_class& source)
    : message_buffer_interface_class(source)
{
}

//==================================================================
// assignment operator
//==================================================================
//
//  basic object copy

fixed_message_length_buffer_class& fixed_message_length_buffer_class::operator=(const fixed_message_length_buffer_class& source)
{
    // **** **** define data for error messages **** ****

    static const string my_name = "fixed_message_length_buffer_class::operator=";
    static const string bad_sizes = "assignment between different-sized buffers ";

    // **** check for source, target buffer size mismatch before copy ****

    if (buffer_size_ != source.buffer_size_)
    {
        stringstream p_message;
        p_message << "size of source, target buffers = " << source.buffer_size_ << ", " << buffer_size_;
        throw message_buffer_incomparable_sizes_exception(my_name, bad_sizes, p_message.str());
    }

    // **** do the assignment ****

    message_buffer_interface_class::operator= (source);
    return *this;
}

//==================================================================
// manipulators
//==================================================================

//---------------------------------------------------------------------
// extract manipulators for each C++ base type  [i.e., buffer >> object]
//---------------------------------------------------------------------
//
void fixed_message_length_buffer_class::extract(char& target)
{
    extract_from_buffer(reinterpret_cast<char*>(&target), sizeof(target), "char");
}

void fixed_message_length_buffer_class::extract(unsigned char& target)
{
    extract_from_buffer(reinterpret_cast<char*>(&target), sizeof(target), "unsigned char");
}

void fixed_message_length_buffer_class::extract(signed char& target)
{
    extract_from_buffer(reinterpret_cast<char*>(&target), sizeof(target), "signed char");
}

void fixed_message_length_buffer_class::extract(short& target)
{
    extract_from_buffer(reinterpret_cast<char*>(&target), sizeof(target), "short");
}

void fixed_message_length_buffer_class::extract(unsigned short& target)
{
    extract_from_buffer(reinterpret_cast<char*>(&target), sizeof(target), "unsigned short");
}
void fixed_message_length_buffer_class::extract(int& target)
{
    extract_from_buffer(reinterpret_cast<char*>(&target), sizeof(target), "int");
}

void fixed_message_length_buffer_class::extract(unsigned int& target)
{
    extract_from_buffer(reinterpret_cast<char*>(&target), sizeof(target), "unsigned int");
}

void fixed_message_length_buffer_class::extract(long& target)
{
    extract_from_buffer(reinterpret_cast<char*>(&target), sizeof(target), "long");
}

void fixed_message_length_buffer_class::extract(unsigned long& target)
{
    extract_from_buffer(reinterpret_cast<char*>(&target), sizeof(target), "unsigned long");
}

void fixed_message_length_buffer_class::extract(float& target)
{
    extract_from_buffer(reinterpret_cast<char*>(&target), sizeof(target), "float");
}

void fixed_message_length_buffer_class::extract(double& target)
{
    extract_from_buffer(reinterpret_cast<char*>(&target), sizeof(target), "double");
}

void fixed_message_length_buffer_class::extract(long double& target)
{
    extract_from_buffer(reinterpret_cast<char*>(&target), sizeof(target), "long double");
}

//---------------------------------------------------------------------
// insert manipulators for each C++ base type  [i.e., buffer << object]
//---------------------------------------------------------------------
//
void fixed_message_length_buffer_class::insert(const char& source)
{
    insert_into_buffer(reinterpret_cast<const char*>(&source), sizeof(source), "char");
}

void fixed_message_length_buffer_class::insert(const unsigned char& source)
{
    insert_into_buffer(reinterpret_cast<const char*>(&source), sizeof(source), "unsigned char");
}

void fixed_message_length_buffer_class::insert(const signed char& source)
{
    insert_into_buffer(reinterpret_cast<const char*>(&source), sizeof(source), "signed char");
}

void fixed_message_length_buffer_class::insert(const short& source)
{
    insert_into_buffer(reinterpret_cast<const char*>(&source), sizeof(source), "short");
}

void fixed_message_length_buffer_class::insert(const unsigned short& source)
{
    insert_into_buffer(reinterpret_cast<const char*>(&source), sizeof(source), "unsigned short");
}

void fixed_message_length_buffer_class::insert(const int& source)
{
    insert_into_buffer(reinterpret_cast<const char*>(&source), sizeof(source), "int");
}

void fixed_message_length_buffer_class::insert(const unsigned int& source)
{
    insert_into_buffer(reinterpret_cast<const char*>(&source), sizeof(source), "unsigned int");
}

void fixed_message_length_buffer_class::insert(const long& source)
{
    insert_into_buffer(reinterpret_cast<const char*>(&source), sizeof(source), "long");
}

void fixed_message_length_buffer_class::insert(const unsigned long& source)
{
    insert_into_buffer(reinterpret_cast<const char*>(&source), sizeof(source), "unsigned long");
}

void fixed_message_length_buffer_class::insert(const float& source)
{
    insert_into_buffer(reinterpret_cast<const char*>(&source), sizeof(source), "float");
}

void fixed_message_length_buffer_class::insert(const double& source)
{
    insert_into_buffer(reinterpret_cast<const char*>(&source), sizeof(source), "double");
}

void fixed_message_length_buffer_class::insert(const long double& source)
{
    insert_into_buffer(reinterpret_cast<const char*>(&source), sizeof(source), "long double");
}

//==================================================================
// destructors
//==================================================================

fixed_message_length_buffer_class::~fixed_message_length_buffer_class(void)
{
}

//==================================================================
// private member functions
//==================================================================

//-------------------------------------------------------------------
// generalized data extraction routine
//-------------------------------------------------------------------
//
void fixed_message_length_buffer_class::
insert_into_buffer(
    const char* const p_target,
    const unsigned long& size,
    const string& type_name
)
{
    // **** **** define data for error messages **** ****

    static const string my_name = "fixed_message_length_buffer_class::insert";
    static const string no_room = "not enough room in buffer ";

    // **** **** check for overflow before attempting extract **** ****

    if (size + cursor_ > buffer_size_)
    {
        stringstream p_message;

        p_message << "attempt to insert " << size << " byte";
        if (size > 1) p_message << "s";
        p_message << "; ";

        switch (buffer_size_ - cursor_)
        {
        case 0:   p_message << " no room left in buffer ";
            break;
        case 1:   p_message << " only 1 byte left in buffer ";
            break;
        default:  p_message << " only " << (buffer_size_ - cursor_) << " bytes left in buffer ";
        }

        throw message_buffer_overflow_exception(my_name, no_room + type_name, p_message.str());
    }

    // **** **** marshall object into buffer **** ****
    //

    message_buffer_interface_class::insert_into_buffer(p_target, size, type_name);
}

//=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
// auxiliary operators
//=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+

//------------------------------------------------------------------
// one extraction operator for each C++ base type
//------------------------------------------------------------------

fixed_message_length_buffer_class& operator>>(fixed_message_length_buffer_class& source, char& target)
{
    source.extract(target);
    return source;
}

fixed_message_length_buffer_class& operator>>(fixed_message_length_buffer_class& source, unsigned char& target)
{
    source.extract(target);
    return source;
}

fixed_message_length_buffer_class& operator>>(fixed_message_length_buffer_class& source, signed char& target)
{
    source.extract(target);
    return source;
}

fixed_message_length_buffer_class& operator>>(fixed_message_length_buffer_class& source, short& target)
{
    source.extract(target);
    return source;
}

fixed_message_length_buffer_class& operator>>(fixed_message_length_buffer_class& source, unsigned short& target)
{
    source.extract(target);
    return source;
}

fixed_message_length_buffer_class& operator>>(fixed_message_length_buffer_class& source, int& target)
{
    source.extract(target);
    return source;
}

fixed_message_length_buffer_class& operator>>(fixed_message_length_buffer_class& source, unsigned int& target)
{
    source.extract(target);
    return source;
}

fixed_message_length_buffer_class& operator>>(fixed_message_length_buffer_class& source, long& target)
{
    source.extract(target);
    return source;
}

fixed_message_length_buffer_class& operator>>(fixed_message_length_buffer_class& source, unsigned long& target)
{
    source.extract(target);
    return source;
}

fixed_message_length_buffer_class& operator>>(fixed_message_length_buffer_class& source, float& target)
{
    source.extract(target);
    return source;
}

fixed_message_length_buffer_class& operator>>(fixed_message_length_buffer_class& source, double& target)
{
    source.extract(target);
    return source;
}

fixed_message_length_buffer_class& operator>>(fixed_message_length_buffer_class& source, long double& target)
{
    source.extract(target);
    return source;
}

//------------------------------------------------------------------
// one insertion operator for each C++ base type
//------------------------------------------------------------------

fixed_message_length_buffer_class& operator<<(fixed_message_length_buffer_class& target, const char& source)
{
    target.insert(source);
    return target;
}

fixed_message_length_buffer_class& operator<<(fixed_message_length_buffer_class& target, const unsigned char& source)
{
    target.insert(source);
    return target;
}

fixed_message_length_buffer_class& operator<<(fixed_message_length_buffer_class& target, const signed char& source)
{
    target.insert(source);
    return target;
}

fixed_message_length_buffer_class& operator<<(fixed_message_length_buffer_class& target, const short& source)
{
    target.insert(source);
    return target;
}

fixed_message_length_buffer_class& operator<<(fixed_message_length_buffer_class& target, const unsigned short& source)
{
    target.insert(source);
    return target;
}

fixed_message_length_buffer_class& operator<<(fixed_message_length_buffer_class& target, const int& source)
{
    target.insert(source);
    return target;
}

fixed_message_length_buffer_class& operator<<(fixed_message_length_buffer_class& target, const unsigned int& source)
{
    target.insert(source);
    return target;
}

fixed_message_length_buffer_class& operator<<(fixed_message_length_buffer_class& target, const long& source)
{
    target.insert(source);
    return target;
}

fixed_message_length_buffer_class& operator<<(fixed_message_length_buffer_class& target, const unsigned long& source)
{
    target.insert(source);
    return target;
}

fixed_message_length_buffer_class& operator<<(fixed_message_length_buffer_class& target, const float& source)
{
    target.insert(source);
    return target;
}

fixed_message_length_buffer_class& operator<<(fixed_message_length_buffer_class& target, const double& source)
{
    target.insert(source);
    return target;
}

fixed_message_length_buffer_class& operator<<(fixed_message_length_buffer_class& target, const long double& source)
{
    target.insert(source);
    return target;
}

//******************************************************************
// variable-length message buffer class
//******************************************************************

//=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//  class proper
//=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+

//==================================================================
// constructors
//==================================================================

//------------------------------------------------------------------
// constructor, first form --
//        allocate buffer of specified size, filling with ' 's,
//        and setting current character pointer to buffer head
//------------------------------------------------------------------
//
variable_message_length_buffer_class::variable_message_length_buffer_class(const unsigned long& buffer_size)
    : message_buffer_interface_class(buffer_size)
{
}

//------------------------------------------------------------------
// constructor, second form --  copy constructor
//------------------------------------------------------------------
//
variable_message_length_buffer_class::variable_message_length_buffer_class(const variable_message_length_buffer_class& source)
    : message_buffer_interface_class(source)
{
}

//==================================================================
// assignment operator
//==================================================================

//------------------------------------------------------------------
//  basic object copy
//------------------------------------------------------------------
//
variable_message_length_buffer_class& variable_message_length_buffer_class::operator=(const variable_message_length_buffer_class& source)
{
    // **** resize buffer on buffer size mismatch ****

    if (buffer_size_ < source.buffer_size_)
    {
        delete[] p_buffer_;
        buffer_size_ = source.buffer_size_;
        p_buffer_ = new char[buffer_size_];
    }

    // **** do the assignment ****

    message_buffer_interface_class::operator= (source);
    return *this;
}

//==================================================================
// manipulators
//==================================================================

//---------------------------------------------------------------------
// extract manipulators for each C++ base type  [i.e., buffer >> object]
//---------------------------------------------------------------------
//
void variable_message_length_buffer_class::extract(char& target)
{
    extract_from_buffer(reinterpret_cast<char*>(&target), sizeof(target), "char");
}

void variable_message_length_buffer_class::extract(unsigned char& target)
{
    extract_from_buffer(reinterpret_cast<char*>(&target), sizeof(target), "unsigned char");
}

void variable_message_length_buffer_class::extract(signed char& target)
{
    extract_from_buffer(reinterpret_cast<char*>(&target), sizeof(target), "signed char");
}

void variable_message_length_buffer_class::extract(short& target)
{
    extract_from_buffer(reinterpret_cast<char*>(&target), sizeof(target), "short");
}

void variable_message_length_buffer_class::extract(unsigned short& target)
{
    extract_from_buffer(reinterpret_cast<char*>(&target), sizeof(target), "unsigned short");
}
void variable_message_length_buffer_class::extract(int& target)
{
    extract_from_buffer(reinterpret_cast<char*>(&target), sizeof(target), "int");
}

void variable_message_length_buffer_class::extract(unsigned int& target)
{
    extract_from_buffer(reinterpret_cast<char*>(&target), sizeof(target), "unsigned int");
}

void variable_message_length_buffer_class::extract(long& target)
{
    extract_from_buffer(reinterpret_cast<char*>(&target), sizeof(target), "long");
}

void variable_message_length_buffer_class::extract(unsigned long& target)
{
    extract_from_buffer(reinterpret_cast<char*>(&target), sizeof(target), "unsigned long");
}

void variable_message_length_buffer_class::extract(float& target)
{
    extract_from_buffer(reinterpret_cast<char*>(&target), sizeof(target), "float");
}

void variable_message_length_buffer_class::extract(double& target)
{
    extract_from_buffer(reinterpret_cast<char*>(&target), sizeof(target), "double");
}

void variable_message_length_buffer_class::extract(long double& target)
{
    extract_from_buffer(reinterpret_cast<char*>(&target), sizeof(target), "long double");
}

//---------------------------------------------------------------------
// insert manipulators for each C++ base type  [i.e., buffer << object]
//---------------------------------------------------------------------
//
void variable_message_length_buffer_class::insert(const char& source)
{
    insert_into_buffer(reinterpret_cast<const char*>(&source), sizeof(source), "char");
}

void variable_message_length_buffer_class::insert(const unsigned char& source)
{
    insert_into_buffer(reinterpret_cast<const char*>(&source), sizeof(source), "unsigned char");
}

void variable_message_length_buffer_class::insert(const signed char& source)
{
    insert_into_buffer(reinterpret_cast<const char*>(&source), sizeof(source), "signed char");
}

void variable_message_length_buffer_class::insert(const short& source)
{
    insert_into_buffer(reinterpret_cast<const char*>(&source), sizeof(source), "short");
}

void variable_message_length_buffer_class::insert(const unsigned short& source)
{
    insert_into_buffer(reinterpret_cast<const char*>(&source), sizeof(source), "unsigned short");
}

void variable_message_length_buffer_class::insert(const int& source)
{
    insert_into_buffer(reinterpret_cast<const char*>(&source), sizeof(source), "int");
}

void variable_message_length_buffer_class::insert(const unsigned int& source)
{
    insert_into_buffer(reinterpret_cast<const char*>(&source), sizeof(source), "unsigned int");
}

void variable_message_length_buffer_class::insert(const long& source)
{
    insert_into_buffer(reinterpret_cast<const char*>(&source), sizeof(source), "long");
}

void variable_message_length_buffer_class::insert(const unsigned long& source)
{
    insert_into_buffer(reinterpret_cast<const char*>(&source), sizeof(source), "unsigned long");
}

void variable_message_length_buffer_class::insert(const float& source)
{
    insert_into_buffer(reinterpret_cast<const char*>(&source), sizeof(source), "float");
}

void variable_message_length_buffer_class::insert(const double& source)
{
    insert_into_buffer(reinterpret_cast<const char*>(&source), sizeof(source), "double");
}

void variable_message_length_buffer_class::insert(const long double& source)
{
    insert_into_buffer(reinterpret_cast<const char*>(&source), sizeof(source), "long double");
}

//------------------------------------------------------------------
//  resize() - expand buffer, if smaller than specified value
//------------------------------------------------------------------
//
void variable_message_length_buffer_class::resize(const unsigned long size)
{
    // **** resize buffer on buffer size mismatch ****

    if (buffer_size_ < size)
    {
        // make copy of current buffer in new, larger buffer
        //
        char* p_new_buffer = new char[size];
        for (unsigned long i = 0; i < buffer_size_; i++)
            p_new_buffer[i] = p_buffer_[i];

        // replace current with new
        //
        delete[] p_buffer_;
        p_buffer_ = p_new_buffer;
        buffer_size_ = size;
    }
}

//==================================================================
// destructors
//==================================================================

variable_message_length_buffer_class::~variable_message_length_buffer_class(void)
{
}

//==================================================================
// private member functions
//==================================================================

//-------------------------------------------------------------------
// generalized data extraction routine
//-------------------------------------------------------------------
//
void variable_message_length_buffer_class::insert_into_buffer(const char* const p_target, const unsigned long& char_count, const string& type_name)
{
    // **** **** resize buffer if overflow would occur on extract **** ****

    if (char_count + cursor_ > buffer_size_)
    {
        char* p_new_buffer = new char[char_count + cursor_];

        for (unsigned i = 0; i < buffer_size_; i++) p_new_buffer[i] = p_buffer_[i];
        delete[] p_buffer_;
        p_buffer_ = p_new_buffer;
        buffer_size_ = char_count + cursor_;
    }

    // **** **** marshall object into buffer **** ****
    //

    message_buffer_interface_class::insert_into_buffer(p_target, char_count, type_name);
}

//=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
// auxiliary operators
//=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+

//------------------------------------------------------------------
// one extraction operator for each C++ base type
//------------------------------------------------------------------

variable_message_length_buffer_class& operator>>(variable_message_length_buffer_class& source, char& target)
{
    source.extract(target);
    return source;
}

variable_message_length_buffer_class& operator>>(variable_message_length_buffer_class& source, unsigned char& target)
{
    source.extract(target);
    return source;
}

variable_message_length_buffer_class& operator>>(variable_message_length_buffer_class& source, signed char& target)
{
    source.extract(target);
    return source;
}

variable_message_length_buffer_class& operator>>(variable_message_length_buffer_class& source, short& target)
{
    source.extract(target);
    return source;
}

variable_message_length_buffer_class& operator>>(variable_message_length_buffer_class& source, unsigned short& target)
{
    source.extract(target);
    return source;
}

variable_message_length_buffer_class& operator>>(variable_message_length_buffer_class& source, int& target)
{
    source.extract(target);
    return source;
}

variable_message_length_buffer_class& operator>>(variable_message_length_buffer_class& source, unsigned int& target)
{
    source.extract(target);
    return source;
}

variable_message_length_buffer_class& operator>>(variable_message_length_buffer_class& source, long& target)
{
    source.extract(target);
    return source;
}

variable_message_length_buffer_class& operator>>(variable_message_length_buffer_class& source, unsigned long& target)
{
    source.extract(target);
    return source;
}

variable_message_length_buffer_class& operator>>(variable_message_length_buffer_class& source, float& target)
{
    source.extract(target);
    return source;
}

variable_message_length_buffer_class& operator>>(variable_message_length_buffer_class& source, double& target)
{
    source.extract(target);
    return source;
}

variable_message_length_buffer_class& operator>>(variable_message_length_buffer_class& source, long double& target)
{
    source.extract(target);
    return source;
}

//------------------------------------------------------------------
// one insertion operator for each C++ base type
//------------------------------------------------------------------

variable_message_length_buffer_class& operator<<(variable_message_length_buffer_class& target, const char& source)
{
    target.insert(source);
    return target;
}

variable_message_length_buffer_class& operator<<(variable_message_length_buffer_class& target, const unsigned char& source)
{
    target.insert(source);
    return target;
}

variable_message_length_buffer_class& operator<<(variable_message_length_buffer_class& target, const signed char& source)
{
    target.insert(source);
    return target;
}

variable_message_length_buffer_class& operator<<(variable_message_length_buffer_class& target, const short& source)
{
    target.insert(source);
    return target;
}

variable_message_length_buffer_class& operator<<(variable_message_length_buffer_class& target, const unsigned short& source)
{
    target.insert(source);
    return target;
}

variable_message_length_buffer_class& operator<<(variable_message_length_buffer_class& target, const int& source)
{
    target.insert(source);
    return target;
}

variable_message_length_buffer_class& operator<<(variable_message_length_buffer_class& target, const unsigned int& source)
{
    target.insert(source);
    return target;
}

variable_message_length_buffer_class& operator<<(variable_message_length_buffer_class& target, const long& source)
{
    target.insert(source);
    return target;
}

variable_message_length_buffer_class& operator<<(variable_message_length_buffer_class& target, const unsigned long& source)
{
    target.insert(source);
    return target;
}

variable_message_length_buffer_class& operator<<(variable_message_length_buffer_class& target, const float& source)
{
    target.insert(source);
    return target;
}

variable_message_length_buffer_class& operator<<(variable_message_length_buffer_class& target, const double& source)
{
    target.insert(source);
    return target;
}

variable_message_length_buffer_class& operator<<(variable_message_length_buffer_class& target, const long double& source)
{
    target.insert(source);
    return target;
}

//******************************************************************
// exception classes [boilerplate]
//******************************************************************

message_buffer_exception::message_buffer_exception(
    const string& name,
    const string& short_description,
    const string& detailed_description,
    const basic_exception& prior_exception
)
    : basic_exception(name, short_description, detailed_description, prior_exception) {}

message_buffer_exception::message_buffer_exception(const message_buffer_exception& source)
    : basic_exception(source) {}

message_buffer_exception::~message_buffer_exception(void) {}

message_buffer_exhausted_exception::message_buffer_exhausted_exception(
    const string& name,
    const string& short_description,
    const string& detailed_description,
    const basic_exception& prior_exception
)
    : message_buffer_exception(name, short_description, detailed_description, prior_exception) {}

message_buffer_exhausted_exception::message_buffer_exhausted_exception(const message_buffer_exhausted_exception& source)
    : message_buffer_exception(source) {}

message_buffer_exhausted_exception::~message_buffer_exhausted_exception(void) {}

message_buffer_incomparable_sizes_exception::message_buffer_incomparable_sizes_exception(
    const string& name,
    const string& short_description,
    const string& detailed_description,
    const basic_exception& prior_exception
)
    : message_buffer_exception(name, short_description, detailed_description, prior_exception) {}

message_buffer_incomparable_sizes_exception::message_buffer_incomparable_sizes_exception(
    const message_buffer_incomparable_sizes_exception& source)
    : message_buffer_exception(source) {}

message_buffer_incomparable_sizes_exception::~message_buffer_incomparable_sizes_exception(void) {}

message_buffer_overflow_exception::message_buffer_overflow_exception(
    const string& name,
    const string& short_description,
    const string& detailed_description,
    const basic_exception& prior_exception
)
    : message_buffer_exception(name, short_description, detailed_description, prior_exception) {}

message_buffer_overflow_exception::message_buffer_overflow_exception(const message_buffer_overflow_exception& source)
    : message_buffer_exception(source) {}

message_buffer_overflow_exception::~message_buffer_overflow_exception(void) {}

// *===*====*===*====*===*====*===*====*===*====*===*====*===*====*===*====
// auxiliary operators  [boilerplate]
// *===*====*===*====*===*====*===*====*===*====*===*====*===*====*===*====

ostream& operator<<(ostream& os, const message_buffer_exception& ex)
{
    ex.insert(os);
    return os;
}

ostream& operator<<(ostream& os, const message_buffer_incomparable_sizes_exception& ex)
{
    ex.insert(os);
    return os;
}

ostream& operator<<(ostream& os, const message_buffer_exhausted_exception& ex)
{
    ex.insert(os);
    return os;
}

ostream& operator<<(ostream& os, const message_buffer_overflow_exception& ex)
{
    ex.insert(os);
    return os;
}