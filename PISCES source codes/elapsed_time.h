// ---------------------elapsed_time.h-------------------------------------------------
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
//-------------------------------------------------------------------------------------

#ifndef ELAPSED_TIME_H
#define ELAPSED_TIME_H

#include <string>
#include <iostream>

#ifndef PLATFORM_H
#include "platform.h"
#endif

#ifndef STD_H
#include "std.h"
#endif

// **************************************************************
//  elapsed time class
// **************************************************************

class elapsedTimeClass
{
public:
    //  ==============================
    //   constructors
    //  ==============================
    //
    //  -.  first form:   init elapsed time object to user-specified values
    //  -.  second form:  copy constructor

    elapsedTimeClass(
        const int secs = 0,
        const int mins = 0,
        const int hours = 0,
        const int days = 0
    );
    elapsedTimeClass(const elapsedTimeClass&);

    //  ==============================
    //   assignment operators
    //  ==============================
    //
    // standard object copy
    //
    elapsedTimeClass& operator=(const elapsedTimeClass&);

    //  ==============================
    //   inspectors
    //  ==============================
    //
    // === get specified attributes of current time of day ===

    int seconds(void) const;
    int minutes(void) const;
    int hours(void) const;
    int days(void) const;

    //  ==============================
    //   facilitators
    //  ==============================

    // === facilitators for temporal arithmetic ===
    //
    elapsedTimeClass add(const elapsedTimeClass&) const;
    elapsedTimeClass subtract(const elapsedTimeClass&) const;

    // === facilitators for temporal comparisons ===
    //
    //  returns -1, 0, 1 when *this is LT, EQ, GT operand, respectively

    int compare(const elapsedTimeClass&) const;

    // === facilitators for converting between times and strings
    //
    //  extract expects an argument of the form days:hours:months:seconds,
    //         where all fields must be present
    //         and individual values may be positive or negative.
    //  insert displays the contents of this object in a similar format
    //
    void extract(istream & = cin);
    void insert(ostream & = cout) const;

    // ============
    //  destructor
    // ============
    //
    virtual ~elapsedTimeClass(void);

private:
    //==========================================================
    // private member data
    //==========================================================
    //
    int seconds_, minutes_, hours_, days_;

    //==========================================================
    // private methods
    //==========================================================
    //
    // convert elapsed time into one of two forms:
    //
    //  -.  positive seconds, minutes, hours, days
    //  -.  negative seconds, minutes, hours, days
    //
    void normalize(void);

    // bypass colon in input stream,
    // setting stream's fail bit if colon not present
    //
    void skipColon(istream& is) const;
};

// **************************************************************************
// auxiliary operators for elapsed time objects
// **************************************************************************

//  elapsed time arithmetic
//
elapsedTimeClass operator+ (const elapsedTimeClass&, const elapsedTimeClass&);
elapsedTimeClass operator- (const elapsedTimeClass&);
elapsedTimeClass operator- (const elapsedTimeClass&, const elapsedTimeClass&);

//  elapsed time comparison operators
//
int operator<  (const elapsedTimeClass&, const elapsedTimeClass&);
int operator<= (const elapsedTimeClass&, const elapsedTimeClass&);
int operator== (const elapsedTimeClass&, const elapsedTimeClass&);
int operator!= (const elapsedTimeClass&, const elapsedTimeClass&);
int operator>= (const elapsedTimeClass&, const elapsedTimeClass&);
int operator>  (const elapsedTimeClass&, const elapsedTimeClass&);

//  operators for copying internal state between elapsed time object, stream
//
ostream& operator << (ostream&, const elapsedTimeClass&);
istream& operator >> (istream&, elapsedTimeClass&);

#endif
