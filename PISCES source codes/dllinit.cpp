//--------------dllinit.cpp----------------------------------------------------------------------------------------------
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
//---------------------------------------------------------------------------------------------------------------------

#include "dllinit.h"
#include <assert.h>

#ifndef SOCKET_APIS_H
#include "socket_apis.h"
#endif

#ifndef SOCKET_ERROR_MESSAGE_H
#include "socket_error_message.h"
#endif

#include <sstream>   // stringstream

//  ==============================
//   static member data
//  ==============================

//  active version parameter a standard parameter to WinSock
//
unsigned16 sockets_dll_init_class::activeVersion_;

//  useCount_ counts number of active objects of type sockets_dll_init_class
//
unsigned long sockets_dll_init_class::useCount_ = 0;

//  ==============================
//   constructors
//  ==============================

// ----------------------------------------------------------------
//  first form of constructor:
//       load specified DLL (if no other objects active),
//       or verify that specified DLL has already been loaded by program
// ----------------------------------------------------------------
//
sockets_dll_init_class::sockets_dll_init_class(const unsigned16 version)
{
    DLLinit(version);
}

// ----------------------------------------------------------------
//  second form of constructor:
//       load specified DLL (if no other objects active),
//       or verify that specified DLL has already been loaded by program
// ----------------------------------------------------------------

sockets_dll_init_class::sockets_dll_init_class(const sockets_dll_init_class& source)
{
    DLLinit(source.activeVersion_);
}

//=====================================
//  destructor
//=====================================

// ----------------------------------------------------------------
//  note that one fewer object in container program is now using the DLL.
//  if no more objects need the sockets DLL, remove that DLL from memory.
// ----------------------------------------------------------------
//
sockets_dll_init_class::~sockets_dll_init_class(void)
{
    // **** **** see if we can retire object **** ****

    //  failure to match invocations of constructor, invocations of destructor implies a programmer's error
    //  assert() used here because C++ destructors should not throw exceptions.
    //
    assert(useCount_ > 0);

    useCount_--;
    if (useCount_ == 0) removeSocketsDLL();
}

// ====================================================================
//  private methods
// ====================================================================

// ----------------------------------------------------------------
//  DLLinit:  load desired version of sockets DLL into memory
//
//  On method's first invocation (i.e., use count == 0),
//     DLLinit loads DLL, saves installed version number.
//
//  On subsequent invocations (i.e., use count > 0),
//     DLLinit verifies that requested, current DLL versions are the same
// ----------------------------------------------------------------

void sockets_dll_init_class::DLLinit(const unsigned16 version)
{
    // **** **** initialize static error message information **** ****

    static const string my_name = "sockets_dll_init_class::destructor";
    static const string cantLoad = "can't install specified version";
    static const string cntrOflo = "active objects counter overflowed";

    // **** try to load program ****

    if (useCount_ == 0)
    {
        // ==== first invocation: attempt to install DLL ===

        if (installSocketsDLL(version))
        {
            // ---- install failure: error ----

            // unpack version number to give clearer error message
            //
            const unsigned versionMajor = (version >> 8);
            const unsigned versionMinor = (version % (1 << 8));

            stringstream p_message;
            p_message << "requested version = " << versionMajor << "." << versionMinor;
            throw dllInitException(my_name, cantLoad, p_message.str());
        }
        activeVersion_ = version;      // save version for later checks
    }
    else
    {
        // ==== verify that requested version = current version ====

        if (activeVersion_ != version)
        {
            //  ---- inconsistent load requests;  error ----

            // unpack version numbers to give clearer error message
            //
            const unsigned requestedMajor = (version >> 8);
            const unsigned requestedMinor = (version % (1 << 8));

            const unsigned activeMajor = (activeVersion_ >> 8);
            const unsigned activeMinor = (activeVersion_ % (1 << 8));

            stringstream p_message;
            p_message << "requested version ("
                << requestedMajor << "." << requestedMinor
                << ") differs from current version ("
                << activeMajor << "." << activeMinor << ")";
            throw dllInitException(my_name, cantLoad, p_message.str());
        }
    }

    // *** increase class instance count, checking for counter rollover
    //
    useCount_++;
    if (useCount_ == 0) throw basic_exception(my_name, cntrOflo);
}

// **************************************************************************
// exception classes for DLL routines  [boilerplate]
// **************************************************************************

dllInitException::dllInitException(
    const string& name,
    const string& short_description,
    const string& detailed_description,
    const basic_exception& prior_exception
)
    : basic_exception(name, short_description, detailed_description, prior_exception) {}

dllInitException::dllInitException(const dllInitException& source)
    : basic_exception(source) {}

dllInitException::~dllInitException(void) {}

// *===*====*===*====*===*====*===*====*===*====*===*====*===*====*===*====
// auxiliary operators  [boilerplate]
// *===*====*===*====*===*====*===*====*===*====*===*====*===*====*===*====

ostream& operator<<(ostream& os, const dllInitException& ex)
{
    ex.insert(os);
    return os;
}