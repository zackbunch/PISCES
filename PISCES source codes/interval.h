//--------interval.h---------------------------------------------------------------------
//
//   Author:   Phil Pfeiffer
//   Date:     July 1999
//   Last modified:  April 2020
//
//   A template that implements classes that represent ranges of values.
//
//   Design Notes:
//      The template parameter, orderedSetType, is a base class from which
//      interval values are drawn.  orderedSetType must be a totally ordered
//      class that supports overloaded <, ==, and unparsing (<<) operators.
//
//      An interval is a pair of endpoints--a lower and higher class element,
//      together with an attribute for that characterizes each interval endpoint:
//      -.   unbounded:  the endpoint is ignored for purposes of interval testing
//      -.   closed:     the endpoint is included in the interval
//      -.   open:       the endpoint is omitted from the interval
// -----------------------------------------------------------------------------------------

#ifndef INTERVAL_H
#define INTERVAL_H

#include <iostream>

#ifndef PLATFORM_H
#include "platform.h"
#endif

#ifndef STD_H
#include "std.h"
#endif

// **************************************************************
//  interval class proper
// **************************************************************

enum class intervalEndpointType { UNBOUNDED_ENDPOINT, CLOSED_ENDPOINT, OPEN_ENDPOINT };

template < class orderedSetType >
class interval_template
{
public:
    //  ==============================
    //   constructors
    //  ==============================

    //   first form:   construct an interval class object with
    //      specified low and high endpoints, and
    //      specified low and high endpoint attributes

    //   second form:  copy constructor
    //
    interval_template(
        const orderedSetType & = orderedSetType(),
        const orderedSetType & = orderedSetType(),
        const intervalEndpointType = intervalEndpointType::CLOSED_ENDPOINT,
        const intervalEndpointType = intervalEndpointType::CLOSED_ENDPOINT
    );

    interval_template(const interval_template< orderedSetType >&);

    //  ==============================
    //   assignment
    //  ==============================

    //   standard assignment operator

    interval_template< orderedSetType >& operator= (const interval_template< orderedSetType >&);

    //  ==============================
    //   inspectors
    //  ==============================

    //   inspect interval endpoints

    orderedSetType        lowEndpoint(void) const;
    intervalEndpointType  lowEndpointType(void) const;

    orderedSetType        highEndpoint(void) const;
    intervalEndpointType  highEndpointType(void) const;

    //   check if specified value is in interval

    int is_in_interval(const orderedSetType&) const;

    //  ====================================
    //   operators
    //  ====================================

    //   comparison:  determine interval equality

    int operator== (const interval_template< orderedSetType >&) const;
    int operator!= (const interval_template< orderedSetType >&) const;

    //   unparsing:  display interval on specified ostream

    void insert(ostream & = cout) const;

    // ============
    //  destructor
    // ============
    //
    virtual ~interval_template(void);

private:
    // ==========================================================
    //   private member data
    // ==========================================================
    //
    //  definining interval and attributes of interval

    orderedSetType lowEndpoint_;             // low value in interval
    intervalEndpointType lowEndpointType_;   // OPEN_ENDPOINT, etc.

    orderedSetType highEndpoint_;            // high value in interval
    intervalEndpointType highEndpointType_;  // OPEN_ENDPOINT, etc.
};

// **************************************************************
//  auxiliary operators
// **************************************************************

//  stream extraction (unparsing) operators for templates instantiated above

template < class orderedSetType >
ostream& operator<< (ostream& os, const interval_template< orderedSetType >& source)
{
    source.insert(os);
    return os;
}

#endif
