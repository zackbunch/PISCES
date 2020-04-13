//--------------dllinit.h----------------------------------------------------------------------------------------
//
//   Author:         Phil Pfeiffer
//   Date:           August 1999
//   Last modified:  April 2020
//
//   Automate loading and terminating supporting socket DLL(s)
//   by encapsulating DLL management logic in an object
//
//   Design notes:
//      The Sockets API, under Windows, is typically used as a DLL.
//      One disadvantage of DLLs is that all of a program's DLLs must
//          be loaded into memory before a program can run.
//
//      Newer Windows C++ environments, including (I believe) Visual C++ v5.0
//      and Borland C++ v4.52, load the Winsock DLL by default, without the need
//      for any special code in a user's application.
//
//      In Borland C++ v4.52, however, programs were required to load,
//      and unload, the Winsock DLL at need.  This logic is designed to
//      make the loading and unloading of the Winsock DLL transparent.
//
//      An instance of the DLL startup object defined here should be included
//      in the body of every constructor or free function that invokes socket
//      logic.  The logic in this startup object also performs the following services:
//
//      -.   it allows the program to specify which version of the sockets DLL to put in memory;
//      -.   it verifies that all instances of thia object request the same version of the sockets DLL;
//      -.   it counts the invocations of the class constructor, and generates
//           -.  a load request when no other copies ofthe object are active.
//           -.  an unload request when the last copy of the object is being destroyed.
//---------------------------------------------------------------------------------------------------------------

#ifndef DLLINIT_H
#define DLLINIT_H

#include <string>

#ifndef PLATFORM_H
#include "platform.h"
#endif

#ifndef STD_H
#include "std.h"
#endif

#ifndef SOCKET_APIS_H
#include "socket_apis.h"
#endif

#ifndef TYPES_H
#include "types.h"
#endif

// ***************************************************
//   DLL initialization class proper
// ***************************************************
//
// problem:
//    Winsock DLL must be started when process first initialized--
//    and should be removed from memory when sockets no longer in use.
//
// solution:
// -.   encapsulate logic for maintaining DLL as separate class
// -.   assume every object that uses DLL includes instance of constructor
// -.   use (static) count of objects in use to manage DLL
//      -.  when count goes to nonzero value, attempt to load DLL
//      -.  when count resturns to zero, clean up DLL

class sockets_dll_init_class
{
public:
    //  ==============================
    //   constructors
    //  ==============================
    //
    //  first form:
    //       allows version number of DLL, error message stream
    //       to be specified by default
    //  second form:
    //       copy constructor
    //
    //  default version of socket DLL specified as compile-time constant.

    sockets_dll_init_class(const unsigned16 = SOCKETS_DLL_VERSION);
    sockets_dll_init_class(const sockets_dll_init_class&);

    //=====================================
    //  destructor
    //=====================================
    //
    virtual ~sockets_dll_init_class(void);

private:
    //--------------------------------------
    //  member data
    //--------------------------------------

    // for tracking current version across all class instances:
    //    used to ensure that two different versions of Winsock DLL
    //    are never activated at same time
    //
    static unsigned16 activeVersion_;

    // for tracking number of active instances of class.
    // when value below drops to zero, destructor removes DLL.
    //
    static long unsigned useCount_;

    //-----------------------------------------
    //  member functions
    //------------------------------------------

    //  implements both versions of constructor
    //
    void DLLinit(const unsigned16);
};

// **************************************************************************
// exception classes for DLL routines
// **************************************************************************

#ifndef EXCEPTION_H
#include "exception.h"
#endif

// **** sole exception.here:  DLLinitException  (thrown if init step fails)  ****

class dllInitException : public basic_exception
{
public:
    dllInitException(
        const string & = "", const string & = "", const string & = "",
        const basic_exception & = basic_exception()
    );
    dllInitException(const dllInitException& source);
    virtual ~dllInitException(void);
};

// *===*====*===*====*===*====*===*====*===*====*===*====*===*====*===*====
// auxiliary operators
// *===*====*===*====*===*====*===*====*===*====*===*====*===*====*===*====

ostream& operator<<(ostream&, const dllInitException&);

#endif
