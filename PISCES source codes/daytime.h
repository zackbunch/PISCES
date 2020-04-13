// ---------------------daytime.h-------------------------------------------------------------
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
// --------------------------------------------------------------------------------------------

#ifndef DAYTIME_H
#define DAYTIME_H

#include <string>
#include <iostream>

#ifndef PLATFORM_H
#include "platform.h"
#endif

#ifndef STD_H
#include "std.h"
#endif

// **************************************************************
//  "moment in time" class proper
// **************************************************************

class elapsedTimeClass;    // difference in time class--
                           // argument type for daytime-changing operators

class daytimeClass
{
public:
    //  ==============================
    //   constructors
    //  ==============================
    //
    //  -.  first form:   null constructor; inits object to current time
    //  -.  second form:  accepts user-specified time-of-day args
    //  -.  third form:   the copy constructor

    daytimeClass(void);
    daytimeClass(
        const int sec,
        const int min,
        const int hr,
        const int day,
        const int mon,
        const int yr,
        const int isDaylightSavingsTime
    );
    daytimeClass(const daytimeClass&);

    //  ==============================
    //   assignment operators
    //  ==============================
    //
    // === set the value of (*this) to specified time of day value ===
    //
    daytimeClass& operator=(const daytimeClass&);

    //  ==============================
    //   inspectors
    //  ==============================
    //
    // === get basic attributes of current moment ===

    int seconds(void) const;
    int minutes(void) const;
    int hours(void) const;
    int day(void) const;          // range: 1..31
    int month(void) const;        // range: 1..12
    int year(void) const;

    int isDaylightSavingsTime(void) const;
    int isStandardTime(void) const;

    // === get attributes computed from day/ month/ year

    int dayOfWeek(void) const;      // range: 1..7, with 1 = Sunday
    int dayOfYear(void) const;

    //  ==============================
    //   facilitators
    //  ==============================

    // === facilitators for temporal arithmetic ===

    daytimeClass add(const elapsedTimeClass&) const;
    daytimeClass subtract(const elapsedTimeClass&) const;

    // === facilitators for temporal comparisons ===
    //
    //  returns -1, 0, 1 when *this is LT, EQ, GT operand, respectively

    int compare(const daytimeClass&) const;

    // === facilitators for converting between times and strings
    //
    void insert(ostream & = cout) const;

    // ============
    //  destructor
    // ============
    //
    virtual ~daytimeClass(void);

private:
    //==========================================================
    // private member data
    //==========================================================

    //  variables that define a moment in time
    //
    int seconds_;
    int minutes_;
    int hours_;
    int day_;            // range: 1..31
    int month_;          // 0 = January, 11 = December
    int year_;           // 0 = 1900
    int isDst_;          // 0 <=> standard time

    //  variables for simplifying calendar scanning logic

    static const int daysPerMonth_NormalYear_[12];
    static const int daysPerMonth_LeapYear_[12];

    //==========================================================
    // private member functions
    //==========================================================

    //  use system calls to fetch, decode system time
    //
    void setTimeToSystemTime(void);

    //  initialize member data, auditing values
    //
    void setSeconds(const int);
    void setMinutes(const int);
    void setHours(const int);
    void setDayMonthYear(const int, const int, const int);

    //  do leap-year test on specified year
    //
    int isLeapYear(const int) const;

    //==========================================================
    // unsupported private member functions
    //==========================================================

    void extract(istream & = cin);   // not yet implemented
};

// **************************************************************************
// auxiliary operators for daytime objects
// **************************************************************************

//-----------------------------------------------------------------------
//  addition and subtraction
//-----------------------------------------------------------------------
//
daytimeClass operator+ (const daytimeClass& summand, const elapsedTimeClass& addend);

daytimeClass operator+ (const elapsedTimeClass& addend, const daytimeClass& summand);

daytimeClass operator- (const daytimeClass& subtrahend, const elapsedTimeClass& minuend);

//-----------------------------------------------------------------------
//  arithmetic assignment
//-----------------------------------------------------------------------
//
daytimeClass operator+= (daytimeClass& summand, const elapsedTimeClass& addend);

daytimeClass operator-= (daytimeClass& subtrahend, const elapsedTimeClass& minuend);

//-----------------------------------------------------------------------
//  comparison operators
//-----------------------------------------------------------------------
//
int operator <  (const daytimeClass& basis, const daytimeClass& other);
int operator <= (const daytimeClass& basis, const daytimeClass& other);
int operator == (const daytimeClass& basis, const daytimeClass& other);
int operator != (const daytimeClass& basis, const daytimeClass& other);
int operator >= (const daytimeClass& basis, const daytimeClass& other);
int operator >  (const daytimeClass& basis, const daytimeClass& other);

//-----------------------------------------------------------------------
//  parsing/unparsing operators
//-----------------------------------------------------------------------

ostream& operator << (ostream&, const daytimeClass&);
istream& operator >> (istream&, daytimeClass&);

// **************************************************************************
// exception classes for daytime objects
// **************************************************************************

#ifndef EXCEPTION_H
#include "exception.h"
#endif

// sole exception.here:  daytimeException
//

class daytimeException : public basic_exception
{
public:
    daytimeException(const string & = "", const string & = "", const string & = "");
    daytimeException(const daytimeException&);

    virtual ~daytimeException(void);
};

// *===*====*===*====*===*====*===*====*===*====*===*====*===*====*===*====
// auxiliary operators
// *===*====*===*====*===*====*===*====*===*====*===*====*===*====*===*====

ostream& operator<<(ostream&, const daytimeException&);

#endif
