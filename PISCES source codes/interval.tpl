//--------interval.tpl---------------------------------------------
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
// -----------------------------------------------------------------

#include "interval.h"

#ifndef BOOL_H
#include "bool.h"
#endif


// **************************************************************
//  interval class proper
// **************************************************************

//  ==============================
//   constructors
//  ==============================

// -----------------------------------------------------------------
//   constructor, first form
// -----------------------------------------------------------------
//
//  construct an interval class object with 
//      specified low and high endpoints, and
//      specified low and high endpoint attributes

template < class orderedSetType >
interval_template< orderedSetType >::interval_template
    ( const orderedSetType& lowValue, 
      const orderedSetType& highValue, 
      const intervalEndpointType lowType,   
      const intervalEndpointType highType
    ) 
    :   lowEndpoint_( lowValue ),   lowEndpointType_( lowType ),
        highEndpoint_( highValue ), highEndpointType_( highType )
{
}

// -----------------------------------------------------------------
//   constructor, second form -- copy constructor
// -----------------------------------------------------------------

template < class orderedSetType >
interval_template< orderedSetType >::interval_template( const interval_template< orderedSetType >& source )
     :  lowEndpoint_( source.lowEndpoint_ ),   
        lowEndpointType_( source.lowEndpointType_ ),
        highEndpoint_( source.highEndpoint_ ), 
        highEndpointType_( source.highEndpointType_ )
{
}


//  ==============================
//   assignment
//  ==============================

// -----------------------------------------------------------------
//   standard assignment operator--copies endpoints, endpoint types
// -----------------------------------------------------------------

template < class orderedSetType >
interval_template< orderedSetType >& interval_template< orderedSetType >::operator= ( const interval_template< orderedSetType >& source )
{
   lowEndpoint_       = source.lowEndpoint_;
   lowEndpointType_   = source.lowEndpointType_;
   highEndpoint_      = source.highEndpoint_;
   highEndpointType_  = source.highEndpointType_;

   return *this;
}


//  ==============================
//   inspectors
//  ==============================

// -----------------------------------------------------------------
//   is_in_interval - check if specified value is in interval
// -----------------------------------------------------------------

template < class orderedSetType >
int interval_template< orderedSetType >::is_in_interval( const orderedSetType& value ) const
{
   if ( lowEndpointType_ != intervalEndpointType::UNBOUNDED_ENDPOINT )
   {
       if ( value < lowEndpoint_ ) return FALSE;
       if ( value == lowEndpoint_ &&  lowEndpointType_ == intervalEndpointType::OPEN_ENDPOINT ) return FALSE;
   }

   if ( highEndpointType_ != intervalEndpointType::UNBOUNDED_ENDPOINT )
   {
       if ( highEndpoint_ < value ) return FALSE;
       if ( highEndpoint_ == value && highEndpointType_ == intervalEndpointType::OPEN_ENDPOINT ) return FALSE;
   }

   return TRUE;
}


// -----------------------------------------------------------------
//   methods for inspecting attributes of interval
// -----------------------------------------------------------------

template < class orderedSetType >
orderedSetType interval_template< orderedSetType >::lowEndpoint( void ) const 
{ 
   return lowEndpoint_; 
}

template < class orderedSetType >
intervalEndpointType interval_template< orderedSetType >::lowEndpointType( void ) const
{ 
    return lowEndpointType_; 
}

template < class orderedSetType >
orderedSetType interval_template< orderedSetType >::highEndpoint( void ) const
{ 
    return highEndpoint_; 
}

template < class orderedSetType >
intervalEndpointType interval_template< orderedSetType >::highEndpointType( void ) const
{ 
    return highEndpointType_; 
}


//  ==============================
//   operators
//  ==============================

//--------------------------------------------------------------
//  operator ==
//--------------------------------------------------------------

template < class orderedSetType >
int interval_template< orderedSetType >::operator== ( const interval_template< orderedSetType >& other ) const
{
    const int sameLowEndpoints =
        ( lowEndpointType_ == other.lowEndpointType_ ) && 
          ( lowEndpointType_ == intervalEndpointType::UNBOUNDED_ENDPOINT ||  lowEndpoint_ == other.lowEndpoint_ );
    const int sameHighEndpoints =
        ( highEndpointType_ == other.highEndpointType_ ) && 
          ( highEndpointType_ == intervalEndpointType::UNBOUNDED_ENDPOINT  ||  highEndpoint_ == other.highEndpoint_ );

    return sameLowEndpoints && sameHighEndpoints;
}

//--------------------------------------------------------------
//  operator !=
//--------------------------------------------------------------

template < class orderedSetType >
int interval_template< orderedSetType >::operator != ( const interval_template< orderedSetType >& other ) const
{
    const int differentLowEndpoints =
        ( lowEndpointType_ != other.lowEndpointType_ ) || 
          ( lowEndpointType_ != intervalEndpointType::UNBOUNDED_ENDPOINT &&  lowEndpoint_ != other.lowEndpoint_ );
    const int differentHighEndpoints =
        ( highEndpointType_ != other.highEndpointType_ ) || 
          ( highEndpointType_ != intervalEndpointType::UNBOUNDED_ENDPOINT &&  highEndpoint_ != other.highEndpoint_ );

    return differentLowEndpoints || differentHighEndpoints;
}

//--------------------------------------------------------------
//  operator insert  [unparsing operator]
//--------------------------------------------------------------

template < class orderedSetType >
void interval_template< orderedSetType >::insert ( ostream& os ) const
{
   switch ( lowEndpointType_ )
   {
       case intervalEndpointType::UNBOUNDED_ENDPOINT:   os << "[ ... ";    break;
       case intervalEndpointType::OPEN_ENDPOINT:        os << "(" << lowEndpoint_;  break;
       case intervalEndpointType::CLOSED_ENDPOINT:      os << "[" << lowEndpoint_;  break;
   }

   os << ", ";

   switch ( highEndpointType_ )
   {
       case intervalEndpointType::UNBOUNDED_ENDPOINT:   os << " ... ]";     break;
       case intervalEndpointType::OPEN_ENDPOINT:        os << highEndpoint_ << ")";  break;
       case intervalEndpointType::CLOSED_ENDPOINT:      os << highEndpoint_ << "]";  break;
   }
}


// ============
//  destructor
// ============

template < class orderedSetType >
interval_template< orderedSetType >::~interval_template( void )
{
}
