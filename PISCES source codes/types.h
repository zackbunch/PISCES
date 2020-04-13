//--------------types.h---------------------------------------------------------
//
//   Author:   Phil Pfeiffer
//   Date:     February 1997
//   Last modified:  April 2020
//
//   Use #if expressions, with sizeof() macros, to obtain typedefs
//      for alternative set of platform-independent type names
//
//   Issues::
//      The size of an integer varies across platforms.
//      This set of equates attempts to achieve some platform independence
//      by creating definitions for 8-, 16-, and 32-bit ints
//
//   Assumption:  sizeof() always returns lengths in bytes
//------------------------------------------------------------------------------

#ifndef TYPES_H
#define TYPES_H

#ifndef PLATFORM_H
#include "platform.h"
#endif

// ****************************************************************
// problem with automating type definitions:
//    the AIX GNU's C++ doesn't understand sizeof
//    the Microsoft VC++ preprocessor apparently doesn't, either
//    don't understand sizeof()
//
// so we check specific platforms,
//    and hard-code assumptions for platforms
//    with C preprocessors that don't understand sizeof()
// *****************************************************************

#define HARDCODED_SIZE_TYPEDEFS()      \

// ***** allow for platforms that need hard-coded typedefs first ****

#ifdef IRIX_GNU_CPP
typedef char  int8;
typedef unsigned char unsigned8;
typedef short int16;
typedef unsigned short unsigned16;
typedef int int32;
typedef unsigned int unsigned32;

#else

#ifdef NETBSD_GNU_CPP
// ****************************************************************
// egcs 1.1.1 doesn't seem to understand sizeof() either --
//   but that's not a problem since egcs defines these types already--
//   just as a different name
// ****************************************************************
typedef int8_t int8;
typedef u_int8_t unsigned8;
typedef int16_t int16;
typedef u_int16_t unsigned16;
typedef int32_t int32;
typedef u_int32_t unsigned32;

#else

#ifdef LINUX_GNU_CPP
typedef char  int8;
typedef unsigned char unsigned8;
typedef short int16;
typedef unsigned short unsigned16;
typedef int int32;
typedef unsigned int unsigned32;

#else

#ifdef WIN_VISUAL_6_0
#include <basetsd.h>        // base type definitions

typedef char  int8;
typedef unsigned char unsigned8;
typedef short int16;
typedef unsigned short unsigned16;
typedef INT32  int32;        // INT32 defined in <basetsd.h>
typedef UINT32 unsigned32;   // UINT32 defined in <basetsd.h>

#else

// **** platforms with preprocessors that understand sizeof() ****

//  define the 8-bit typedefs
//
#if ( sizeof( int ) == 1 )
typedef int int8;
typedef unsigned unsigned8;
#else
#if ( sizeof( short ) == 1 )
typedef short int8;
typedef unsigned short unsigned8;
#else
#if ( sizeof( long ) == 1 )
typedef long int8;
typedef unsigned long unsigned8;
#else
#if ( sizeof( char ) == 1 )
typedef char int8;
typedef unsigned char unsigned8;
#endif
#endif
#endif
#endif

//  define the 16-bit typedefs
//
#if ( sizeof( int ) == 2 )
typedef int int16;
typedef unsigned unsigned16;
#else
#if ( sizeof( short ) == 2 )
typedef short int16;
typedef unsigned short unsigned16;
#else
#if ( sizeof( long ) == 2 )
typedef long int16;
typedef unsigned long unsigned16;
#else
#if ( sizeof( char ) == 2 )
typedef char int16;
typedef unsigned char unsigned16;
#endif
#endif
#endif
#endif

//  define the 32-bit typedefs
//
#if ( sizeof( int ) == 4 )
typedef int int32;
typedef unsigned unsigned32;
#else
#if ( sizeof( short ) == 4 )
typedef short int32;
typedef unsigned short unsigned32;
#else
#if ( sizeof( long ) == 4 )
typedef long int32;
typedef unsigned long unsigned32;
#else
#if ( sizeof( char ) == 4 )
typedef char int32;
typedef unsigned char unsigned32;
#endif
#endif
#endif
#endif

#endif  // #ifdef VISUAL_5_0
#endif  // #ifdef LINUX_GNU_CPP
#endif  // #ifdef NETBSD_GNU_CPP
#endif  // #ifdef IRIX_GNU_CPP

#endif  // TYPES_H
