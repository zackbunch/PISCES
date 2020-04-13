//--------------socket.h------------------------------------------------------------------------
//
//   Authors:  Phil Pfeiffer, Sridhar Koritala
//   Date:     September 1999
//   Last modified:  April 2020
//
//   Simplify task of managing a communication by implementing socket functionality as an object.
//
//   Background:
//      In socket-based network programming, communications proceed
//      with the aid of **sockets**:  endpoints for connecting processes.
//
//      The two most commonly used socket APIs are BSD and Winsock.
//      The two are similar, but not the same.  Both APIs, furthermore,
//      are procedural in character.
//
//   Idea:
//      This module defines an object that captures features of BSD/
//      Winsock sockets common to all TCP socket APIs:  namely,
//
//       -.   the ability to identify port associated with current socket;
//       -.   the ability to manipulate a socket's mode of operation; and
//       -.   the ability to terminate socket processing.
//
//     The send(), recv(), sendto(), and recvfrom() socket communication
//     primitives should, in an ideal programming language, be defined
//     as variadic parameters like printf--
//
//        i.e., functions that accept a varying number of parameters
//        of varying types.
//
//     Rationale for statement: convenience and clarity that would be gained
//     by supplying exactly as many arguments to send/recv/etc. as an
//     exchange between client and server required.
//
//     To get this effect of variadic functions in C++, where functions must
//     must take a fixed number of arguments of fixed types, it is standard
//     to use "thing pointers"-- arguments of type "void *" which are recast
//     to objects of an "implicitly understood" type, after being passed to
//     socket objects.
//
//     Since "thing pointers" are not type safe, this module uses a convention
//     for "faking" variadic functions that's a little more structured:
//
//         STL "maps" -- i.e., associative arrays -- that
//             associate strings with pointers of type "void *".
//
//     Rationale:
//         using strings to tag arguments is safer than simply using context
//         (i.e., position within vector) to identify argument type.
//
//  Key parameters for class send/receive routines:
//
//     "char buffer"  -> (void *) char*
//     "buffer size"  -> (void *) unsigned*
//
//        identifies buffer+buffer size for stored message
//
//     "service access point" -> (void *) socket_service_access_point_class
//
//        holds host, IP address associated with message exchange
//        (sendto()/ recvfrom() only)
//
//     "receive count" -> (void *) unsigned *
//
//        captures number of characters actually received
//            (optional parameter for recv(), recvfrom())
//
//-----------------------------------------------------------------------------------------------

#include <string>
#include <map>
#include <iostream>

#ifndef SOCKET_H
#define SOCKET_H

#ifndef PLATFORM_H
#include "platform.h" 
#endif

#ifndef STD_H
#include "std.h"
#endif

#ifndef TYPES_H
#include "types.h"
#endif

#ifndef SOCKET_APIS_H
#include "socket_apis.h"           // portable socket APIs module
#endif

#ifndef DLLINIT_H
#include "dllinit.h"            // self-loading DLL module
#endif

#ifndef SERVICE_ACCESS_POINT_H
#include "service_access_point.h"            // service access point type
#endif

// *******************************************************************
//  socket class object
// *******************************************************************
//
//  definition of a socket:  an object that supports
//
//  -.   a constructor that initializes socket processing:
//         here, that ensures that supporting DLLs are resident.
//
//  -.   a closesocket() method, which terminates socket processing
//
//  -.   three public methods for inspecting socket attributes:
//       -.   bytes_pending( ) - retrieves bytes for reading on port
//       -.   service_access_point( ) - retrieves IP/DNS/port for socket
//       -.   peer_service_access_point( ) - retrieves IP/DNS/port for peer socket
//                                        (i.e., connected socket)
//
//  -.   two public methods for altering socket attributes:
//       -.   set_blocking( )    - makes socket API calls blocking (default)
//       -.   setNonBlocking( ) - makes socket API calls nonblocking
//
//  -.   four public methods for supporting transmissions:
//       -.   recv() - receive data from an implicit sender
//       -.   recvfrom() - receive data, returning ident of sender
//       -.   send() - send data to an implicit receiver
//       -.   sendto() - send data to a specific receiver
//       =>   all four of these methods take one parameter:
//            a map<> that maps strings that name parameters to void * pointers.
//            functions are defined more carefully in a derived class.
//
//  -.   four protected methods for supporting transmissions:
//       -.   recv() - receive data from an implicit sender
//       -.   recvfrom() - receive data, returning ident of sender
//       -.   send() - send data to an implicit receiver
//       -.   sendto() - send data to a specific receiver
//       =>   these methods, whose signatures reflect the basic sockets APIs,
//            implement the public transmission primitives listed above
//
//  -.   one protected method for inspecting socket attributes:
//       -.   get_sock_option( )  - a generalized get attribute method
//
//  -.   two protected methods for altering socket attributes:
//       -.   setSockOpt( ) - a generalized set attribute method
//       -.   fioIoctlSocket( ) - a generalized mode setting method
//
//  -.   the comparison operators != and ==
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

// =*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
//  the socket class proper
// =*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*

// ----------------------------------------------------------------------
//  type defined below used for parameter passing instead of "void *"
//  for socket functions that need to be polymorphic and variadic:
//      read_event(), write_event(), and exception_event()
// ---------------------------------------------------------------------

typedef map< string, void* > argument_block_type;

class socket_class
{
public:
    //  =====================================================
    //  static data
    //  =====================================================
    //
    //  strings below define standard keys for argument blocks.
    //  these keys are used to retrieve parameters from argument blocks
    //       by send/sendto/recv/recvfrom routines.

    static const string P_BUFFER_ADDRESS;
    static const string P_BUFFER_SIZE;
    static const string P_RECEIVE_COUNT;
    static const string P_SERVER_ACCESS_POINT;

    //  =====================================================
    //  assignment operator
    //  =====================================================
    //
    // IMPORTANT: assignment operator does NOT duplicate socket.
    // operator merely makes second copy of socket_ variable.

    socket_class& operator=(const socket_class&);

    // ===========================================================
    //  inspectors
    // ===========================================================

    //  *** methods for determining message attributes ***

    //   get number of bytes available to be recv()'d on this socket
    //   parameters:  number of bytes available for read (on return)
    //
    unsigned long bytes_pending(void) const;

    //  *** methods for inspecting socket attributes ***

    //   get service access point associated with this socket
    //
    socket_service_access_point_class service_access_point(void) const;

    //   get service access point associated with peer socket.
    //   [virtual because unconnected sockets don't have peers]
    //
    virtual socket_service_access_point_class peer_service_access_point(void) const;

    // ========================================================
    //   manipulators
    // ========================================================

    //  **** methods for altering socket attributes ****
    //
    void set_blocking(void);
    void set_nonblocking(void);

    //  **** methods for managing socket communications ****
    //
    // send/recv primitives
    //     - exchange data with implicit host, using data in arg block
    // sendto/recvfrom primitives
    //     - exchange data with explicit host, using data in arg block

    virtual void send(argument_block_type&);
    virtual void recv(argument_block_type&);
    virtual void sendto(argument_block_type&);
    virtual void recvfrom(argument_block_type&);

    //  **** close primitive ****
    //
    void closesocket(void);

    // ========================================================
    //   other operators
    // ========================================================

    // ---- logical op facilitators ----
    //
    // socket object equality test-- based on value of socket_

    int operator== (const socket_class&) const;
    int operator!= (const socket_class&) const;

    // ========================================================
    //   facilitators
    // ========================================================
    //
    // insert_socket_class_data - dump socket characteristics to specified ostream

    void insert_socket_class_data(ostream&);

    // ==================
    //  destructor
    // ==================
    //
    virtual ~socket_class(void);

protected:
    // =====================================================
    //  constructor
    // =====================================================

    // constructors protected to ensure class is abstract.
    //
    // first version -  specifies version of sockets dll in use
    // second version - copy constructor
    //
    // IMPORTANT: copy constructor does NOT duplicate socket.
    // Copy constructor merely makes second copy of socket_ variable.

    socket_class(const unsigned16 = SOCKETS_DLL_VERSION);
    socket_class(const socket_class&);

    //  =====================================================
    //  inspectors
    //  =====================================================
    //
    //  == get_sock_option -  wrapper for Socket API's getsockopt(),
    //                   which returns requested socket characteristic
    //
    void get_sock_option(const int level, const int option_name, char* const option_val, int* const option_len) const;

    //  =====================================================
    //  manipulators
    //  =====================================================

    //  === setSockOpt -  wrapper for Socket API's setsockopt(),
    //                   which sets specified socket characteristic
    //
    void setSockOpt(const int level, const int option_name, const char* const option_val, const int option_len);

    //  =====================================================
    //      member data
    //  =====================================================

    //  ==== the socket proper ====
    //
    SOCKET socket_;

private:
    //  =====================================================
    //      member functions
    //  =====================================================

    //  === basic methods for managing socket communications

    // send/recv primitives - exchange data with implicit host
    // sendto/recvfrom primitives - exchange data with explicit host
    //
    //   send()     returns on success, throws exception on failure
    //              parameters:  message to send; length of message
    //
    //   recv()     returns on success, throws exceptions on failure
    //              parameters:  buffer to capture message;
    //                           size of buffer;
    //                           number of bytes returned
    //   sendto()   returns on success, throws exception on failure
    //              parameters:  message to send;
    //                           length of message;
    //                           service access point of receiver;
    //
    //   recvfrom() returns on success, throws exception on failure
    //              parameters:  buffer to capture message;
    //                           size of buffer;
    //                           service access point of sender;
    //                           number of bytes returned

    void send(const char* const, const unsigned);
    void recv(char* const, const unsigned, unsigned&);

    void sendto(const char* const, const unsigned, const socket_service_access_point_class&);
    void recvfrom(char* const, const unsigned, socket_service_access_point_class&, unsigned&);

    // ========================================================
    //   arg block access methods
    // ========================================================

    //  -.  recover pointers to buffer, buffer size arguments
    //  -.  recover pointer to receive count argument
    //  -.  recover pointer to service access point argument
    //
    //  first two routines throw exceptions on failure.
    //  final argument to first two routines is string that names caller--
    //      for diagnostic messages

    void get_buffer_args(argument_block_type&, char*&, unsigned*&, const string&);
    void get_access_point_args(argument_block_type&, socket_service_access_point_class*&, const string&);
    void get_recv_count_args(argument_block_type&, unsigned*&);

    // ========================================================
    // member data
    // ========================================================
    //
    //  for ensuring that supporting socket DLL is loaded and unloaded
    //    default (void) constructor =>  load default version of DLL.
    //  make this static since DLL only needs to be loaded once.

    sockets_dll_init_class dll_init_object_;
};

// =*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
//  auxiliary operators
// =*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*

// --------------------------------------------------------------------
// dump data regarding specified socket to specified ostream
// --------------------------------------------------------------------

ostream& operator<<(ostream&, socket_class&);

// **************************************************************************
// exception classes for socket routines
// **************************************************************************
//
// create related names for exceptions in this module using
// -.  a base class to serve as a base class for all socket exceptions, plus
// -.  a set of derived classes that identify specific conditions

#ifndef EXCEPTION_H
#include "exception.h"
#endif

// =========================================================================
//  zero-order exception:  socket_exception
// =========================================================================

class socket_exception : public basic_exception
{
public:
    socket_exception(
        const string & = "", const string & = "", const string & = "",
        const basic_exception & = basic_exception());
    socket_exception(const socket_exception&);

    virtual ~socket_exception(void);
};

// ============================================================================
//  first-order exceptions
// ============================================================================
//
//   socket_service_access_point_exception           - couldn't get service access point of socket
//   socket_send_exception          - couldn't send to server
//   socket_recv_exception          - couldn't receive from server
//   socket_option_exception        - couldn't do socket options operation
//   socket_bytes_pending_exception  - couldn't check # bytes available for recv
//   socket_close_exception         - couldn't close the socket
//   socketMissingBufferException - couldn't get buffer parameters

class socket_service_access_point_exception : public socket_exception
{
public:
    socket_service_access_point_exception(
        const string & = "", const string & = "", const string & = "",
        const basic_exception & = basic_exception());
    socket_service_access_point_exception(const socket_service_access_point_exception& source);
    virtual ~socket_service_access_point_exception(void);
};

class socket_send_exception : public socket_exception
{
public:
    socket_send_exception(
        const string & = "", const string & = "", const string & = "",
        const basic_exception & = basic_exception());
    socket_send_exception(const socket_send_exception& source);
    virtual ~socket_send_exception(void);
};

class socket_recv_exception : public socket_exception
{
public:
    socket_recv_exception(
        const string & = "", const string & = "", const string & = "",
        const basic_exception & = basic_exception());
    socket_recv_exception(const socket_recv_exception& source);
    virtual ~socket_recv_exception(void);
};

class socket_option_exception : public socket_exception
{
public:
    socket_option_exception(
        const string & = "", const string & = "", const string & = "",
        const basic_exception & = basic_exception());
    socket_option_exception(const socket_option_exception& source);
    virtual ~socket_option_exception(void);
};

class socket_bytes_pending_exception : public socket_exception
{
public:
    socket_bytes_pending_exception(
        const string & = "", const string & = "", const string & = "",
        const basic_exception & = basic_exception());
    socket_bytes_pending_exception(const socket_bytes_pending_exception& source);
    virtual ~socket_bytes_pending_exception(void);
};

class socket_close_exception : public socket_exception
{
public:
    socket_close_exception(
        const string & = "", const string & = "", const string & = "",
        const basic_exception & = basic_exception());
    socket_close_exception(const socket_close_exception& source);
    virtual ~socket_close_exception(void);
};

class socket_missing_argument_exception : public socket_exception
{
public:
    socket_missing_argument_exception(
        const string & = "", const string & = "", const string & = "",
        const basic_exception & = basic_exception());
    socket_missing_argument_exception(const socket_missing_argument_exception&);
    virtual ~socket_missing_argument_exception(void);
};

// ============================================================================
//  second-order exceptions
// ============================================================================
//
//   socket_empty_recv_exception      - recv'd empty packet (probably dropped connection)
//   socket_deficient_recv_exception  - recv'd insufficient data

class socket_empty_recv_exception : public socket_recv_exception
{
public:
    socket_empty_recv_exception(
        const string & = "", const string & = "", const string & = "",
        const basic_exception & = basic_exception());
    socket_empty_recv_exception(const socket_empty_recv_exception& source);
    virtual ~socket_empty_recv_exception(void);
};

class socket_deficient_recv_exception : public socket_recv_exception
{
public:
    socket_deficient_recv_exception(
        const string & = "", const string & = "", const string & = "",
        const basic_exception & = basic_exception());
    socket_deficient_recv_exception(const socket_deficient_recv_exception& source);
    virtual ~socket_deficient_recv_exception(void);
};

// *===*====*===*====*===*====*===*====*===*====*===*====*===*====*===*====
// auxiliary operators
// *===*====*===*====*===*====*===*====*===*====*===*====*===*====*===*====

ostream& operator<<(ostream&, const socket_exception&);
ostream& operator<<(ostream&, const socket_missing_argument_exception&);
ostream& operator<<(ostream&, const socket_service_access_point_exception&);
ostream& operator<<(ostream&, const socket_send_exception&);
ostream& operator<<(ostream&, const socket_recv_exception&);
ostream& operator<<(ostream&, const socket_option_exception&);
ostream& operator<<(ostream&, const socket_bytes_pending_exception&);
ostream& operator<<(ostream&, const socket_close_exception&);
ostream& operator<<(ostream&, const socket_empty_recv_exception&);
ostream& operator<<(ostream&, const socket_deficient_recv_exception&);

#endif
