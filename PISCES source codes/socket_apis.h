//--------------socket_apis.h------------------------------------------------
//
//   Author:   Phil Pfeiffer
//   Date:     September 1999
//   Last modified:  April 2020
//
//   specify location of C++ socket libraries packages, and define
//   BSD-implementation-specific constants in terms of Winsock constants.
//
//   Design Notes:
//   -.  it imports socket library header files, according to
//       the specified target platform and sockets implementation
//       (from "platform.h")
//
//  -.  it defines functions, types, macros, that mask differences
//      between WinSock and BSD approaches to
//      -.  converting IP addresses to vectors of characters
//      -.  defining the type of a socket
//      -.  defining error conditions
//
//---------------------------------------------------------------------------

#ifndef SOCKET_APIS_H
#define SOCKET_APIS_H

#ifndef PLATFORM_H
#include "platform.h"
#endif

#include <string>
#include <iostream>            //included to isolate the io_state_type

#ifndef STD_H
#include "std.h"
#endif

#ifndef TYPES_H
#include "types.h"
#endif

// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// @@@                                                             @@@
// @@@   PROTOTYPES FOR CROSS-PLATFORM ISOLATION LAYER FUNCTIONS   @@@
// @@@                                                             @@@
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

//=============================================================================
//  functions for loading, unloading DLLs:
//
//     installSocketsDLL -  load specified version of sockets DLL,
//                          returning 0 iff load succeeded
//     removeSocketsDLL -   remove currently installed DLL
//
//  functions not meaningful in BSD, which doesn't support socket DLL
//=============================================================================
//
int installSocketsDLL(const unsigned16&);
void removeSocketsDLL(void);

//=============================================================================
//  define function for converting IP addresses to byte strings--
//  this conversion is slightly different in WinSock and BSD
//=============================================================================
//
string in_addr_to_string(char** pIpAddr);

//==============================================================================
//   BSD, WinSock return different error codes when a process attempts
//   to reclose a socket that's already been closed.  define function
//   that masks this difference.
//==============================================================================
//
long  socket_already_closed(void);

//==============================================================================
//   in BSD and WinSock, logic for accessing code for last error differs
//==============================================================================
//
long  get_last_socket_error(void);

//=================================================================================
//  get_socket_error_message() -
//     translate name of socket API routine, error code into diagnostic message
//=================================================================================

string get_socket_error_message(const long error_code, const string& caller);

// ##########################################################
// End cross-platform Definitions
// ##########################################################

// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// @@@                                        @@@
// @@@   DEFINTIONS FOR SPECIFIC PLATFORMS    @@@
// @@@                                        @@@
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// Support for WinSock Platforms
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

#ifdef WINSOCK

//    Key concerns addressed below:
//    -.   #include's for header files for supporting API header files
//    -.   masking differences between WinSock, BSD approaches for
//         converting IP addresses to null-terminated char vectors
//    -.   masking differences between WinSock, BSD error management

//=========================================================================
// import the appropriate version of WinSock,
// and define compile-time variable that identifies version of choice
//========================================================================

#ifdef WINSOCK_2_2
#define SOCKETS_DLL_VERSION (0x0202)
#include <winsock2.h>
#endif  // WINSOCK_2

#ifdef WINSOCK_1_1
#define SOCKETS_DLL_VERSION (0x0101)
#include <winsock.h>
#endif  // WINSOCK_1_1

//=========================================================================
// typedefs for ioctlsocket():
//     type of 3rd (pointer-to-result) argument for calls of form
//          ioctlsocket( SOCKET, FIOxxxx, ___ )
//========================================================================

typedef unsigned long fio_ioctl_arg_type;
typedef fio_ioctl_arg_type* p_fio_ioctl_arg_type;

//=========================================================================
// typedef for getsockname(), getpeername(), recvfrom()
//      type of 3rd (length of sockaddr buffer) argument for calls to
//           getsockname( SOCKET, struct sockaddr *, _____ )
//           getpeername( SOCKET, struct sockaddr *, _____ )
//      type of 6th (length of sockaddr buffer) argument for calls to
//           recvfrom( SOCKET, char *, int, int, struct sockaddr *, _____ )
//========================================================================

typedef int sockAddrBufferLenType;

//=========================================================================
// typedef for getsockopt()
//      type of 5th (length of option type buffer) argument for calls to
//           getsockopt( SOCKET, int, int, char *,  _____ )
//========================================================================

typedef int sockOptionResultLenType;

typedef long int io_state_type;
typedef long int format_flags;
//linux: typedef std::_Ios_Fmtflags format_flags;

#endif  // WINSOCK

// ##########################################################
// End WinSock-specific Definitions
// ##########################################################

// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// support for BSD Platforms
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//
//    Key concerns addressed below:
//    -.   #include's for header files for supporting API header files
//    -.   masking differences between WinSock, BSD approaches
//           to converting an IP addr object to a null-terminated char*
//    -.   masking differences between WinSock, BSD error management

#ifdef BSD_SOCKETS

//==============================================================================
//   -- #includes for supporting API header files --
//==============================================================================
//
//
#include <sys/types.h>    // system-wide data type definitions
#include <sys/socket.h>   // definition of the socket address structure
#include <sys/ioctl.h>    // definition of i/o control primitives
#include <netinet/in.h>   // definitions for Internet-family network addresses
#include <arpa/inet.h>    // IP addr struct <-> char* conversion functions
#include <netdb.h>        // structures for supporting DNS data
#include <unistd.h>       // BSD close() primitive
#include <errno.h>        // socket error codes stored in global var errno...
#include <string.h>       // ...and converted to strings using strerror()
#include <sys/time.h>       // struct timeval defined here

#ifdef NETBSD_GNU_CPP
//
//  For some reason <sys/param.h> is also included by the above include files.
//  <sys/param.h> defines an is_set() macro that conflicts with
//  selectable_socket_class::is_set().
//
//  AFAIK, is_set() is a kernel function, and I don't know why it is included.
//  Simply undefine it.    (Andy Finnell)
//
#undef is_set
#endif

//==============================================================================
//  no DLL version need be defined--sockets are statically linked in Unix
//==============================================================================
//
#define SOCKETS_DLL_VERSION (0x0101)

//==============================================================================
//  allow for WinSock's treatment of a socket as a handle
//==============================================================================
//
//    in BSD Sockets, sockets are file descriptors--
//         i.e., small integers--and not a special data type
//    in BSD Sockets, ioctlsocket() and closesocket() doesn't exist;
//         socket close operations are handled by ioctl() and close()
//
typedef int SOCKET;
int ioctlsocket(SOCKET, long, char*);
int closesocket(SOCKET);

//=========================================================================
// typedefs for result argument, pointer to result argument for
// ioctlsocket( SOCKET, FIOxxxx, result * ):
//========================================================================

typedef int fio_ioctl_arg_type;
typedef char* p_fio_ioctl_arg_type;

//==============================================================================
//  error management
//==============================================================================
//
//   In BSD Sockets, unlike WinSock,
//      status values that signal errors have no special names
//      -.  BSD uses "-1" to signal what WinSock calls SOCKET_ERROR
//          (cf. gethostname(), getpeername(), listen(), and shutdown())
//      -.  BSD uses "-1" to signal what WinSock calls INVALID_SOCKET
//
#define SOCKET_ERROR   (-1)
#define INVALID_SOCKET (-1)

//   WinSock.h defines all BSD error codes,
//      but BSD header files don't define WinSock error codes.
//   Most WinSock codes, howeveer, correspond to the equivalent BSD codes--
//      codes obtained by removing the prepended "WSA".
//
//   Accordingly, we use WinSock error codes throughout,
//      and redefine these codes in terms of the BSD codes.
//
//   Caution: not all codes correspond across systems
//
#define WSAENOTSOCK          (ENOTSOCK)
#define WSAEINTR             (EINTR)
#define WSAEACCES            (EACCES)
#define WSAEFAULT            (EFAULT)
#define WSAEINVAL            (EINVAL)
#define WSAEWOULDBLOCK       (EWOULDBLOCK)
#define WSAEINPROGRESS       (EINPROGRESS)
#define WSAEALREADY          (EALREADY)
#define WSAENOTSOCK          (ENOTSOCK)
#define WSAEDESTADDRREQ      (EDESTADDREQ)
#define WSAEMSGSIZE          (EMSGSIZE)
#define WSAEPROTOTYPE        (EPROTOTYPE)
#define WSAENOPROTOOPT       (ENOPROTOOPT)
#define WSAEPROTONOSUPPORT   (EPROTONOSUPPORT)
#define WSAESOCKTNOSUPPORT   (ESOCKTNOSUPPORT)
#define WSAEOPNOTSUPP        (EOPNOTSUPP)
#define WSAEAFNOSUPPORT      (EAFNOSUPPORT)
#define WSAEADDRINUSE        (EADDRINUSE)
#define WSAEADDRNOTAVAIL     (EADDRNOTAVAIL)
#define WSAENETDOWN          (ENETDOWN)
#define WSAENETUNREACH       (ENETUNREACH)
#define WSAENETRESET         (ENETRESET)
#define WSAECONNABORTED      (ECONNABORTED)
#define WSAECONNRESET        (ECONNRESET)
#define WSAENOBUFS           (ENOBUFS)
#define WSAEISCONN           (EISCONN)
#define WSAENOTCONN          (ENOTCONN)
#define WSAESHUTDOWN         (ESHUTDOWN)
#define WSAETIMEDOUT         (ETIMEDOUT)
#define WSAECONNREFUSED      (ECONNREFUSED)
#define WSAEMFILE            (EMFILE)
#define WSAEBADF             (EBADF)
#define WSAEHOSTDOWN         (EHOSTDOWN)
#define WSAEHOSTUNREACH      (EHOSTUNREACH)
#define WSAENOTEMPTY         (ENOTEMPTY)
#define WSAEPROCLIM          (EPROCLIM)
#define WSAESTALE            (ESTALE)
#define WSAEPFNOSUPPORT      (EPFNOSUPPORT)
#define WSAENAMETOOLONG      (ENAMETOOLONG)
#define WSAETOOMANYREFS      (ETOOMANYREFS)
#define WSAEUSERS            (EUSERS)
#define WSAEDQUOT            (EDQUOT)
#define WSAELOOP             (ELOOP)
#define WSAEREMOTE           (EREMOTE)

// @@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-
//  IRIX-specific BSD Sockets definitions
// @@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-

#ifdef IRIX_GNU_CPP

//=========================================================================
// typedef for getsockname(), getpeername(), recvfrom()
//      type of 3rd (length of sockaddr buffer) argument for calls to
//           getsockname( SOCKET, struct sockaddr *, _____ )
//           getpeername( SOCKET, struct sockaddr *, _____ )
//      type of 6th (length of sockaddr buffer) argument for calls to
//           recvfrom( SOCKET, char *, int, int, struct sockaddr *, _____ )
//========================================================================

typedef socklen_t sockAddrBufferLenType;

//========================================================================
// typedef for getsockopt()
//      type of 5th (length of option type buffer) argument for calls to
//           getsockopt( SOCKET, int, int, char *,  _____ )
//========================================================================

typedef socklen_t sockOptionResultLenType;

typedef std::_Ios_Iostate io_state_type;
typedef std::_Ios_Fmtflags format_flags;

//========================================================================
//  hack for using BSD inet_ntoa() on IRIX
//========================================================================

// *** subsituting custom-crafted inet_ntoa for standard is a hack ***
//
// ... but I can't get IRIX's inet_ntoa() to work  (Pfeiffer, Sept. 1998)

char* my_inet_ntoa(const struct in_addr& adr);
#define inet_ntoa( arg ) ( my_inet_ntoa( arg ) )

#endif  // IRIX_GNU_CPP

// ##-##-##-##-##-##-##-##-##-##-##-##-##-##-##-##-##-##-##-##-##-##-##-##-##-
//  End IRIX-specific definitions
// ##-##-##-##-##-##-##-##-##-##-##-##-##-##-##-##-##-##-##-##-##-##-##-##-##-

// @@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-
//  NETBSD-specific BSD Sockets definitions
// @@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-

#ifdef NETBSD_GNU_CPP

//=========================================================================
// typedef for getsockname(), getpeername(), accept(), recvfrom()
//      type of 3rd (length of sockaddr buffer) argument for calls to
//           getsockname( SOCKET, struct sockaddr *, _____ )
//           getpeername( SOCKET, struct sockaddr *, _____ )
//           accept( SOCKET, struct sockaddr *, _____ )
//      type of 6th (length of sockaddr buffer) argument for calls to
//           recvfrom( SOCKET, char *, int, int, struct sockaddr *, _____ )
//========================================================================

typedef socklen_t sockAddrBufferLenType;

//========================================================================
// typedef for getsockopt()
//      type of 5th (length of option type buffer) argument for calls to
//           getsockopt( SOCKET, int, int, char *,  _____ )
//========================================================================

typedef socklen_t sockOptionResultLenType;

typedef std::_Ios_Iostate io_state_type;
typedef std::_Ios_Fmtflags format_flags;

#endif  // NETBSD_GNU_CPP

// ##-##-##-##-##-##-##-##-##-##-##-##-##-##-##-##-##-##-##-##-##-##-##-##-##-
//  End NETBSD-specific definitions
// ##-##-##-##-##-##-##-##-##-##-##-##-##-##-##-##-##-##-##-##-##-##-##-##-##-

// @@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-
//  LINUX-specific BSD Sockets definitions
// @@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-@@-

#ifdef LINUX_GNU_CPP

//=========================================================================
// typedef for getsockname(), getpeername(), recvfrom()
//      type of 3rd (length of sockaddr buffer) argument for calls to
//           getsockname( SOCKET, struct sockaddr *, _____ )
//           getpeername( SOCKET, struct sockaddr *, _____ )
//      type of 6th (length of sockaddr buffer) argument for calls to
//           recvfrom( SOCKET, char *, int, int, struct sockaddr *, _____ )
//========================================================================

typedef socklen_t sockAddrBufferLenType;

typedef int SOCKET;
#define SOCKETS_DLL_VERSION (0x0101)

//========================================================================
// typedef for getsockopt()
//      type of 5th (length of option type buffer) argument for calls to
//           getsockopt( SOCKET, int, int, char *,  _____ )
//========================================================================

typedef socklen_t sockOptionResultLenType;

typedef std::_Ios_Iostate io_state_type;
typedef std::_Ios_Fmtflags format_flags;

#endif  // LINUX_GNU_CPP

// ##-##-##-##-##-##-##-##-##-##-##-##-##-##-##-##-##-##-##-##-##-##-##-##-##-
//  End LINUX-specific definitions
// ##-##-##-##-##-##-##-##-##-##-##-##-##-##-##-##-##-##-##-##-##-##-##-##-##-

// ##########################################################
// End BSD Sockets-specific Definitions
// ##########################################################

#endif  // BSD_SOCKETS

// ##########################################################
// Declare IPv4 and IPv6 functionality
// ##########################################################

//#######QUESTION: there's a lot of replication in the declaration, could we
//#######do this part once and then separate the code for the definition??

//#####need to enforce use of 4 or 16 bytes? already done semanticly?

//#####GLOBAL: look for FIXME and CHANGED and QUESTION comments

//#####FIXME: clean up here when fully tested/working

#include <vector>
#include <sstream>

#ifdef IP_V4

#ifndef DNS_EXCEPTION_H
#include "dns_exception.h"  //DNS exceptions (thrown by class methods on error)
#endif

typedef vector< unsigned8 > ip_address_vector_type;
typedef sockaddr_in socket_address;
typedef in_addr ip_version_address;
#ifdef WIN_VISUAL_6_0
typedef int in_addr_t;
#endif
typedef in_addr_t ip_version_address_type;
typedef u_long ip_address_struct;

//defines
#define ip_address_length 4        //4 bytes
#define ip_address_family AF_INET
#ifndef INET_ADDRSTRLEN
#define INET_ADDRSTRLEN 16
#endif
#define ip_address_strlen INET_ADDRSTRLEN
#define max_field_value 255
#define addr_any INADDR_ANY

// return the host using the v4 system call
//
inline hostent* get_host_by_name(const char* dns_name) { return gethostbyname(dns_name); }

// Return the port number/ip address for a sockaddr_in struct
// Reason: the internal naming is different for v4 than v6
//
inline unsigned short get_port_number(const socket_address& input_socket_address) { return input_socket_address.sin_port; }    //cast??

// Call the socket level fucntion to convert from sockaddr_in.in_addr to string
char* socket_address_numerical_to_string(const socket_address&, char*);

#endif // IP_V4

/*QUESTION
// Fill in version specific address information into sockaddr_in6
// for BSD 4.4, also need to fill in the length (?, new division?)
void populate_socket_address_struct(socket_address&, unsigned short,
    ip_version_address);
*/

#ifdef IP_V6

#ifndef DNS_EXCEPTION_H
#include "dns_exception.h"  //DNS exceptions
#endif

#ifndef EXCEPTION_H
#include "exception.h"  //DNS exceptions
#endif

typedef vector< unsigned16 > ip_address_vector_type;
typedef sockaddr_in6 socket_address;
typedef in6_addr ip_version_address;
typedef in6_addr ip_version_address_type;    //huh?
typedef u_long ip_address_struct;    //u_long for above? what goes here?

#define ip_address_length 8        //eight words
#define ip_address_family AF_INET6
#define ip_address_strlen INET6_ADDRSTRLEN
#define max_field_value 65535
#define addr_any in6addr_any

//return the host using the v6 system call
//
inline hostent* get_host_by_name(const char* dns_name) { return gethostbyname2(dns_name, AF_INET6); }

// Return the port number/ip address for a sockaddr_in6 struct
// Reason: the internal naming is different for v4 than v6
//
inline unsigned short get_port_number(const socket_address& input_socket_address) { return input_socket_address.sin6_port; }//cast??

// Call the socket level fucntion to convert from sockaddr_in.in_addr to string
const char* socket_address_numerical_to_string(const socket_address&, char*);

// Conversion specific items
//
struct ip_field
{
    unsigned short value;
    int status;     //-1 = invalid; 0 = empty; 1 = valid
    unsigned begin_stream_position, end_stream_position;
};

typedef vector< ip_field > ip_vector;

// Conversion #defines
#define max_fields 8
#define max_empty_fields 1
#define max_char_to_read 10

// Build and throw appropriate exception for malformed IPv6 address
void throw_error(const ip_vector&, const vector<int>&, string);

// Convert an IPv6 string in input stream into internal vector representation
ip_address_vector_type convert_ip_address_string(istream&);

// Helper function for IPv6 conversion
ip_vector build_ip_vector(istream&);

// Helper function for IPv6 conversion
ip_field get_field(istream&, bool&);

// Internally used exception class for conversion
//
class empty_stream_exception : public basic_exception
{
public:
    empty_stream_exception(
        const string & = "", const string & = "", const string & = "",
        const basic_exception & = basic_exception());
    empty_stream_exception(const empty_stream_exception&);

    virtual ~empty_stream_exception(void);
};

// *===*====*===*====*===*====*===*====*===*====*===*====*===*====*===*====
// auxiliary operators
// *===*====*===*====*===*====*===*====*===*====*===*====*===*====*===*====

ostream& operator<<(ostream&, const empty_stream_exception&);

// end empty_stream_exception declaration

#endif // IP_V6

// COMMON DEFINITIONS
//

//function prototype for returning the size
inline unsigned get_ip_address_length() { return ip_address_length; };

//function prototype for returning the IP family
inline int get_ip_family() { return ip_address_family; };

//get the length that a buffer must be equal to, to hold a IPv6 address string
inline unsigned get_address_string_length() { return ip_address_strlen; }

//get the largest value that can be stored in an IP address field (1 or 2 byte value)
inline unsigned get_max_field_value() { return max_field_value; }

// COMMON DECLARATIONS
//

// Convert the ip address to a string
string ip_address_as_string(ip_address_vector_type);

// Convert the ip address from string to vector of bytes
ip_address_vector_type ip_address_string_to_ip_address(string);

//Insert the ip address into the stream
bool ip_address_insert(ostream&, const ip_address_vector_type&);

// Convert from dotted string to internal byte vector
ip_address_vector_type convert_ip_address_string(istream&);

// Fill in version specific address information into sockaddr_in
void populate_socket_address_struct(socket_address&, unsigned short, ip_version_address_type);

// Overloaded version of above to handle INADDR_ANY
void populate_socket_address_struct(socket_address&, unsigned short, ip_address_struct);

// Call the socket level fucntion to convert from string to sockaddr_in.in_addr
ip_version_address_type socket_address_string_to_numerical(const char*);

#endif
