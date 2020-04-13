// ---------------------elapsed_time.cpp---------------------------------------
//
//   Author:  Phil Pfeiffer
//   Date:    August 1999
//   Last modified:  April 2020
//
//   Support a measure of elapsed time, in seconds/ hours/ minutes/ days,
//      together with operations on elapsed time.
//
//   Design notes:
//       No attention given in class to carrying out computations in ways
//       that detect potential overflow, or minimize chance of same.
//-----------------------------------------------------------------------------

#include "elapsed_time.h"

#ifndef BOOL_H
#include "bool.h"
#endif

#ifndef STD_H
#include "std.h"
#endif

#include <iomanip>
#include <sstream>   // stringstream

#ifndef SOCKET_APIS_H
#include "socket_apis.h"   // **needed for IO transparency,
#endif                       //hopefully an STL replacement here will improve this

//*****************************************************
//  class method definitions
//*****************************************************

//  ==============================
//   constructors
//  ==============================

//--------------------------------------------------------------------------
//  constructor, first form--
//      initialize contents of elapsed time object to specified quantity of time
//--------------------------------------------------------------------------

elapsedTimeClass::elapsedTimeClass(const int seconds, const int minutes, const int hours, const int days)
    : seconds_(seconds), minutes_(minutes), hours_(hours), days_(days)
{
    normalize();
}

//--------------------------------------------------------------------------
//  constructor, second form-- copy constructor
//--------------------------------------------------------------------------

elapsedTimeClass::elapsedTimeClass(const elapsedTimeClass& source_object)
    : seconds_(source_object.seconds_),
    minutes_(source_object.minutes_),
    hours_(source_object.hours_),
    days_(source_object.days_)
{
}

//  ==============================
//   assignment operators
//  ==============================

//--------------------------------------------------------------------------
// standard object copy
//--------------------------------------------------------------------------
//
elapsedTimeClass& elapsedTimeClass::operator=(const elapsedTimeClass& source_object)
{
    seconds_ = source_object.seconds_;
    minutes_ = source_object.minutes_;
    hours_ = source_object.hours_;
    days_ = source_object.days_;
    return *this;
}

//  ==============================
//   inspectors
//  ==============================
//
//   one inspector provided below for each field in the structure

int elapsedTimeClass::seconds(void) const
{
    return seconds_;
}

int elapsedTimeClass::minutes(void) const
{
    return minutes_;
}

int elapsedTimeClass::hours(void) const
{
    return hours_;
}

int elapsedTimeClass::days(void) const
{
    return days_;         // when stored, 1 = 1st day of month
}

//  ==============================
//   facilitators
//  ==============================

//--------------------------------------------------------------------------
//  add - increment current time unit by specified time unit
//--------------------------------------------------------------------------

elapsedTimeClass elapsedTimeClass::add(const elapsedTimeClass& addend) const
{
    elapsedTimeClass result;

    result.seconds_ = seconds() + addend.seconds();
    result.minutes_ = minutes() + addend.minutes();
    result.hours_ = hours() + addend.hours();
    result.days_ = days() + addend.days();

    result.normalize();

    return result;
}

//--------------------------------------------------------------------------
//  subtract - decrement current time unit by specified time unit
//--------------------------------------------------------------------------

elapsedTimeClass elapsedTimeClass::subtract(const elapsedTimeClass& minuend) const
{
    elapsedTimeClass result;

    result.seconds_ = seconds() - minuend.seconds();
    result.minutes_ = minutes() - minuend.minutes();
    result.hours_ = hours() - minuend.hours();
    result.days_ = days() - minuend.days();

    result.normalize();

    return result;
}

//--------------------------------------------------------------------------
//  compare - compare *this to argument,
//            returning -1, 0, 1 when *this is LT, EQ, GT operand, respectively
//--------------------------------------------------------------------------
//
// routine assumes *this, other are normalized (should always be true on entry)

int elapsedTimeClass::compare(const elapsedTimeClass& other) const
{
    if (days_ < other.days()) return -1;
    if (days_ > other.days()) return 1;

    if (hours_ < other.hours()) return -1;
    if (hours_ > other.hours()) return 1;

    if (minutes_ < other.minutes()) return -1;
    if (minutes_ > other.minutes()) return 1;

    if (seconds_ < other.seconds()) return -1;
    if (seconds_ > other.seconds()) return 1;

    return 0;
}

//--------------------------------------------------------------------------
//  extract - set internal elapsed time data from stream
//--------------------------------------------------------------------------
//
//  on error, stream returned to original position and state

void elapsedTimeClass::extract(istream& is)
{
    // ==== attempt parse ======

    // assume simple days:hours:minutes:seconds format
    // access successive fields
    // quit if any part of parse fails
    //
    int seconds, minutes, hours, days;

    format_flags initial_flags = is.flags();        // remember initial flags value
    long long int initialReadPosition = is.tellg();   // mark start of parse

    is >> days;
    if (!is.good())  goto parse_error;

    skipColon(is);     // skip colon, setting fail bit if not present
    is >> hours;
    if (!is.good())  goto parse_error;

    skipColon(is);     // skip colon, setting fail bit if not present
    is >> minutes;
    if (!is.good())  goto parse_error;

    skipColon(is);     // skip colon, setting fail bit if not present
    is >> seconds;
    if (!is.good())  goto parse_error;

    // === parse succeeded ===
    //
    // update internal data with scanned data, then return

    days_ = days;
    hours_ = hours;
    minutes_ = minutes;
    seconds_ = seconds;

    return;

parse_error:
    // === parse failed ====

    // --- put stream back to initial state ---
    //
    is.flags(initial_flags);
    is.seekg(initialReadPosition);

    // --- let lack of progress signal failure to complete parse  ---
    //
    return;
}

//--------------------------------------------------------------------------
//  insert - write internal elapsed time data to stream
//--------------------------------------------------------------------------
//
void elapsedTimeClass::insert(ostream& os) const
{
    os << days_ << ":" << hours_ << ":" << minutes_ << ":" << seconds_;
    return;
}

//  ==============================
//   destructors
//  ==============================
//
elapsedTimeClass::~elapsedTimeClass(void)
{
}

//=====================================
//  private member functions
//=====================================

//--------------------------------------------------------------------------
//  normalize internal time so that all values are positive, or negative
//--------------------------------------------------------------------------
//
void elapsedTimeClass::normalize(void)
{
    static const int HOURS_PER_DAY = 24;
    static const int MINUTES_PER_HOUR = 60;
    static const int SECONDS_PER_MINUTE = 60;
    static const int SECONDS_PER_HOUR = MINUTES_PER_HOUR * SECONDS_PER_MINUTE;
    static const int SECONDS_PER_DAY = SECONDS_PER_HOUR * HOURS_PER_DAY;

    // === first, reduce value of time increment to seconds ===
    //

    const long int totalSeconds =
        ((((days_ * HOURS_PER_DAY) + hours_) * MINUTES_PER_HOUR) + minutes_) * SECONDS_PER_MINUTE + seconds_;

    // === then, re-encode time increments, making all signs the same

    const long int absTotalSeconds = (totalSeconds < 0) ? -totalSeconds : totalSeconds;

    days_ = absTotalSeconds / SECONDS_PER_DAY;
    hours_ = (absTotalSeconds / SECONDS_PER_HOUR) % HOURS_PER_DAY;
    minutes_ = (absTotalSeconds / SECONDS_PER_MINUTE) % MINUTES_PER_HOUR;
    seconds_ = absTotalSeconds % SECONDS_PER_MINUTE;

    if (totalSeconds < 0)
    {
        days_ = -days_;
        hours_ = -hours_;
        minutes_ = -minutes_;
        seconds_ = -seconds_;
    }
}

//--------------------------------------------------------------------------
//  skip over a colon in the input stream  (used by parse routine)
//--------------------------------------------------------------------------
//
void elapsedTimeClass::skipColon(istream& is) const
{
    int c = is.peek();                       // get next char from in stream
    if (c >= 0)                            // if present, good.
    {
        char ch = static_cast<char>(c);    // must be ':'
        if (ch == ':')
        {
            is.ignore();                    // skip over ':'
            return;                         // continue
        }
    }

    format_flags fail_bit = static_cast<format_flags>(ios::failbit);
    is.setf(fail_bit);      // signal parse failure
}

// **************************************************************************
// auxiliary operators for elapsed time objects
// **************************************************************************

//-----------------------------------------------------------------------
//  unary negation
//-----------------------------------------------------------------------
//
elapsedTimeClass operator- (const elapsedTimeClass& value)
{
    static const elapsedTimeClass zero = (0, 0, 0, 0);
    return zero.subtract(value);
}

//-----------------------------------------------------------------------
//  addition and subtraction
//-----------------------------------------------------------------------
//
elapsedTimeClass operator+ (const elapsedTimeClass& summand, const elapsedTimeClass& addend)
{
    return summand.add(addend);
}

elapsedTimeClass operator- (const elapsedTimeClass& subtrahend, const elapsedTimeClass& minuend)
{
    return subtrahend.subtract(minuend);
}

//-----------------------------------------------------------------------
//  comparison operators
//-----------------------------------------------------------------------
//
int operator<  (const elapsedTimeClass& basis, const elapsedTimeClass& other)
{
    return basis.compare(other) == -1;   // less than
}

int operator<= (const elapsedTimeClass& basis, const elapsedTimeClass& other)
{
    return basis.compare(other) != 1;    // not greater than
}

int operator== (const elapsedTimeClass& basis, const elapsedTimeClass& other)
{
    return basis.compare(other) == 0;    // equal to
}

int operator!= (const elapsedTimeClass& basis, const elapsedTimeClass& other)
{
    return basis.compare(other) != 0;    // not equal to
}

int operator>= (const elapsedTimeClass& basis, const elapsedTimeClass& other)
{
    return basis.compare(other) != -1;    // not less than
}

int operator>  (const elapsedTimeClass& basis, const elapsedTimeClass& other)
{
    return basis.compare(other) == 1;    // greater than
}

//-----------------------------------------------------------------------
//  parsing/unparsing operators
//-----------------------------------------------------------------------
//
ostream& operator << (ostream& os, const elapsedTimeClass& basis)
{
    basis.insert(os);   // write state of basis to output stream
    return os;
}

istream& operator >> (istream& is, elapsedTimeClass& basis)
{
    basis.extract(is);    // define contents of basis from input stream
    return is;
}