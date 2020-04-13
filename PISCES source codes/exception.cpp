//--------------exception.cpp------------------------------------------------------
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
//--------------------------------------------------------------------------------

#include "exception.h"

//*****************************************************
//  class method definitions
//*****************************************************

//  ==============================
//   constructors
//  ==============================

//------------------------------------------------------------------
//  first form of constructor -
//      set object to contain specified routine name, etc.
//------------------------------------------------------------------
//
basic_exception::basic_exception(
    const string& routine_name,
    const string& short_description,
    const string& detailed_description
)
    : routine_name_(routine_name),
    short_description_(short_description),
    detailed_description_(detailed_description)
{
}

//------------------------------------------------------------------
//  second form of constructor -
//      like first, but add information from previously thrown exception
//      to detailed description of exception
//------------------------------------------------------------------
//
#include <iostream>
using namespace std;

basic_exception::basic_exception(
    const string& routine_name,
    const string& short_description,
    const string& detailed_description,
    const basic_exception& prior_exception
)
    : routine_name_(routine_name),
    short_description_(short_description),
    detailed_description_(detailed_description)
{
    //  **** initialize static data for message assembly ****
    //
    static const string first_thrown_by = "\nrethrow of exception generated by ";
    static const string old_short = "\nprev. synopsis: ";
    static const string old_long = "\nprev. descrip.: ";

    //  **** see if previous exception contributes to this message ****
    //
    const string past_name = prior_exception.routine_name_;
    const string past_short_description = prior_exception.short_description_;
    const string past_long_description = prior_exception.detailed_description_;

    if (past_name != "")
    {
        string eEXCEPTION_History = first_thrown_by + past_name;

        if (past_short_description != "") eEXCEPTION_History += old_short + past_short_description;
        if (past_long_description != "")  eEXCEPTION_History += old_long + past_long_description;

        detailed_description_ += eEXCEPTION_History;
    }
}

//------------------------------------------------------------------
//  third form of constructor -  copy constructor
//------------------------------------------------------------------
//
basic_exception::basic_exception(const basic_exception& sourceException)
    : routine_name_(sourceException.routine_name_),
    short_description_(sourceException.short_description_),
    detailed_description_(sourceException.detailed_description_)
{
}

//  ==============================
//   assignment operator
//  ==============================

//------------------------------------------------------------------
//  assignment operator, first form - makes new copy of message object
//------------------------------------------------------------------
//
basic_exception& basic_exception::operator=(const basic_exception& sourceException)
{
    routine_name_ = sourceException.routine_name_;
    short_description_ = sourceException.short_description_;
    detailed_description_ = sourceException.detailed_description_;
    return *this;
}

//  ==============================
//   inspectors
//  ==============================

//------------------------------------------------------------------
//  get_routine_name() -       sets routine that threw exception
//------------------------------------------------------------------
//
void basic_exception::get_routine_name(string& target_name) const
{
    target_name = routine_name_;
}

//------------------------------------------------------------------
//  get_short_description() -  sets short description of problem
//------------------------------------------------------------------
//
void basic_exception::get_short_description(string& target_description) const
{
    target_description = short_description_;
}

//------------------------------------------------------------------
//  get_detailed_description() -  sets problem detailed message
//------------------------------------------------------------------
//
void basic_exception::get_detailed_description(string& target_description) const
{
    target_description = detailed_description_;
}

//  ==============================
//   manipulators
//  ==============================

//------------------------------------------------------------------
//  set_routine_name() -       sets routine that threw exception
//------------------------------------------------------------------
//
void basic_exception::set_routine_name(const string& source_name)
{
    routine_name_ = source_name;
}

//------------------------------------------------------------------
//  set_short_description() -  sets short description of problem
//------------------------------------------------------------------
//
void basic_exception::set_short_description(const string& source_description)
{
    short_description_ = source_description;
}

//------------------------------------------------------------------
//  set_detailed_description() -  set detailed description of problem
//------------------------------------------------------------------
//
void basic_exception::set_detailed_description(const string& source_description)
{
    detailed_description_ = source_description;
}

//  ==============================
//   facilitators
//  ==============================

//------------------------------------------------------------------
//  insert() -- dump messages to specified string
//
//  ignore stream errors--if stream is bad, what can we really do?
//------------------------------------------------------------------
//
void basic_exception::insert(ostream& os) const
{
    string result = routine_name_ + " -- ";

    //  limit name/short description part of message to number of chars
    //  that typically fit on one line of a console
    //
    const unsigned max_prefix_length = 80;

    if (short_description_ != "")
    {
        if (result.length() + short_description_.length() > max_prefix_length)
            result += "\n";
        result += short_description_;
    }

    if (detailed_description_ != "") result += "\n" + detailed_description_;

    os << result;
}

//=====================================
//  destructor
//=====================================
//
basic_exception::~basic_exception(void)
{
}

// =*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
//  basic_exception auxiliary operators
// =*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*

ostream& operator<<(ostream& os, const basic_exception& ex)
{
    ex.insert(os);
    return os;
}