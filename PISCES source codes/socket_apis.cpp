//--------------socket_apis.cpp---------------------------------------------
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
//---------------------------------------------------------------------------

#include "socket_apis.h"

#ifdef WINSOCK

// **********************************************************
// Support for WinSock Platforms
// **********************************************************

//=============================================================================
//  functions for loading, unloading DLLs:
//
//     installSocketsDLL -  load specified version of sockets DLL,
//                          returning 0 iff load succeeded
//     removeSocketsDLL -   remove currently installed DLL
//=============================================================================
//
int installSocketsDLL(const unsigned16& version)
{
    WSADATA WsaData_;                      // scratch area needed by WSAStartup
    return WSAStartup(version, &WsaData_);
}

void removeSocketsDLL(void)
{
    WSACleanup();
}

//=============================================================================
//  in_addr_to_string - function for converting IP addresses to byte strings
//=============================================================================
//
//  this conversion is slightly different in WinSock and BSD
//
string in_addr_to_string(char** pIpAddr)
{
    return(string(inet_ntoa(*reinterpret_cast<LPIN_ADDR>(*pIpAddr))));
}

//=============================================================================
//   SocketAlreadyClosed() - determine if socket is being closed a second time
//=============================================================================
//
long socket_already_closed(void)
{
    return WSAENOTSOCK;   // closing socket, in WinSock, makes it non-socket
}

//==============================================================================
//  get_last_socket_error() - retrieve code number for last socket error
//==============================================================================
//
long get_last_socket_error(void) { return WSAGetLastError(); }

//=================================================================================
//  get_socket_error_message() -
//     translate name of socket API routine, error code into diagnostic message
//
//     The error messages for the WinSock interface were developed from
//     the Winsock 1.1 standard,
//         "Windows Sockets 1.1 Specification - Socket Library Reference"
//     (URL:  http://www.winsock.com/wsresource/winsock1/ws_c.html).
//
//  WinSock users can reference the original Winsock specification
//  for more documentation on the causes of the errors in question.
//
//=================================================================================
//
string get_socket_error_message(const long error_code, const string& caller)
{
    // **** **** init static data-- messages for returning to caller *** ***

    static string WSAEINTR_default_ =
        "WSAEINTR: The (blocking) call was canceled via WSACancelBlockingCall()";

    static string WSAEACCES_default_ =
        "WSAEACCES: Broadcast address requested, but appropriate flag not set";

    static string WSAEFAULT_accept_ =
        "WSAEFAULT: Address length smaller than a struct sockaddr";

    static string WSAEFAULT_bind_ =
        "WSAEFAULT: Name length smaller than a struct sockaddr";

    static string WSAEFAULT_connect_ =
        "WSAEFAULT: Name length argument is incorrect";

    static string WSAEFAULT_gethostname_ =
        "WSAEFAULT: Name length argument is too small";

    static string WSAEFAULT_getpeername_ =
        "WSAEFAULT: Name length argument is too small";

    static string WSAEFAULT_getsockname_ =
        "WSAEFAULT: Name length argument is too small";

    static string WSAEFAULT_getsockopt_ =
        "WSAEFAULT: Invalid option list length";

    static string WSAEFAULT_listen_ =
        "WSAEFAULT: An invalid argument was given";

    static string WSAEFAULT_recvfrom_ =
        "WSAEFAULT: Receive buffer too small to hold peer address";

    static string WSAEFAULT_send_ =
        "WSAEFAULT: Invalid buffer address";

    static string WSAEFAULT_sendto_ =
        "WSAEFAULT: Invalid buf. or dest. adr., or dest. smaller than a struct sockaddr";

    static string WSAEFAULT_setsockopt_ =
        "WSAEFAULT: Invalid address for option argument";

    static string WSAEFAULT_WSAAsyncSelect_ =
        "WSAEFAULT: The namelen argument is incorrect";

    static string WSAEFAULT_default_ =
        "WSAEFAULT: Incorrect parameter";

    static string WSAEINVAL_accept_ =
        "WSAEINVAL: listen() was not invoked prior to accept()";

    static string WSAEINVAL_bind_ =
        "WSAEINVAL: The socket is already bound to an address";

    static string WSAEINVAL_connect_ =
        "WSAEINVAL: The socket is not already bound to an address";

    static string WSAEINVAL_getsockname_ =
        "WSAEINVAL: The socket has not been bound with bind";

    static string WSAEINVAL_ioctlsocket_ =
        "WSAEINVAL: Invalid command, command/socket type combination, or parameter";

    static string WSAEINVAL_listen_ =
        "WSAEINVAL: The socket has not been bound with bind or is already connected";

    static string WSAEINVAL_recv_ =
        "WSAEINVAL: The socket has not been bound with bind";

    static string WSAEINVAL_recvfrom_ =
        "WSAEINVAL: The socket has not been bound with bind";

    static string WSAEINVAL_select_ =
        "WSAEINVAL: The timeout value is invalid";

    static string WSAEINVAL_send_ =
        "WSAEINVAL: The socket has not been bound with bind";

    static string WSAEINVAL_setsockopt_ =
        "WSAEINVAL: The level is invalid, or the options are invalid";

    static string WSAEINVAL_closesocket_ =
        "WSAEINVAL: The close mode is invalid";

    static string WSAEINVAL_WSAAsyncSelect_ =
        "WSAEINVAL: Invalid parameter, or socket is already bound";

    static string WSAEINVAL_WSACancelAsyncRequest_ =
        "WSAEINVAL: The specified asynchronous task handle was invalid";

    static string WSAEINVAL_WSACancelBlockingCall_ =
        "WSAEINVAL: There is no outstanding blocking call";

    static string WSAEINVAL_default_ =
        "WSAEINVAL: Invalid parameter";

    static string WSAEWOULDBLOCK_accept_ =
        "WSAEWOULDBLOCK: Socket nonblocking, and no connections are present";

    static string WSAEWOULDBLOCK_connect_ =
        "WSAEWOULDBLOCK: Socket non-blocking, and connection cannot be completed immediately";

    static string WSAEWOULDBLOCK_closesocket_ =
        "WSAEWOULDBLOCK: Socket nonblocking, and SO_LINGER is nonzero";

    static string WSAEWOULDBLOCK_WSAAsync_ =
        "WSAEWOULDBLOCK: Winsock constraints preclude scheduling of asychronous operation at this time";

    static string WSAEWOULDBLOCK_default_ =
        "WSAEWOULDBLOCK: Socket non-blocking, and operation would cause the socket to block";

    static string WSAEINPROGRESS_default_ =
        "WSAEINPROGRESS: Blocking Winsock call or operation in progress";

    static string WSAEALREADY_default_ =
        "WSAEALREADY: Routine being canceled has already completed";

    static string WSAENOTSOCK_select_ =
        "WSAENOTSOCK: Descriptor set contains an entry that is not a socket";

    static string WSAENOTSOCK_default_ =
        "WSAENOTSOCK: The descriptor is not a socket";

    static string WSAEDESTADDRREQ_default_ =
        "WSAEDESTADDRREQ: A destination address is required";

    static string WSAEMSGSIZE_send_ =
        "WSAEMSGSIZE: Datagram larger than maximum supported by Winsock implementation";

    static string WSAEMSGSIZE_sendto_ =
        "WSAEMSGSIZE: Datagram larger than maximum supported by Winsock implementation";

    static string WSAEMSGSIZE_default_ =
        "WSAEMSGSIZE: The datagram was too large to fit into the buffer";

    static string WSAEPROTOTYPE_default_ =
        "WSAEPROTOTYPE: Specified protocol is wrong type for socket";

    static string WSAENOPROTOOPT_default_ =
        "WSAENOPROTOOPT: Option is unknown or unsupported";

    static string WSAEPROTONOSUPPORT_default_ =
        "WSAEPROTONOSUPPORT: specified protocol is not supported";

    static string WSAESOCKTNOSUPPORT_default_ =
        "WSASOCKTNOSUPPORT: socket type not supported by address family";

    static string WSAEOPNOTSUPP_accept_ =
        "WSAEOPNOTSUPP: referenced socket does not support connection-oriented service";

    static string WSAEOPNOTSUPP_listen_ =
        "WSAEOPNOTSUPP: referenced socket does not support the listen operation";

    static string WSAEOPNOTSUPP_recv_ =
        "WSAEOPNOTSUPP: out-of-band data exchange only supported for type SOCK_STREAM";

    static string WSAEOPNOTSUPP_default_ =
        "WSAEOPNOTSUPP: option not supported for specified type of socket";

    static string WSAEAFNOSUPPORT_bind_ =
        "WSAEAFNOSUPPORT: The specified address family is not supported by this protocol";

    static string WSAEAFNOSUPPORT_socket_ =
        "WSAEAFNOSUPPORT: The specified address family is not supported";

    static string WSAEAFNOSUPPORT_default_ =
        "WSAEAFNOSUPPORT: Addresses in the specified family cannot be used with this socket";

    static string WSAEADDRINUSE_default_ =
        "WSAEADDRINUSE: specified address is already in use";

    static string WSAEADDRNOTAVAIL_default_ =
        "WSAEADDRNOTAVAIL:  specified address not available from local machine";

    static string WSAENETDOWN_default_ =
        "WSAENETDOWN:  failure of network subsystem detected";

    static string WSAENETUNREACH_default_ =
        "WSAENETUNREACH:  network unreachable from host at this time";

    static string WSAENETRESET_default_ =
        "WSAENETRESET:  must reset connection--Winsock dropped it";

    static string WSAECONNABORTED_default_ =
        "WSAECONNABORTED:  connection aborted due to timeout or other failure";

    static string WSAECONNRESET_default_ =
        "WSAECONNRESET:  connection reset by the remote side";

    static string WSAENOBUFS_default_ =
        "WSAENOBUFS: Insufficient buffer space";

    static string WSAEISCONN_default_ =
        "WSAEISCONN: The socket is already connected";

    static string WSAENOTCONN_default_ =
        "WSAENOTCONN: The socket is not connected";

    static string WSAESHUTDOWN_recv_ =
        "WSAESHUTDOWN: Socket shut down, and cannot be used to receive data";

    static string WSAESHUTDOWN_recvfrom_ =
        "WSAESHUTDOWN: Socket shut down, and cannot be used to receive data";

    static string WSAESHUTDOWN_send_ =
        "WSAESHUTDOWN: Socket shut down, and cannot be used to send data";

    static string WSAESHUTDOWN_sendto_ =
        "WSAESHUTDOWN: Socket shut down, and cannot be used to send data";

    static string WSAESHUTDOWN_default_ =
        "WSAESHUTDOWN: Socket has been shut down";

    static string WSAETIMEDOUT_default_ =
        "WSAETIMEDOUT:  Attempt to connect timed out, connection not established";

    static string WSAECONNREFUSED_default_ =
        "WSAECONNREFUSED:  Attempt to connect was forcefully rejected";

    static string WSASYSNOTREADY_default_ =
        "WSASYSNOTREADY:  Underlying network subsystem not ready for network communication";

    static string WSAVERNOTSUPPORTED_default_ =
        "WSAVERNOTSUPPORTED:  API requested not provided by this Winsock DLL";

    static string WSANOTINITIALISED_default_ =
        "WSANOTINITIALISED:  API requires successful WSAStartup()";

    static string WSAHOST_NOT_FOUND_default_ =
        "WSAHOST_NOT_FOUND:  Authoritative Answer Host not found";

    static string WSATRY_AGAIN_default_ =
        "WSATRY_AGAIN:  Non-Authoritative Host not found, or SERVERFAIL";

    static string WSANO_RECOVERY_default_ =
        "WSANO_RECOVERY:  Non recoverable errors, FORMERR, REFUSED, NOTIMP";

    static string WSANO_DATA_default_ =
        "WSANO_DATA:  Valid name, no data record of requested type";

    static string WSAEMFILE_default_ =
        "WSAEMFILE:  No more file descriptors are available";

    static string WSAEBADF_default_ =
        "WSAEBADF: Bad file descriptor";

    static string WSAEHOSTDOWN_default_ =
        "WSAEHOSTDOWN: Host is down";

    static string WSAEHOSTUNREACH_default_ =
        "WSAEHOSTUNREACH: No route to host";

    static string WSAENOTEMPTY_default_ =
        "WSAENOTEMPTY: Directory not empty";

    static string WSAEPROCLIM_default_ =
        "WSAEPROCLIM: Too many processes";

    static string WSAESTALE_default_ =
        "WSAESTALE: No file system (i.e., stale file system handle)";

    static string WSAEPFNOSUPPORT_default_ =
        "WSAPFNOSUPPORT:  Protocol family not supported";

    static string WSAENAMETOOLONG_default_ =
        "WSAENAMETOOLONG: File name too long";

    static string WSANO_ADDRESS_default_ =
        "WSANO_ADDRESS: Probably (?) address not supplied";

    static string WSAETOOMANYREFS_default_ =
        "WSAETOOMANYREFS: Too many references: can't splice";

    static string WSAEUSERS_default_ =
        "WSAEUSERS: Too many users";

    static string WSAEDQUOT_default_ =
        "WSAEDQUOT: Disc quota exceeded";

    static string WSAELOOP_default_ =
        "WSAELOOP: Too many levels of symbolic links";

    static string WSAEREMOTE_default_ =
        "WSAEREMOTE: Item is not local to host";

    static string WSAunknown_default_ =
        "Undocumented error code (?)";

    // *** *** get specified message, and return it to user *** ***

    switch (error_code)
    {
    case WSAEINTR:    return WSAEINTR_default_;
    case WSAEACCES:   return WSAEACCES_default_;
    case WSAEFAULT:
        if (caller == "accept")
            return WSAEFAULT_accept_;
        else if (caller == "bind")
            return WSAEFAULT_bind_;
        else if (caller == "connect")
            return WSAEFAULT_connect_;
        else if (caller == "gethostname")
            return WSAEFAULT_gethostname_;
        else if (caller == "getpeername")
            return WSAEFAULT_getpeername_;
        else if (caller == "getsockname")
            return WSAEFAULT_getsockname_;
        else if (caller == "getsockopt")
            return WSAEFAULT_getsockopt_;
        else if (caller == "listen")
            return WSAEFAULT_listen_;
        else if (caller == "recvfrom")
            return WSAEFAULT_recvfrom_;
        else if (caller == "send")
            return WSAEFAULT_send_;
        else if (caller == "sendto")
            return WSAEFAULT_sendto_;
        else if (caller == "setsockopt")
            return WSAEFAULT_setsockopt_;
        else if (caller == "WSAAsyncSelect")
            return WSAEFAULT_WSAAsyncSelect_;
        else
            return WSAEFAULT_default_;

    case WSAEINVAL:
        if (caller == "accept")
            return WSAEINVAL_accept_;
        else if (caller == "bind")
            return WSAEINVAL_bind_;
        else if (caller == "connect")
            return WSAEINVAL_connect_;
        else if (caller == "getsockname")
            return WSAEINVAL_getsockname_;
        else if (caller == "ioctlsocket")
            return WSAEINVAL_ioctlsocket_;
        else if (caller == "listen")
            return WSAEINVAL_listen_;
        else if (caller == "recv")
            return WSAEINVAL_recv_;
        else if (caller == "recvfrom")
            return WSAEINVAL_recvfrom_;
        else if (caller == "select")
            return WSAEINVAL_select_;
        else if (caller == "send")
            return WSAEINVAL_send_;
        else if (caller == "setsockopt")
            return WSAEINVAL_setsockopt_;
        else if (caller == "closesocket")
            return WSAEINVAL_closesocket_;
        else if (caller == "WSAAsyncSelect")
            return WSAEINVAL_WSAAsyncSelect_;
        else if (caller == "WSACancelAsyncRequest")
            return WSAEINVAL_WSACancelAsyncRequest_;
        else if (caller == "WSACancelBlockingCall")
            return WSAEINVAL_WSACancelBlockingCall_;
        else
            return WSAEINVAL_default_;

    case WSAEWOULDBLOCK:
        if (caller == "accept")
            return WSAEWOULDBLOCK_accept_;
        else if (caller == "connect")
            return WSAEWOULDBLOCK_connect_;
        else if (caller == "closesocket")
            return WSAEWOULDBLOCK_closesocket_;
        else if (caller.find("WSAAsync") == 0)
            return WSAEWOULDBLOCK_WSAAsync_;
        else
            return WSAEWOULDBLOCK_default_;

    case WSAEINPROGRESS:
        return WSAEINPROGRESS_default_;

    case WSAEALREADY:
        return WSAEALREADY_default_;

    case WSAENOTSOCK:
        if (caller == "select")
            return WSAENOTSOCK_select_;
        else
            return WSAENOTSOCK_default_;

    case WSAEDESTADDRREQ:
        return WSAEDESTADDRREQ_default_;

    case WSAEMSGSIZE:
        if (caller == "send")
            return WSAEMSGSIZE_send_;
        else if (caller == "sendto")
            return WSAEMSGSIZE_sendto_;
        else
            return WSAEMSGSIZE_default_;

    case WSAEPROTOTYPE:
        return WSAEPROTOTYPE_default_;

    case WSAENOPROTOOPT:
        return WSAENOPROTOOPT_default_;

    case WSAEPROTONOSUPPORT:
        return WSAEPROTONOSUPPORT_default_;

    case WSAESOCKTNOSUPPORT:
        return WSAESOCKTNOSUPPORT_default_;

    case WSAEOPNOTSUPP:
        if (caller == "accept")
            return WSAEOPNOTSUPP_accept_;
        else if (caller == "listen")
            return WSAEOPNOTSUPP_listen_;
        else if (caller == "recv")
            return WSAEOPNOTSUPP_recv_;
        else
            return WSAEOPNOTSUPP_default_;

    case WSAEAFNOSUPPORT:
        if (caller == "bind")
            return WSAEAFNOSUPPORT_bind_;
        else if (caller == "socket")
            return WSAEAFNOSUPPORT_socket_;
        else
            return WSAEAFNOSUPPORT_default_;

    case WSAEADDRINUSE:
        return WSAEADDRINUSE_default_;

    case WSAEADDRNOTAVAIL:
        return WSAEADDRNOTAVAIL_default_;

    case WSAENETDOWN:
        return WSAENETDOWN_default_;

    case WSAENETUNREACH:
        return WSAENETUNREACH_default_;

    case WSAENETRESET:
        return WSAENETRESET_default_;

    case WSAECONNABORTED:
        return WSAECONNABORTED_default_;

    case WSAECONNRESET:
        return WSAECONNRESET_default_;

    case WSAENOBUFS:
        return WSAENOBUFS_default_;

    case WSAEISCONN:
        return WSAEISCONN_default_;

    case WSAENOTCONN:
        return WSAENOTCONN_default_;

    case WSAESHUTDOWN:
        if (caller == "recv")
            return WSAESHUTDOWN_recv_;
        else if (caller == "recvfrom")
            return WSAESHUTDOWN_recvfrom_;
        else if (caller == "send")
            return WSAESHUTDOWN_send_;
        else if (caller == "sendto")
            return WSAESHUTDOWN_sendto_;
        else
            return WSAESHUTDOWN_default_;

    case WSAETIMEDOUT:
        return WSAETIMEDOUT_default_;

    case WSAECONNREFUSED:
        return WSAECONNREFUSED_default_;

    case WSASYSNOTREADY:
        return WSASYSNOTREADY_default_;

    case WSAVERNOTSUPPORTED:
        return WSAVERNOTSUPPORTED_default_;

    case WSANOTINITIALISED:
        return WSANOTINITIALISED_default_;

    case WSAHOST_NOT_FOUND:
        return WSAHOST_NOT_FOUND_default_;

    case WSATRY_AGAIN:
        return WSATRY_AGAIN_default_;

    case WSANO_RECOVERY:
        return WSANO_RECOVERY_default_;

    case WSANO_DATA:
        return WSANO_DATA_default_;

    case WSAEMFILE:
        return WSAEMFILE_default_;

    case WSAEBADF:
        return WSAEBADF_default_;

    case WSAEHOSTDOWN:
        return WSAEHOSTDOWN_default_;

    case WSAEHOSTUNREACH:
        return WSAEHOSTUNREACH_default_;

    case WSAENOTEMPTY:
        return WSAENOTEMPTY_default_;

    case WSAEPROCLIM:
        return WSAEPROCLIM_default_;

    case WSAESTALE:
        return WSAESTALE_default_;

    case WSAEPFNOSUPPORT:
        return WSAEPFNOSUPPORT_default_;

    case WSAENAMETOOLONG:
        return WSAENAMETOOLONG_default_;

    case WSAETOOMANYREFS:
        return WSAETOOMANYREFS_default_;

    case WSAEUSERS:
        return WSAEUSERS_default_;

    case WSAEDQUOT:
        return WSAEDQUOT_default_;

    case WSAELOOP:
        return WSAELOOP_default_;

    case WSAEREMOTE:
        return WSAEREMOTE_default_;

    default:
        return WSAunknown_default_;
    }
}

#else
#ifdef BSD_SOCKETS

// **********************************************************
// support for BSD Platforms
// **********************************************************

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
int installSocketsDLL(const unsigned16&) { return 0; }
void removeSocketsDLL(void) { }

//==============================================================================
//  allow for WinSock's treatment of a socket as a handle
//==============================================================================
//
//    in BSD Sockets, sockets are file descriptors--
//         i.e., small integers--and not a special data type
//    in BSD Sockets, ioctlsocket() and closesocket() doesn't exist;
//         socket close operations are handled by ioctl() and close()
//
int ioctlsocket(SOCKET s, long request_code, p_fio_ioctl_arg_type p_arg)
{
    return ioctl(s, request_code, p_arg);
}

int closesocket(SOCKET s)
{
    return close(s);
}

//=============================================================================
//  in_addr_to_string - function for converting IP addresses to byte strings
//=============================================================================

//---------------------------
//  support routines
//---------------------------
//
// workaround for IRIX's inet_ntoa()--
//     I couldn't make the built-in routine work as required

#ifdef IRIX_GNU_CPP

// ###### hack alert: workaround for problems calling IRIX inet_ntoa() ######
//
#include <iostream>
#include <iomanip>

#ifndef STRING_STREAM_HANDLE_H
#include "string_stream_handle.h"     // string stream handle class
#endif

// non-reentrant, like IRIX's inet_ntoa()
// strstream is needed here for char*

char* my_inet_ntoa(const struct in_addr& adr)
{
    static strstream_handle_class ip_buffer;
    static strstream& ip_bufferRef = *ip_buffer;

    const char* address_as_byte_vector = (char*)&adr.s_addr;

    ip_bufferRef.seekp(0, ios::beg);
    ip_bufferRef << ((unsigned)address_as_byte_vector[0]) << '.'
        << ((unsigned)address_as_byte_vector[1]) << '.'
        << ((unsigned)address_as_byte_vector[2]) << '.'
        << ((unsigned)address_as_byte_vector[3]) << '\0';

    return ip_bufferRef.str();
}

#endif  // ############# IRIX_GNU_CPP #####################

//---------------------------
//  in_addr_to_string
//---------------------------
//
// requires conditional compilation because of problem with IRIX inet_ntoa()

#ifdef IRIX_GNU_CPP

string in_addr_to_string(char** pIpAddr)
{
    return(
        string(inet_ntoa(*reinterpret_cast<struct in_addr*>(*pIpAddr)))
        );
}

#endif  // ############# IRIX_GNU_CPP #######################

#ifdef NETBSD_GNU_CPP

string in_addr_to_string(char** pIpAddr)
{
    return(inet_ntoa(*reinterpret_cast<struct in_addr*> (*pIpAddr)));
}

#endif  // ############# NETBSD_GNU_CPP #####################

#ifdef LINUX_GNU_CPP

string in_addr_to_string(char** pIpAddr)
{
    return(inet_ntoa(*reinterpret_cast<struct in_addr*> (*pIpAddr)));
}

#endif  // ############# NETBSD_GNU_CPP #####################

//==============================================================================
//   SocketAlreadyClosed - determine if a socket was closed when not open
//==============================================================================
//
//   BSD, WinSock return different error codes when a process attempts
//   to reclose a socket that's already been closed.  define function
//   that masks this difference.
//
long socket_already_closed(void)
{
    return EBADF;
}

//=================================================================================
//  get_socket_error_message() -
//     translate name of socket API routine, error code into diagnostic message
//=================================================================================
//
string get_socket_error_message(const long error_code, const string& caller)
{
    return string(strerror(error_code));
}

//==============================================================================
//  get_last_socket_error() - retrieve code number for last socket error
//==============================================================================
//
long get_last_socket_error(void) { return errno; }

#endif  // BSD_SOCKETS
#endif  // WINSOCK

// ##########################################################
// Define IPv4 and IPv6 functionality
// ##########################################################

#ifdef IP_V4

//insert the ip address into the stream
bool ip_address_insert(ostream& os, const ip_address_vector_type& ip_address)
{
    os << static_cast<unsigned>(ip_address[0]) << "."
        << static_cast<unsigned>(ip_address[1]) << "."
        << static_cast<unsigned>(ip_address[2]) << "."
        << static_cast<unsigned>(ip_address[3]);
    return os.good();
}

ip_address_vector_type convert_ip_address_string(istream& is)
{
    static const string caller_name = "convert_ip_address_string";
    static const string bad_ip = "improperly formatted IP address";

    // string must be of form  num1 . num2 . num3 . num4
    unsigned i = 0;
    ip_address_vector_type ip_address(4);

    while (true)    // ends with break in middle  ( look for num, {.num}x3 }
    {
        //  problem:   since ip_address is type unsigned char[],
        //     operator>> will try to get char from stream, rather than byte.
        //
        //  solution:  use operator>> with value of type unsigned,
        //     then cast to unsigned char, checking for range.
        //
        unsigned this_value;

        if (!(is >> this_value))
        {
            stringstream p_message;

            p_message << "couldn't get field " << i << endl;
            if (i > 0)
            {
                p_message << "value so far: ";
                for (unsigned j = 0; j < i; j++)
                    p_message << static_cast<unsigned>(ip_address[j])
                    << ".";
            }
            throw ip_address_format_exception(caller_name, bad_ip,
                p_message.str());
        }

        if (this_value > 255)
        {
            stringstream p_message;
            p_message << "value for field " << i << " (" << this_value
                << ") " << " is too large" << endl;
            if (i > 0)
            {
                p_message << "value so far: ";
                for (unsigned j = 0; j < i; j++)
                    p_message << static_cast<unsigned>(ip_address[j])
                    << ".";
            }
            throw ip_address_format_exception(caller_name, bad_ip,
                p_message.str());
        }

        ip_address[i] = static_cast<unsigned char>(this_value);
        i++;                      // field now successfully parsed

        if (i == 4) break;

        if (is.peek() != '.')
        {
            stringstream p_message;

            p_message << "missing . after field " << i << endl;
            p_message << "value so far: ";
            for (unsigned j = 0; j < i - 1; j++)
                p_message << static_cast<unsigned>(ip_address[j])
                << ".";
            p_message << static_cast<unsigned>(ip_address[i - 1]);
            throw ip_address_format_exception(caller_name, bad_ip,
                p_message.str());
        }
        is.ignore();
    }

    return ip_address;
}

//fill in version specific address information into sockaddr_in
void populate_socket_address_struct(socket_address& input_socket_address,
    unsigned short port_number, ip_version_address_type ip_address_v4)
{
    input_socket_address.sin_family = AF_INET;
    input_socket_address.sin_port = htons(port_number);

    //works for [] to [] copy?
    //QUESTION: works here but not in v6??
    input_socket_address.sin_addr.s_addr = ip_address_v4;
}

//overloaded for INADDR_ANY
void populate_socket_address_struct(socket_address& input_socket_address,
    unsigned short port_number, ip_address_struct ip_address_v4)
{
    input_socket_address.sin_family = AF_INET;
    input_socket_address.sin_port = htons(port_number);

    //works for [] to [] copy?
    input_socket_address.sin_addr.s_addr = ip_address_v4;
}

// Return the port number for a sockaddr_in struct
// Reason: the internal naming is different for v4 than v6

/*inline ip_version_address get_ip_address(const socket_address& input_socket_address)
{
    return input_socket_address.sin_addr;
}*/

//Call the socket level function to convert from sockaddr_in.in_addr to string
char* socket_address_numerical_to_string(const socket_address&
    input_socket_address, char* buffer)
{
    return inet_ntoa(input_socket_address.sin_addr);
}

//And vice-versa
ip_version_address_type socket_address_string_to_numerical(const char*
    socket_address_string)
{
    return inet_addr(socket_address_string);
}

#endif // IP_V4

#ifdef IP_V6

//may not need this since we will do the parsing here
//function prototype for returning the IP family
/*inline char get_ip_address_separator()        //returns ":"
{
    return ":";
}*/

//insert the ip address into the stream
bool ip_address_insert(ostream& os, const ip_address_vector_type& ip_address)
{
    // for IPv6 don't worry about using the :: shortcut notation here,
    // because the full address works fine

    bool status = (os << hex
        << static_cast<unsigned>(ip_address[0]) << ":"
        << static_cast<unsigned>(ip_address[1]) << ":"
        << static_cast<unsigned>(ip_address[2]) << ":"
        << static_cast<unsigned>(ip_address[3]) << ":"
        << static_cast<unsigned>(ip_address[4]) << ":"
        << static_cast<unsigned>(ip_address[5]) << ":"
        << static_cast<unsigned>(ip_address[6]) << ":"
        << static_cast<unsigned>(ip_address[7])
        );

    return status;
}

//fill in version specific address information into sockaddr_in6
void populate_socket_address_struct(socket_address& input_socket_address,
    unsigned short port_number, ip_version_address ip_address_v6)
{
    input_socket_address.sin6_family = AF_INET6;
    input_socket_address.sin6_port = htons(port_number);

    //works for [] to [] copy, NO
    //input_socket_address.sin6_addr.s6_addr = ip_address_v6.s6_addr;
    for (int i = 0; i < 16; ++i)
        input_socket_address.sin6_addr.s6_addr[i] = ip_address_v6.s6_addr[i];
}

//overloaded for INADDR_ANY
void populate_socket_address_struct(socket_address& input_socket_address,
    unsigned short port_number, ip_address_struct ip_address_v6)
{
    populate_socket_address_struct(input_socket_address, port_number,
        ip_version_address());
}

/*inline ip_version_address get_ip_address(const socket_address& input_socket_address)
{
    return input_socket_address.sin6_addr;
}*/

//Call the socket level fucntion to convert from sockaddr_in.in_addr to string
const char* socket_address_numerical_to_string(const socket_address&
    input_socket_address, char* buffer)
{
    //need &input...?
    return inet_ntop(AF_INET6, (void*)&input_socket_address.sin6_addr,
        buffer, INET6_ADDRSTRLEN);
}

//And vice-versa
ip_version_address socket_address_string_to_numerical(const char*
    socket_address_string)
{
    //int inet_pton(int af, const char *src, void *dst);

    ip_version_address address;        //struct in6_addr

    cout << socket_address_string << endl;         //TEST COUT

    if (inet_pton(AF_INET6, socket_address_string, (void*)&address) < 1)
        throw basic_exception("socket_address_string_to_numerical",
            "invalid source string",
            "IP address string passed in was invalid");

    return address;
}

// ==================CODING===========================

//testing - put this in it's own file and test separately

// Throws a basic exception with some helpful error reporting
//
void throw_error(const ip_vector& invalid_address,
    const vector<int>& invalid_positions, string error_message)
{
    string long_error_message;
    string single_error_message;
    string buffer;     //conversion space

    // testing stringstream for conversion
    stringstream ss;

    for (int i = 0; i < invalid_positions.size(); ++i)
    {
        ss << invalid_positions[i];
        ss >> buffer;
        single_error_message = "Field " + buffer
            + " is invalid. Starting at stream position: ";
        ss << invalid_address[invalid_positions[i]].begin_stream_position;
        ss >> buffer;
        single_error_message += buffer;
        single_error_message += " and ending at position: ";
        ss << invalid_address[invalid_positions[i]].end_stream_position;
        ss >> buffer;
        single_error_message += buffer;
        single_error_message += '\n';

        long_error_message += single_error_message;
    }

    throw basic_exception("convert_ip_address_string", error_message,
        long_error_message);
}

// Parse a string within the stream into a IPv6 vector of bytes
ip_address_vector_type convert_ip_address_string(istream& is)
{
    //IP vector level, check format; throw exception or return the ip vector

    //create the ip_address_vector
    ip_vector ip_address_fields = build_ip_vector(is);
    ip_address_vector_type ip_address;
    ip_address.resize(max_fields);    //room for 8 fields

    //now loop through ip_address and check for conditions
    int invalid_fields = 0;
    int empty_fields = 0;
    int valid_fields = 0;
    vector<int> invalid_positions;  //track invalid positions with vector
    int i;

    for (i = 0; i < ip_address_fields.size(); ++i)
    {
        if (ip_address_fields[i].status == 1)
            ++valid_fields;
        else if (ip_address_fields[i].status == 0)
        {
            ++empty_fields;
            if (empty_fields > 1) invalid_positions.push_back(i);
        }
        else
        {
            ++invalid_fields;
            invalid_positions.push_back(i);
        }
    }

    //ERROR conditions
    if ((valid_fields > max_fields) ||        /*if 9 or more fields---*/
        (valid_fields == max_fields &&
            empty_fields == max_empty_fields))         /*8 with one empty*/
        throw_error(ip_address_fields, invalid_positions, "too many fields");

    if (empty_fields > max_empty_fields)    //if 2 or more empties
        throw_error(ip_address_fields, invalid_positions,
            "too many empty fields (::)");

    if (invalid_fields >= 1)    //if any invalid fields
        throw_error(ip_address_fields, invalid_positions, "invalid field(s)");

    if (valid_fields < max_fields && empty_fields <= 0)    //too few fields
        throw_error(ip_address_fields, invalid_positions, "too few fields");

    //else, format into ip_address_vector_type
    for (i = 0; i < ip_address.size(); ++i)
    {
        if (i < ip_address_fields.size())
            if (ip_address_fields[i].status == 0) break;

        ip_address[i] = (unsigned16)ip_address_fields[i].value;
    }

    if (i < ip_address.size())    //if we didn't get it all (because of ::)
    {
        int j = ip_address.size() - 1;    //-1 since it is used as an end index
        while (ip_address_fields.size() - 1 > i)    //again, need the index
        {
            ip_address[j] = ip_address_fields.back().value;
            ip_address_fields.pop_back();

            --j;
        }
        while (j >= i)    //we didn't meet up, so fill the remainder with 0
        {
            ip_address[j] = 0;
            --j;
        }
    }

    return ip_address;
}

ip_vector build_ip_vector(istream& is)
{
    //vector of bytes level, just get the vector of bytes
    ip_field current_field;
    int current_fields_retrieved = 0;
    ip_vector ip_address;
    char buffer;
    bool stream_ended = false;

    const char separator = ':';

    try
    {
        if (is.peek() == separator)    //check for invalid first field
        {
            is >> buffer;     //pull off the :
            if (is.peek() != separator)    //invalid first field, 1 :, but not 2
            {
                current_field.status = -1;
                ip_address.push_back(current_field);
            }
            //else return to allow get_field to detect shortcut notation
            else is.putback(buffer);
        }

        while (current_fields_retrieved < 2 * max_fields) //allow room for error
        {
            current_field = get_field(is, stream_ended);    //get a field

            //push each byte vector in current_field into main vector
            ip_address.push_back(current_field);

            ++current_fields_retrieved;

            if (stream_ended) break;
        }

        //if we didn't finish after 16 words, then bail out totally
        return ip_address;     //just return whatever is there
    }
    catch (const empty_stream_exception& message) //catch exception on finish
    {
        //we're done, one way or the other, return and let upper layer decide
        return ip_address;
    }
}

ip_field get_field(istream& is, bool& stream_ended)
{
    //field level, get one word's worth of data and return it
    char current_char;
    char field_chars[max_char_to_read];    //store each char that is read
    int num_chars_read = 0;
    ip_field current_field;

    //init to zeros
    current_field.status = 0;
    current_field.value = 0;
    strncpy(field_chars, "\0\0\0\0\0\0\0\0", max_char_to_read);

    current_field.begin_stream_position = is.tellg();

    const char separator = ':';

    //read the first char
    while (num_chars_read < max_char_to_read)
    {
        if (is.peek() == separator)
        {
            //if this is the start of a field (ie chars_read = 0)
            if (num_chars_read == 0)
            {
                is >> current_char; //discard the ':'

                if (is.peek() == separator) //check for another ':'
                {
                    //empty field
                    current_field.status = 0;
                    return current_field;
                }
            }
            else break;
            //else this field is done, convert (below), break here, leave :
        }
        if (!(is >> current_char))
        {
            //this may or may not be an error, we cannot tell here, so
            //leave it to code above
            //throw an empty stream exception, and catch above
            if (num_chars_read == 0)
                throw empty_stream_exception("get_field",
                    "stream read error", "stream is empty");

            stream_ended = true;

            break;
        }
        field_chars[num_chars_read] = current_char;
        ++num_chars_read;
    }

    current_field.end_stream_position = is.tellg();

    if (num_chars_read > 4)    //make sure there wasn't too much data
    {
        current_field.status = -1;    //invalid field
        return current_field;
    }

    field_chars[5] = '\0';      //null terminate the char*
    char* invalid_chars;     // rename terminating_char (?)

    unsigned long value = strtoul(field_chars, &invalid_chars, 16);

    if (value > 65535)
    {
        current_field.status = -1;    //invalid field
        return current_field;
    }

    current_field.value = static_cast<unsigned short>(value);
    current_field.status = 1;
    return current_field;
}

//=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* QUESTION =*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
// haven't yet handled the case of IPv4 mapped Ipv6 addresses
// THEY separate with '.'
//=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* QUESTION =*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*

/* example addresses
      FEDC:BA98:7654:3210:FEDC:BA98:7654:3210
      1080:0:0:0:8:800:200C:4171
      3ffe:2a00:100:7031::1
      1080::8:800:200C:417A
      ::192.9.5.5
      ::FFFF:129.144.52.38
      2010:836B:4179::836B:4179*/

      // Class empty_stream_exception

empty_stream_exception::empty_stream_exception(
    const string& name, const string& short_message,
    const string& long_message, const basic_exception& prior_exception)
    : basic_exception(name, short_message, long_message, prior_exception) {}

empty_stream_exception
::empty_stream_exception(const empty_stream_exception& source)
    : basic_exception(source) {}

empty_stream_exception::~empty_stream_exception(void) {}

ostream& operator<<(ostream& os, const empty_stream_exception& ex)
{
    ex.insert(os);
    return os;
}

#endif // IP_V6