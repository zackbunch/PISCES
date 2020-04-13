//--------------platform.h------------------------------------------------------------------
//
//   Author:   Phil Pfeiffer
//   Date:     Fall 1999
//   Last modified:  April 2020
//
//   Specify features particular to a target platform for compilation
//   Currently, platform-specific differences involve
//   -.  choice of the sockets API  (for socket_apis.h/.cpp)
//   -.  decisions to disable a set of mildly annoying Visual C++ compiler warnings.
//-------------------------------------------------------------------------------------------

#ifndef PLATFORM_H
#define PLATFORM_H

// **************************************************************
//  Definitions for isolating target platform
//
//   Uncomment one, and only one, of the following definitions.
//   Make sure that the phrase "#define" is flush against the left column
//   after the desired line has been uncommented.

#define WIN_VISUAL_6_0           (1)    // VISUAL C++, version 6.0; also, VISUAL C++, .NET

//#define IRIX_GNU_CPP           (1)    // GNU C++ v2.8.1.1, under IRIX
//#define LINUX_GNU_CPP          (1)    // GNU C++ v2.8.1.1, under LINUX
//#define NETBSD_GNU_CPP         (1)    // GNU C++ v1.1.1 (egcs), under NetBSD

// **************************************************************
//  Definitions for isolating target IP version
//
//   Uncomment one, and only one, of the following definitions.
//   Make sure that the phrase "#define" is flush against the left column
//   after the desired line has been uncommented.
//
//   Reasoning: The class heirarchy becomes too complex when deriving separate
//   classes for each IP version, with too little changing to benefit
//   Result: All the version specific code os moved into the isolation layer

#define IP_V4 (1)
//#define IP_V6 (1)

// ***********************************************************8****
//   The code that follows this line should not be altered
//       by anyone who is not familiar with this package
//
//  inferences from platform definitions
//
//   Visual C++ environments:
//
//   disable warning 4290 --
//      Visual C++ compilers ignore throw() specifications
//      in function prototypes and headers
//
//   disable warning 4786 --
//      The Visual C++ debugger truncates identifier names of
//      greater than 255 characters
//
//   disable warning 4503 --
//      "decorated name length exceeded, name was truncated".
//      arises in conjunction with STL template expansions.
//
#ifdef WIN_VISUAL_6_0
#pragma warning (disable: 4290)
#pragma warning (disable: 4503)
#pragma warning (disable: 4786)

#define WINSOCK
#define WINSOCK_1_1

#endif  // WIN_VISUAL_6_0

#ifdef IRIX_GNU_CPP
#define BSD_SOCKETS
#endif

#ifdef LINUX_GNU_CPP
#define BSD_SOCKETS
#endif

#ifdef NETBSD_GNU_CPP
#define BSD_SOCKETS
#endif

#endif  // PLATFORM_H
