// ---------------------daytime.cpp---------------------------------------------
//
//  Author:  Phil Pfeiffer
//  Date:    August 1999
//  Last modified:  April 2020
//
//  Implement a basic Western-style time of day, under lots of simplifications:
//  -.   no support for dates before Jan. 1, 1900
//  -.   daylight savings time information ignored in comparison operators
//  -.   no check for overflow in time computations
//  -.   extract() facilitator not implemented
//------------------------------------------------------------------------------

#include "daytime.h"

#ifndef BOOL_H
#include "bool.h"
#endif

#include <iomanip>

#ifndef STD_H
#include "std.h"
#endif

#include <time.h>    // system timekeeping routines
#include <sstream>   // stringstream

#ifndef ELAPSED_TIME_H
#include "elapsed_time.h"   // elapsed time class
#endif

//*****************************************************
//  class method definitions
//*****************************************************

//  ==============================
//   static data
//  ==============================

const int daytimeClass::daysPerMonth_NormalYear_[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
const int daytimeClass::daysPerMonth_LeapYear_[12] = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

//  ==============================
//   constructors
//  ==============================

//--------------------------------------------------------------------------
//  constructor, first form--
//      initialize contents of time of day object to internal time.
//--------------------------------------------------------------------------
//
daytimeClass::daytimeClass(void)
{
    setTimeToSystemTime();
}

//--------------------------------------------------------------------------
//  constructor, second form--
//      initialize contents of time of day object to specified time.
//--------------------------------------------------------------------------

daytimeClass::daytimeClass(
    const int seconds,
    const int minutes,
    const int hours,
    const int day,
    const int month,
    const int year,
    const int isDaylightSavingsTime
)
{
    // use class set methods to init and audit internal time-of-day structure
    //
    setSeconds(seconds);
    setMinutes(minutes);
    setHours(hours);
    setDayMonthYear(day, month, year);

    isDst_ = (isDaylightSavingsTime != FALSE);
}

//--------------------------------------------------------------------------
//  constructor, third form--copy constructor
//--------------------------------------------------------------------------

daytimeClass::daytimeClass(const daytimeClass& sourceTime)
    : seconds_(sourceTime.seconds_),
    minutes_(sourceTime.minutes_),
    hours_(sourceTime.hours_),
    day_(sourceTime.day_),
    month_(sourceTime.month_),
    year_(sourceTime.year_),
    isDst_(sourceTime.isDst_)
{
}

//  ==============================
//   assignment operators
//  ==============================
//
// === set the value of (*this) to specified time of day value ===

daytimeClass& daytimeClass::operator=(const daytimeClass& sourceTime)
{
    seconds_ = sourceTime.seconds_;
    minutes_ = sourceTime.minutes_;
    hours_ = sourceTime.hours_;
    day_ = sourceTime.day_;
    month_ = sourceTime.month_;
    year_ = sourceTime.year_;
    isDst_ = sourceTime.isDst_;
    return *this;
}

//  ==============================
//   inspectors
//  ==============================

//--------------------------------------------------------------------------
//  inspectors for internal state variables
//--------------------------------------------------------------------------
//
//   one inspector provided below for each field in the structure

int daytimeClass::seconds(void) const
{
    return seconds_;
}

int daytimeClass::minutes(void) const
{
    return minutes_;
}

int daytimeClass::hours(void) const
{
    return hours_;
}

int daytimeClass::day(void) const
{
    return day_ + 1;     // stored value equates 0 to 1st day of month
}

int daytimeClass::month(void) const
{
    return month_ + 1;    // stored value equates 0 to January
}

int daytimeClass::year(void) const
{
    return year_ + 1900;        // stored value equates 0 to 1900
}

int daytimeClass::isDaylightSavingsTime(void) const
{
    return isDst_ != FALSE;
}

int daytimeClass::isStandardTime(void) const
{
    return isDst_ == FALSE;
}

//--------------------------------------------------------------------------
//  inspectors for information derived from day/ month/ year
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
//  getDayOfWeek() returns 1..7
//     according to whether current day is Sunday, Monday, ... Saturday
//--------------------------------------------------------------------------

int daytimeClass::dayOfWeek(void) const
{
    static const int JAN1ST1900 = 1;           // January 1, 1900 was a Monday

    // ===  weekdays progress at a rate of 1 day / year,
    //      plus one extra day / year for every (Gregorian calendar) leap year
    //
    //  note:  year_ contains years since 1900

    const int julianLeapYearsSince1900 = year_ / 4;
    const int centuriesSince1900 = year_ / 100;
    const int leapCenturiesSince1900 = (year_ + 300) / 400;

    const int leapYearsSince1900 = julianLeapYearsSince1900 - centuriesSince1900 + leapCenturiesSince1900;

    const int jan1stOfCurrentYear = (JAN1ST1900 + (year_ % 7) + (leapYearsSince1900 % 7)) % 7;

    //  finally, return day of week based on current day of year
    //
    const int dayOfYearZeroOffset = dayOfYear() - 1;
    const int dayOfWeekZeroOffset = (jan1stOfCurrentYear + dayOfYearZeroOffset) % 7;

    return dayOfWeekZeroOffset + 1;    // return value in range 1..7
}

//--------------------------------------------------------------------------
//  getDayOfWeek() returns 1..366
//     according to whether current day is 1st ... 366th day of year
//--------------------------------------------------------------------------

int daytimeClass::dayOfYear(void) const
{
    // start with number of days in current month
    //
    int dayOfYearZeroOffset = day_;

    // count up days in full months preceding current
    //
    const int* const daysPerMonth = isLeapYear(year_) ? daysPerMonth_LeapYear_ : daysPerMonth_NormalYear_;

    for (int nextMonth = 0; nextMonth < month_; nextMonth++)
    {
        dayOfYearZeroOffset += daysPerMonth[nextMonth];
    }

    // add in days elapsed in current month
    //
    return dayOfYearZeroOffset + 1;     // return value in range 1..366
}

//  ==============================
//   facilitators
//  ==============================

//  =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//    arithmetic facilitators
//  =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

//--------------------------------------------------------------------------
//  advance current time by specified increment
//--------------------------------------------------------------------------

daytimeClass daytimeClass::add(const elapsedTimeClass& addend) const
{
    // **** initialize static constant supporting data  ****
    //
    static const elapsedTimeClass zeroElapsedTime = elapsedTimeClass(0, 0, 0, 0);

    // **** handle negative addends using subtraction algorithm ****
    //
    if (addend < zeroElapsedTime) return subtract(-addend);

    // **** compute sum in two addition steps ****

    // === init computation

    daytimeClass result(*this);           // start w. *this as initial result

    const int addsecs = addend.seconds();   // aliases for addend components
    const int addmins = addend.minutes();
    const int addhrs = addend.hours();
    const int adddays = addend.days();

    // === first, add in hours/seconds/minutes ===
    //
    const elapsedTimeClass partialSum = elapsedTimeClass(seconds_, minutes_, hours_, 0) + elapsedTimeClass(addsecs, addmins, addhrs, 0);

    result.seconds_ = partialSum.seconds();
    result.minutes_ = partialSum.minutes();
    result.hours_ = partialSum.hours();

    // === then, add in days, one day at a time ===
    //
    //  repeated normalization of days, months, years slow, but simple to code

    const int* daysPerMonthInCurrentYear = isLeapYear(year_) ? daysPerMonth_LeapYear_ : daysPerMonth_NormalYear_;

    int daysToAdd = adddays + partialSum.days();

    while (daysToAdd > 0)
    {
        daysToAdd--;
        result.day_++;

        if (result.day_ == daysPerMonthInCurrentYear[month_])
        {
            result.day_ = 0;
            result.month_++;

            if (result.month_ == 12)
            {
                result.month_ = 0;
                result.year_++;
                daysPerMonthInCurrentYear = isLeapYear(result.year_) ? daysPerMonth_LeapYear_ : daysPerMonth_NormalYear_;
            }
        }
    }
    return result;
}

//--------------------------------------------------------------------------
//  roll back current time by specified decrement
//--------------------------------------------------------------------------

daytimeClass daytimeClass::subtract(const elapsedTimeClass& minuend) const
{
    // **** initialize static constant supporting data  ****
    //
    static const string my_name = "daytimeClass::subtract";
    static const string minuendError = "minuend too large";
    static const elapsedTimeClass zeroElapsedTime = elapsedTimeClass(0, 0, 0, 0);

    // **** handle negative minuends using addition algorithm ****
    //
    if (minuend < zeroElapsedTime) return add(-minuend);

    // **** compute subtraction in two differencing steps ****

    // === init computation

    daytimeClass result(*this);           // start w. *this as initial result

    const int subsecs = minuend.seconds();   // aliases for addend components
    const int submins = minuend.minutes();
    const int subhrs = minuend.hours();
    const int subdays = minuend.days();

    // === first, subtract out hours/seconds/minutes ===
    //
    const elapsedTimeClass partialDiff = elapsedTimeClass(seconds_, minutes_, hours_, 1) - elapsedTimeClass(subsecs, submins, subhrs, 0);

    result.seconds_ = partialDiff.seconds();
    result.minutes_ = partialDiff.minutes();
    result.hours_ = partialDiff.hours();

    // === then, subtract out days, one day at a time ===
    //
    //  repeated normalization of days, months, years slow, but simple to code

    const int* daysPerMonthInCurrentYear = isLeapYear(year_) ? daysPerMonth_LeapYear_ : daysPerMonth_NormalYear_;

    int daysToSubtract = subdays - 1 + partialDiff.days();

    while (daysToSubtract > 0)
    {
        daysToSubtract--;
        if (result.day_ == 0)
        {
            if (result.month_ == 0)
            {
                if (result.year_ == 0)
                {
                    // ** can't roll back before 1/1/1900: print error, quit **
                    //
                    stringstream p_message;
                    p_message << "subtracting " << minuend << " from current date would yield date before " << " 1 Jan 1900";
                    throw daytimeException(my_name, minuendError, p_message.str());
                }
                result.year_--;
                daysPerMonthInCurrentYear = isLeapYear(result.year_) ? daysPerMonth_LeapYear_ : daysPerMonth_NormalYear_;
                result.month_ = 12;
            }
            result.month_--;
            result.day_ = daysPerMonthInCurrentYear[result.month_];
        }
        result.day_--;
    }
    return result;
}

//  =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//    logical operation facilitators
//  =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

//--------------------------------------------------------------------------
//  compare() - returns -1, 0, 1 when *this is LT, EQ, GT operand, respectively
//--------------------------------------------------------------------------

int daytimeClass::compare(const daytimeClass& other) const
{
    if (year_ < other.year_) return -1;
    if (year_ > other.year_) return 1;

    if (month_ < other.month_) return -1;
    if (month_ > other.month_) return 1;

    if (day_ < other.day_) return -1;
    if (day_ > other.day_) return 1;

    if (hours_ < other.hours_) return -1;
    if (hours_ > other.hours_) return 1;

    if (minutes_ < other.minutes_) return -1;
    if (minutes_ > other.minutes_) return 1;

    if (seconds_ < other.seconds_) return -1;
    if (seconds_ > other.seconds_) return 1;

    return 0;
}

//  =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//    parsing/unparsing facilitators
//  =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

// === facilitators for converting between times and strings

//--------------------------------------------------------------------------
//  extract() -- retrieve time of day from text stream (not yet implemented)
//--------------------------------------------------------------------------
//
void daytimeClass::extract(istream& is)
{
    throw basic_exception("daytimeClass::extract - not yet implemented");
}

//--------------------------------------------------------------------------
//  insert -- dump internal data to specified stream
//--------------------------------------------------------------------------
//
// not const because getDayOfWeek() is not const

void daytimeClass::insert(ostream& os) const
{
    switch (dayOfWeek())
    {
    case  1:  os << "Sun ";  break;
    case  2:  os << "Mon ";  break;
    case  3:  os << "Tue ";  break;
    case  4:  os << "Wed ";  break;
    case  5:  os << "Thu ";  break;
    case  6:  os << "Fri ";  break;
    case  7:  os << "Sat ";  break;
    default:  os << "??? ";  break;
    }

    os << setw(2) << setfill(' ') << (day_ + 1) << " ";

    switch (month_)
    {
    case  0:  os << "Jan ";  break;
    case  1:  os << "Feb ";  break;
    case  2:  os << "Mar ";  break;
    case  3:  os << "Apr ";  break;
    case  4:  os << "May ";  break;
    case  5:  os << "Jun ";  break;
    case  6:  os << "Jul ";  break;
    case  7:  os << "Aug ";  break;
    case  8:  os << "Sep ";  break;
    case  9:  os << "Oct ";  break;
    case 10:  os << "Nov ";  break;
    case 11:  os << "Dec ";  break;
    default:  os << "??? ";  break;
    }

    os << setw(4) << (year_ + 1900) << "  "
        << setw(2) << setfill('0') << hours_ << ":"
        << setw(2) << setfill('0') << minutes_ << ":"
        << setw(2) << setfill('0') << seconds_ << " "
        << (isDst_ ? " daylight savings time " : " standard time ");
}

//  ==============================
//   destructors
//  ==============================
//
daytimeClass::~daytimeClass(void)
{
}

//=====================================
//  private member functions
//=====================================

//--------------------------------------------------------------------------
//  setTimeToSystemTime() -
//       use system time of day to initialize object contents.
//       throw exception if initialization unsuccessful
//--------------------------------------------------------------------------
//
void daytimeClass::setTimeToSystemTime(void)
{
    // === initialize static constant supporting data  ===
    //
    static const string my_name = "daytimeClass::setTimeToSystemTime";
    static const string noAccess = "can't access time of day";
    static const string noDecode = "can't decode system-supplied time of day";

    //  === get time, as seconds after 1 Jan 1970,
    //      using time() system call, from <time.h>
    //
    const time_t timeOfDay = time(0);
    if (timeOfDay == -1) throw daytimeException(my_name, noAccess);

    // ===  convert time retrieved to secs/mins/hrs/etc. format,
    //      using localtime() system call, from <time.h>
    //
    struct tm decodedTime;
    localtime_s(&decodedTime, &timeOfDay);

    // ==== copy time data from static structure returned by localtime()
    //
    seconds_ = decodedTime.tm_sec;
    minutes_ = decodedTime.tm_min;
    hours_ = decodedTime.tm_hour;

    day_ = decodedTime.tm_mday - 1;   // time() treats day 1 as 1
    month_ = decodedTime.tm_mon;      // time() treats month 1 as 0
    year_ = decodedTime.tm_year;      // time() treats 1900 as 0

    isDst_ = decodedTime.tm_isdst;
}

//--------------------------------------------------------------------------
//  setSeconds() - set seconds to user-specified value.
//                 throw exception if value invalid
//--------------------------------------------------------------------------

void daytimeClass::setSeconds(const int seconds)
{
    // **** initialize unchanging supporting data once  ****
    //
    static const string my_name = "daytimeClass::setSeconds";
    static const string bad_arg = "seconds out of range (0..59)";

    // **** update seconds, if argument valid ***
    //
    if (seconds < 0 || seconds > 59)
    {
        stringstream p_message;
        p_message << "supplied value = " << seconds;
        throw daytimeException(my_name, bad_arg, p_message.str());
    }
    seconds_ = seconds;
}

//--------------------------------------------------------------------------
//  setMinutes() - set minutes to user-specified value
//                 throw exception if value invalid
//--------------------------------------------------------------------------

void daytimeClass::setMinutes(const int minutes)
{
    // **** initialize static constant supporting data  ****
    //
    static const string my_name = "daytimeClass::setMinutes";
    static const string bad_arg = "minutes out of range (0..59)";

    // **** update minutes, if argument valid ***
    //
    if (minutes < 0 || minutes > 59)
    {
        stringstream p_message;
        p_message << "supplied value = " << minutes;
        throw daytimeException(my_name, bad_arg, p_message.str());
    }
    minutes_ = minutes;
}

//--------------------------------------------------------------------------
//  setHours() - set hours to user-specified value
//                 throw exception if value invalid
//--------------------------------------------------------------------------

void daytimeClass::setHours(const int hours)
{
    // **** initialize static constant supporting data  ****
    //
    static const string my_name = "daytimeClass::setHours";
    static const string bad_arg = "hours out of range (0..23)";

    // **** update hours, if argument valid ***
    //
    if (hours < 0 || hours > 23)
    {
        stringstream p_message;
        p_message << "supplied value = " << hours;
        throw daytimeException(my_name, bad_arg, p_message.str());
    }
    hours_ = hours;
}

//--------------------------------------------------------------------------
//  setDayMonthYear() - set day, month, and year to user-specified value
//                      throw exception if value invalid
//--------------------------------------------------------------------------

void daytimeClass::setDayMonthYear(
    const int specifiedDay,
    const int specifiedMonth,
    const int specifiedYear)
{
    // **** initialize static constant supporting data  ****
    //
    static const string my_name = "daytimeClass::setDayMonthYear";
    static const string badYear = "years before 1900 not supported";
    static const string badMonth = "month out of range (1..12)";
    static const string badDate = "invalid date";

    //  === process year parameter ===
    //
    int updatedYear;
    if (specifiedYear == -1)        // -1 forces default to current year
        updatedYear = year_ + 1900;    // year stored with 0 = 1900
    else
        if (specifiedYear >= 1900)
            updatedYear = specifiedYear;
        else
        {
            stringstream p_message;
            p_message << "supplied value = " << specifiedYear;
            throw daytimeException(my_name, badYear, p_message.str());
        }

    //  === process month parameter ===
    //
    int updatedMonth;
    if (specifiedMonth == -1)     // -1 forces default to current month
        updatedMonth = month_ + 1;  // month stored with 0 = 1st month
    else
        if (specifiedMonth < 1 || specifiedMonth > 12)
            updatedMonth = specifiedMonth;
        else
        {
            stringstream p_message;
            p_message << "supplied value = " << specifiedMonth;
            throw daytimeException(my_name, badMonth, p_message.str());
        }

    //  === process day parameter ===
    //
    int updatedDay;
    if (specifiedDay == -1)     // -1 forces default to current day
        updatedDay = day_ + 1;    // day stored with 0 = 1st day
    else
        updatedDay = specifiedDay;

    if (specifiedDay != -1 || specifiedMonth != -1 || specifiedYear != -1)
    {
        // something changed; audit day
        //
        const int* const daysPerMonth = isLeapYear(year_) ? daysPerMonth_LeapYear_ : daysPerMonth_NormalYear_;
        if (updatedDay < 1 || updatedDay > daysPerMonth[updatedMonth])
        {
            stringstream p_message;
            p_message << updatedMonth << "/" << updatedDay << "/" << updatedYear;
            throw daytimeException(my_name, badDate, p_message.str());
        }
    }

    day_ = updatedDay - 1;    // logic equates 0 with day 1
    month_ = updatedMonth - 1;    // logic equates 0 with January
    year_ = updatedYear - 1900;    // logic equates 0 with 1900
}

//--------------------------------------------------------------------------
//  isLeapYear() - do the Gregorian calendar leap year check
//--------------------------------------------------------------------------
//
int daytimeClass::isLeapYear(int year) const
{
    if ((year % 400) == 0)  return TRUE;    // every 4th century is leap yr
    if ((year % 100) == 0)  return FALSE;   // other centuries aren't
    if ((year % 4) == 0)  return TRUE;    // every 4th noncentury is leap
    return FALSE;                               // o.w., not a leap year
}

// **************************************************************************
// auxiliary operators for daytime objects
// **************************************************************************

//-----------------------------------------------------------------------
//  addition and subtraction
//-----------------------------------------------------------------------
//
daytimeClass operator+ (const daytimeClass& summand, const elapsedTimeClass& addend)
{
    return summand.add(addend);
}

daytimeClass operator+ (const elapsedTimeClass& addend, const daytimeClass& summand)
{
    return summand.add(addend);
}

daytimeClass operator- (const daytimeClass& subtrahend, const elapsedTimeClass& minuend)
{
    return subtrahend.subtract(minuend);
}

//-----------------------------------------------------------------------
//  arithmetic assignment
//-----------------------------------------------------------------------

daytimeClass operator+= (daytimeClass& summand, const elapsedTimeClass& addend)
{
    summand = summand.add(addend);
    return summand;
}

daytimeClass operator-= (daytimeClass& subtrahend, const elapsedTimeClass& minuend)
{
    subtrahend = subtrahend.subtract(minuend);
    return subtrahend;
}

//-----------------------------------------------------------------------
//  comparison operators
//-----------------------------------------------------------------------
//
int operator<  (const daytimeClass& basis, const daytimeClass& other)
{
    return basis.compare(other) == -1;   // less than
}

int operator<= (const daytimeClass& basis, const daytimeClass& other)
{
    return basis.compare(other) != 1;    // not greater than
}

int operator== (const daytimeClass& basis, const daytimeClass& other)
{
    return basis.compare(other) == 0;    // equal to
}

int operator!= (const daytimeClass& basis, const daytimeClass& other)
{
    return basis.compare(other) != 0;    // not equal to
}

int operator>= (const daytimeClass& basis, const daytimeClass& other)
{
    return basis.compare(other) != -1;    // not less than
}

int operator>  (const daytimeClass& basis, const daytimeClass& other)
{
    return basis.compare(other) == 1;    // greater than
}

//-----------------------------------------------------------------------
//  parsing/unparsing operators
//-----------------------------------------------------------------------
//
ostream& operator << (ostream& os, const daytimeClass& basis)
{
    basis.insert(os);   // write state of basis to output stream
    return os;
}

// **************************************************************************
// exception classes for daytime routines  [boilerplate]
// **************************************************************************

daytimeException::daytimeException(const string& name, const string& short_message, const string& long_message)
    : basic_exception(name, short_message, long_message) {}

daytimeException::daytimeException(const daytimeException& source)
    : basic_exception(source) {}

daytimeException::~daytimeException(void)
{}

// *===*====*===*====*===*====*===*====*===*====*===*====*===*====*===*====
// auxiliary operators  [boilerplate]
// *===*====*===*====*===*====*===*====*===*====*===*====*===*====*===*====

ostream& operator<<(ostream& os, const daytimeException& ex)
{
    ex.insert(os);
    return os;
}