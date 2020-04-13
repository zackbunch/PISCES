//--------------socket.cpp----------------------------------------------------------
//
//   Authors:  Phil Pfeiffer, Sridhar Koritala
//   Date:     September 1999
//   Last modified:  April 2020
//
//   Simplify task of managing a communication by implementing socket 
//   functionality as an object.
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
//        identifies buffer+buffer size for stored message
//
//     "service access point" -> (void *) socket_service_access_point_class
//        holds host, IP address associated with message exchange
//        (sendto()/ recvfrom() only)
//
//     "receive count" -> (void *) unsigned *
//        captures number of characters actually received
//            (optional parameter for recv(), recvfrom())
//--------------------------------------------------------------------------------------

#include "socket.h"

#ifndef BOOL_H
#include "bool.h"       // defines TRUE, FALSE, if needed
#endif

#include <sstream>   // stringstream

#ifndef EXCEPTION_H
#include "exception.h"   // exception management class
#endif

#ifndef STREAM_EXCEPTION_H
#include "stream_exception.h"   // stream exceptions class
#endif

#ifndef SOCKET_ERROR_MESSAGE_H
#include "socket_error_message.h"   // socket error code->message class
#endif

// **************************************************************************
// socket class
// **************************************************************************

//  +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=
//  static data items
//  +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=

//  strings below define keys for argument blocks.
//  part of protocol for class send/sendto/recv/recvfrom routines

const string socket_class::P_BUFFER_ADDRESS = "buffer address ptr";
const string socket_class::P_BUFFER_SIZE = "buffer size ptr";
const string socket_class::P_RECEIVE_COUNT = "receive count ptr";
const string socket_class::P_SERVER_ACCESS_POINT = "service access point ptr";

// +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=
//  socket class proper
// +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=

// ========================================================================
//  constructors
// ========================================================================

//-------------------------------------------------------------
//  first form - specify version of sockets DLL
//-------------------------------------------------------------
//
socket_class::socket_class(const unsigned16 dll_version)
    : dll_init_object_(dll_version), socket_(INVALID_SOCKET)
{
}

//-----------------------------------------------------------------------
//  second form - copy constructor
//------------------------------------------------------------------------
//
socket_class::socket_class(const socket_class& source)
    : dll_init_object_(source.dll_init_object_),
    socket_(source.socket_)
{
}

// ========================================================================
//  assignment
// ========================================================================

//-----------------------------------------------------------------------
//  assignment, first form - standard object copy
//------------------------------------------------------------------------
//
socket_class& socket_class::operator=(const socket_class& source)
{
    socket_ = source.socket_;
    dll_init_object_ = source.dll_init_object_;
    return *this;
}

// ========================================================================
//  inspectors
// ========================================================================

// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//  methods for determining message attributes
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

//-----------------------------------------------------------------=-=-=-=-=-=-=-=-
//  bytes_pending -
//     determine number of bytes available to receive in system input buffers
//
//  Comments in Quinn and Shute:
//     "Some TCP/IP stacks (for stream sockets) may not report total bytes
//      across network-system packet buffer boundaries.... 
//     "An application typically uses ioctlsocket( ) FIONREAD to determine a
//      buffer size to allocate, or perhaps to check for the arrival of a
//      specific amount of data.  We recommend against either of these uses.
//      They are unnecessary, and this operation represents an overhead your
//      can avoid, not to mention that it implicitly misuses system buffers....
//     "... simply preallocate the largest buffer your application will
//      possibly need..."
//
//  Concerns with Quinn and Shute:
//      *.  preallocation won't work when application does not know,
//          ahead of time, the protocol in use.
//-----------------------------------------------------------------=-=-=-=-=-=-=-=-
//
unsigned long socket_class::bytes_pending(void) const
{
    // **** set up static data for error messages ****

    static const string my_name = "socket_class::bytes_pending";
    static const string no_count = "can't read count of pending bytes";

    // **** get, return byte count for user ****

    fio_ioctl_arg_type bytes_pending_count = 0;   // this assignment quiets warning C6001 (using uninitialized memory)
    p_fio_ioctl_arg_type p_bytes_pending_count = reinterpret_cast<p_fio_ioctl_arg_type>(&bytes_pending_count);

    if (ioctlsocket(socket_, FIONREAD, p_bytes_pending_count) == SOCKET_ERROR)
        throw socket_option_exception(my_name, no_count, socket_error_message_class("ioctlsocket").get_message());

    return static_cast<unsigned long>(bytes_pending_count);
}

// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//  methods for determining socket attributes
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

//---------------------------------------------------------------------
//  service_access_point() -
//      get service access point (IP, DNS, port) associated with this socket
//---------------------------------------------------------------------
//
socket_service_access_point_class socket_class::service_access_point(void) const
{
    // **** set up static data for error messages ****

    static const string my_name = "socket_class::service_access_point";
    static const string no_host = "can't get host info for local socket";
    static const string no_port = "can't get port number for local socket";

    // **** go get required data ****

    try
    {
        // **** get data on current host ****
        //
        socket_service_access_point_class result;

        // **** access local port data, using getsockname() method ****

        // === store size of typical host_data buffer in variable, because ===
        // === getsockname() wants a pointer (!) to a length variable     ===

        socket_address host_data;
        sockAddrBufferLenType host_data_length = sizeof(socket_address);

        // === wrap standard socket call in error-checking logic ===
        //
        struct sockaddr* host_data_ref = reinterpret_cast<struct sockaddr*>(&host_data);
        if (getsockname(socket_, host_data_ref, &host_data_length) == SOCKET_ERROR)
            throw socket_service_access_point_exception(my_name, no_port, socket_error_message_class("getsockname").get_message());

        // ==== port number is a network short-- a two-byte quantity,
        // ==== in network order.  adjust port number for network ordering!
        //
        port_number_class sap_port_number(ntohs(get_port_number(host_data)));
        result = sap_port_number;
        return result;
    }

    //  socket_service_access_point_exceptions already name this routine; simply rethrow, unchanged
    //  update diagnostic for other exceptions (i.e., from result() init.)
    //
    catch (const socket_service_access_point_exception&)
    {
        throw;
    }
    catch (const basic_exception& message)
    {
        throw socket_service_access_point_exception(my_name, no_host, "", message);
    }
}

//-------------------------------------------------------------------------------
//  peer_service_access_point() -
//      get service access point (IP, DNS, port) associated with peer socket
//-------------------------------------------------------------------------------
//
socket_service_access_point_class socket_class::peer_service_access_point(void) const
{
    // **** set up static data for error messages ****

    static const string my_name = "socket_class::peer_service_access_point";
    static const string no_peer = "can't get data on peer socket";

    // **** go get required data ****

    try
    {
        // **** get data on peer host ****

        // === store size of typical host_data buffer in variable, because ===
        // === getsockname() wants a pointer (!) to a length variable     ===

        socket_address host_data;
        sockAddrBufferLenType host_data_length = sizeof(socket_address);

        // === wrap standard socket call in error-checking logic
        //
        struct sockaddr* host_data_ref = reinterpret_cast<struct sockaddr*>(&host_data);
        if (getpeername(socket_, host_data_ref, &host_data_length) == SOCKET_ERROR)
            throw socket_service_access_point_exception(my_name, no_peer, socket_error_message_class("getpeername").get_message());

        // === return data obtained from getpeername() ====
        const ip_address_class peer_ip_addr(host_data);
        const port_number_class peer_port(ntohs(get_port_number(host_data)));
        return socket_service_access_point_class(peer_ip_addr, peer_port);
    }

    //  socket_service_access_point_exceptions already name this routine; simply rethrow, unchanged
    //  update diagnostic for other exceptions (i.e., from result() init.)
    //
    catch (const socket_service_access_point_exception&)
    {
        throw;
    }
    catch (const basic_exception& message)
    {
        throw socket_service_access_point_exception(my_name, no_peer, "", message);
    }
}

//-------------------------------------------------------------------------------------
//  get_sock_option -  report socket characteristics, via call to getsockopt()
//
//  exact meaning of parameters depends on value of "level" parameter.
//  data returned via option_val, option_len parameters.
//
//  see a good reference on sockets (e.g., Quinn and Shute) for details.
//-------------------------------------------------------------------------------------
//
void socket_class::get_sock_option(const int level, const int option_name, char* const pOptval, int* const pOptlen) const
{
    // **** set up static data for error messages ****

    static const string my_name = "socket_class::get_sock_option";
    static const string no_data = "can't get socket attributes";

    // **** get, return data for user ****

    sockOptionResultLenType optionLength = *pOptlen;
    if (::getsockopt(socket_, level, option_name, pOptval, &optionLength) == SOCKET_ERROR)
        throw socket_option_exception(my_name, no_data, socket_error_message_class("getsockopt").get_message());
}

// ========================================================
//   manipulators
// ========================================================

// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//  methods for altering socket attributes
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

//-------------------------------------------------------------------------------
// set_blocking() - set socket to run as blocking (synchronous) socket (default)
//-------------------------------------------------------------------------------
//
void socket_class::set_blocking(void)
{
    // **** set up static data for error messages ****

    static const string my_name = "socket_class::set_nonblocking";
    static const string no_block = "can't set socket mode to blocking";

    // **** set socket to work as blocking ***

    fio_ioctl_arg_type blocking_operation = 0;
    p_fio_ioctl_arg_type p_blocking_operation = reinterpret_cast<p_fio_ioctl_arg_type>(&blocking_operation);

    if (ioctlsocket(socket_, FIONBIO, p_blocking_operation) == SOCKET_ERROR)
        throw socket_option_exception(my_name, no_block, socket_error_message_class("ioctlsocket").get_message());
}

//-------------------------------------------------------------------------------
// set_nonblocking() - set socket to run as nonblocking (asynchronous) socket
//-------------------------------------------------------------------------------
//
void socket_class::set_nonblocking(void)
{
    // **** set up static data for error messages ****

    static const string my_name = "socket_class::set_nonblocking";
    static const string no_nonblocking = "can't set socket mode to nonblocking";

    // **** set socket to work as nonblocking ***

    fio_ioctl_arg_type NONblocking_operation = 1;   // any nonzero value will do
    p_fio_ioctl_arg_type pNONblocking_operation = reinterpret_cast<p_fio_ioctl_arg_type>(&NONblocking_operation);

    if (ioctlsocket(socket_, FIONBIO, pNONblocking_operation) == SOCKET_ERROR)
        throw socket_option_exception(my_name, no_nonblocking, socket_error_message_class("ioctlsocket").get_message());
}

//---------------------------------------------------------------------------
//  setSockOpt() -  update socket characteristics, via call to setsockopt()
//
//  exact meaning of parameters depends on value of "level" parameter.
//  data returned via option_val, option_len parameters.
//
//  see a good reference on sockets (e.g., Quinn and Shute) for details.
//---------------------------------------------------------------------------
//
void socket_class::setSockOpt(const int level, const int option_name, const char* const pOptval, const int option_len)
{
    // **** set up static data for error messages ****

    static const string my_name = "socket_class::get_sock_option";
    static const string no_attribute = "can't change socket attributes";

    // **** attempt to update socket operating options ****

    if (::setsockopt(socket_, level, option_name, pOptval, option_len) == SOCKET_ERROR)
        throw socket_option_exception(my_name, no_attribute, socket_error_message_class("setsockopt").get_message());
}

// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//  methods for managing socket communications
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

// =---=---=---=---=---=---=---=---=---=---=---=---=---=---=---=---=---=---
// send/recv/sendto/recvfrom primitives - exchange data with host
//
// all four functions are virtual functions that, in an ideal language,
// would be variadic functions like printf--
//     i.e., functions that could accept a varying number of parameters
//     of varying types.
//
// to get this effect of variadic functions in a language where
//    functions must take a fixed number of arguments of fixed types,
// we'll use blocks of "thing pointers"--
//    collections of pointers of type "void *".
//
// to make these blocks of "thing pointers" somewhat more safe to use,
// we'll use "maps" --
//    here, associations of strings with pointers,
//    where the strings serve as surrogate types for pointed-to objects.
// =---=---=---=---=---=---=---=---=---=---=---=---=---=---=---=---=---=---

//--------------------------------------------------------------------------------
//  send-- send message of specified length, in specified buffer, to peer
//
//   assumes:  prior connection with peer
//   parameters:
//
//     the pair of maps
//           "char buffer"  -> (void *) char*
//           "buffer size"  -> (void *) unsigned*
//                define buffer with message to send,
//                with size of value to send
//
//   action:  sends the message specified by the maps,
//   with preference given to the messageBuffer params, if present
//--------------------------------------------------------------------------------
//
void socket_class::send(argument_block_type& arg_block)
{
    // **** set up static data for error messages ****

    static const string my_name = "socket_class::send";

    // **** retrieve parameters from argument block ****

    char* p_buffer = 0;
    unsigned* p_size = 0;

    get_buffer_args(arg_block, p_buffer, p_size, my_name);

    // **** do the send ****

    send(p_buffer, *p_size);
}

//----------------------------------------------------------------------
//  recv-- receive message, using specified buffer of specified length
//    return number of bytes actually received as reference parameter
//
//   assumes:  prior connection with peer
//   parameters:
//
//      the pair of maps
//           "char buffer"  -> (void *) char*
//           "buffer size"  -> (void *) unsigned*
//                define buffer with message to send,
//                with size of value to send
//
//     and optionally the map
//           "receive count" -> (void *) unsigned *
//                // captures number of characters actually received
//
//   action:  sends the message specified by the maps,
//   with preference given to the messageBuffer params, if present
//----------------------------------------------------------------------
//
void socket_class::recv(argument_block_type& arg_block)
{
    // **** set up static data for error messages ****

    static const string my_name = "socket_class::recv";

    // **** retrieve parameters from argument block ****

    char* p_buffer = 0;
    unsigned* p_size = 0;
    unsigned* p_receive_count = 0;
    unsigned dummy_receive_count = 0;

    get_buffer_args(arg_block, p_buffer, p_size, my_name);
    get_recv_count_args(arg_block, p_receive_count);

    // **** set up parameters, do the send ****

    if (p_receive_count == 0) p_receive_count = &dummy_receive_count;

    recv(p_buffer, *p_size, *p_receive_count);
}

//----------------------------------------------------------------------
//  sendto-- send message of specified length, in specified buffer,
//         to specified host
//
//
//   parameters:
//
//     an object of argument block type that defines the maps
//           "ip address" ->  (void *) ip_address_class*
//           "port number" -> (void *) port_number_class*
//                // defines target IP address, port for message
//
//      the pair of maps
//           "char buffer"  -> (void *) char*
//           "buffer size"  -> (void *) unsigned*
//                // define buffer with message to send,
//                // with size of value to send
//
//   action:  sends the message specified by the maps,
//   with preference given to the messageBuffer params, if present
//----------------------------------------------------------------------
//
void socket_class::sendto(argument_block_type& arg_block)
{
    // **** set up static data for error messages ****

    static const string my_name = "socket_class::sendto";

    // **** retrieve parameters from argument block ****

    char* p_buffer = 0;
    unsigned* p_size = 0;
    socket_service_access_point_class* p_access_point = 0;

    get_buffer_args(arg_block, p_buffer, p_size, my_name);
    get_access_point_args(arg_block, p_access_point, my_name);

    // **** do the send ****

    sendto(p_buffer, *p_size, *p_access_point);
}

//--------------------------------------------------------------------------------
//  recvfrom-- receive message, using specified buffer of specified length,
//     obtaining IP address and port of message origin.
//     also updates sendingIpAddr and sendingPortno parameters.
//    return number of bytes actually received as reference parameter
//
//   parameters:
//
//     an object of argument block type that defines the maps
//           "ip address" ->  (void *) ip_address_class*
//           "port number" -> (void *) port_number_class*
//                // defines objects for holding IP address,
//                // port from which message came
//
//     the pair of maps
//           "char buffer"  -> (void *) char*
//           "buffer size"  -> (void *) unsigned*
//                // define buffer with message to send,
//                // with size of value to send
//
//     and optionally the map
//           "receive count" -> (void *) unsigned *
//                // captures number of characters actually received
//
//   action:  sends the message specified by the maps,
//   with preference given to the messageBuffer params, if present
//--------------------------------------------------------------------------------
//
void socket_class::recvfrom(argument_block_type& arg_block)
{
    // **** set up static data for error messages ****

    static const string my_name = "socket_class::recvfrom";

    // **** retrieve parameters from argument block ****

    char* p_buffer = 0;
    unsigned* p_size = 0;
    socket_service_access_point_class* p_access_point = 0;
    unsigned* p_receive_count = 0;

    get_buffer_args(arg_block, p_buffer, p_size, my_name);
    get_access_point_args(arg_block, p_access_point, my_name);
    get_recv_count_args(arg_block, p_receive_count);

    // **** set up parameters, do the send ****

    unsigned dummy_receive_count;
    if (p_receive_count == 0) p_receive_count = &dummy_receive_count;

    recvfrom(p_buffer, *p_size, *p_access_point, *p_receive_count);
}

// =---=---=---=---=---=---=---=---=---=---=---=---=---=---=---=---=---=---=---=---=---=---=
// socket API-like forms of send/recv/sendto/recvfrom primitives - exchange data with host
// =---=---=---=---=---=---=---=---=---=---=---=---=---=---=---=---=---=---=---=---=---=---=

//------------------------------------------------------------------------------
//  send-- send message of specified length, in specified buffer, to peer
//        (prior connection assumed)
//------------------------------------------------------------------------------
//
void socket_class::send(const char* const message, const unsigned message_length)
{
    // **** set up static data for error messages ****

    static const string my_name = "socket_class::send";
    static const string no_send = "can't send message";

    // **** attempt the send ****

    // === use the Socket API's version of sendto()
    //
    // flags byte = 0  => message is routed, and sent as in-band data

    int length = static_cast<int>(message_length);

    if (::send(socket_, message, length, 0) == SOCKET_ERROR)
        throw socket_send_exception(my_name, no_send, socket_error_message_class("send").get_message());
}

//------------------------------------------------------------------------------
//  recv-- receive message, using specified buffer of specified length
//         return number of bytes actually received as reference parameter
//         (prior connection assumed)
//------------------------------------------------------------------------------
//
void socket_class::recv(char* const buffer, const unsigned buffer_size, unsigned& recv_count)
{
    // **** set up static data for error messages ****

    static const string my_name = "socket_class::recv";
    static const string no_recv = "didn't receive message correctly";
    static const string no_zero = "zero-length receive not supported";

    // **** attempt the receive ****

    // ==== initialize the attempt ====

    recv_count = 0;     // no data yet received

    // ==== reject requests to receive 0 bytes  ====
    //
    if (buffer_size == 0) throw socket_empty_recv_exception(my_name, no_recv, no_zero);

    // ==== use the Socket API's version of recv() ====

    // final argument of 0 used as a flag that directs API to
    //       remove the message from the system buffer, and
    //       ignore out-of-band data
    //
    int recv_status = ::recv(socket_, buffer, buffer_size, 0);

    // **** audit the receive ****

    // === check first if receive succeeded at all ===
    //
    if (recv_status == SOCKET_ERROR)
        throw socket_recv_exception(my_name, no_recv, socket_error_message_class("recv").get_message());

    recv_count = recv_status;     // report bytes actually received

    // ==== check transmission had expected amount of data ===
    //
    if (recv_count < buffer_size)
    {
        stringstream p_message;

        p_message << buffer_size << " byte";
        if (buffer_size != 1) p_message << "s";
        p_message << " requested, " << recv_count << " actually received ";
        if (recv_count == 0) p_message << "(probable disconnect)";

        if (recv_count == 0)
            throw socket_empty_recv_exception(my_name, no_recv, p_message.str());
        throw socket_deficient_recv_exception(my_name, no_recv, p_message.str());
    }
}

//---------------------------------------------------------------------------------------
//  sendto-- send message of specified length, in specified buffer, to specified host
//---------------------------------------------------------------------------------------
//
void socket_class::sendto(const char* const message, const unsigned message_length, const socket_service_access_point_class& service_access_point)
{
    // **** set up static data for error messages ****

    static const string my_name = "socket_class::sendto";
    static const string no_send = "can't send message";

    // **** attempt the send ****

    // ==== get required data from user objects ====

    const string target_ip_address_string = service_access_point.ip_address().ip_address_as_string();

    const port_number_type target_port_number = service_access_point.port_object().port();

    // ==== init data for sendto() operation ====

    // includes port number, receiver IP address, and--just in case--
    //     zero sin_zero field of host_structure.
    // call to htons() ensures that port number, in buffer, is in standard
    //     (network) byte ordering, as required by socket library spec
    //
    socket_address host;

    populate_socket_address_struct(host, target_port_number, socket_address_string_to_numerical(target_ip_address_string.c_str()));

    // === use the Socket API's version of sendto()

    struct sockaddr* const host_reference = reinterpret_cast<struct sockaddr*>(&host);
    const int length = static_cast<int>(message_length);

    if (::sendto(socket_, message, length, 0, host_reference, sizeof(host)) == SOCKET_ERROR)
        throw socket_send_exception(my_name, no_send, socket_error_message_class("sendto").get_message());
}

//------------------------------------------------------------------------------
//  recvfrom-- receive message, using specified buffer of specified length,
//     obtaining IP address and port of message origin.
//     also updates sendingIpAddr and sendingPortno parameters.
//    return number of bytes actually received as reference parameter
//------------------------------------------------------------------------------
//
void socket_class::recvfrom(char* const buffer, const unsigned buffer_size, socket_service_access_point_class& service_access_point, unsigned& recv_count)
{
    // **** set up static data for error messages ****

    static const string my_name = "socket_class::recvfrom";
    static const string no_recv = "didn't receive message correctly";
    static const string no_zero = "zero-length receive not supported";

    // **** attempt the receive ****

    // ==== initialize the attempt ====

    recv_count = 0;     // no data yet received

    // ==== reject requests to receive 0 bytes  ====
    //
    if (buffer_size == 0) throw socket_empty_recv_exception(my_name, no_recv, no_zero);

    // ==== use the Socket API's version of recvfrom() ====

    // final argument of 0 used as a flag that directs API to
    //       remove the message from the system buffer, and
    //       ignore out-of-band data
    //
    socket_address host;

    populate_socket_address_struct(host, 0, (ip_address_struct)(NULL));     //only set family

    struct sockaddr* const host_reference = reinterpret_cast<struct sockaddr*>(&host);

    sockAddrBufferLenType len = sizeof(host);

    int recv_status = ::recvfrom(socket_, buffer, buffer_size, 0, host_reference, &len);

    // **** audit the receive ****

    // ==== check first if receive succeeded, period ====
    //
    if (recv_status == SOCKET_ERROR)
        throw socket_recv_exception(my_name, no_recv, socket_error_message_class("recvfrom").get_message());

    // ==== get service access point (port number, IP address) of sender ====

    //   must undo network byte order when retrieving the port number.
    //   ( ntohs() -- return local host byte number form of short integer )

    service_access_point = socket_service_access_point_class(ip_address_class(host), port_number_class(ntohs(get_port_number(host))));

    // ==== check transmission had expected amount of data ===
    //
    recv_count = recv_status;   // status, if >= 0, has count of bytes received

    if (recv_count < buffer_size)
    {
        stringstream p_message;

        p_message << buffer_size << " byte";
        if (buffer_size != 1) p_message << "s";
        p_message << " requested, " << recv_count << " actually received ";

        if (recv_count == 0)
            throw socket_empty_recv_exception(my_name, no_recv, p_message.str());

        throw socket_deficient_recv_exception(my_name, no_recv, p_message.str());
    }
}

// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//  close processing on socket
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

//----------------------------------------------------------------
// closesocket() - terminate processing on socket
//----------------------------------------------------------------
//
void socket_class::closesocket(void)
{
    // **** set up static data for error messages ****

    static const string my_name = "socket_class::closesocket";
    static const string no_close = "can't close socket";

    // **** attempt to close socket ****
    //
    // ignore close errors if socket is already closed.

    if ((::closesocket(socket_) == SOCKET_ERROR) && (get_last_socket_error() != socket_already_closed()))
        throw socket_close_exception(my_name, no_close, socket_error_message_class("closesocket").get_message());
}

// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//  standard argument block parameter access methods
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

//  all routines throw exceptions on failure.
//  final argument to every routine is string that names caller--
//      for diagnostic messages

//---------------------------------------------------------------------------
//  get_buffer_args()
//    recover pointers to buffer, buffer size arguments in arg block arg,
//    throwing exception on failure
//
//    note that method preferentially checks for message buffer in block
//    before looking for classic char* buffer/ size pair.
//---------------------------------------------------------------------------
//
void socket_class::get_buffer_args(argument_block_type& arg_block, char*& p_buffer, unsigned*& p_unsigned_buffer_size, const string& caller_name)
{
    // **** set up static data for error messages ****

    static const string bad_block = "buffer parameter missing from arg block";

    // **** get buffer/ buffer size ptr pair ***

    argument_block_type::iterator p_string_buffer_ptr_pair = arg_block.find(P_BUFFER_ADDRESS);

    if (p_string_buffer_ptr_pair == arg_block.end())
        throw socket_missing_argument_exception(caller_name, bad_block, "buffer pointer parameter missing");

    argument_block_type::iterator p_string_size_ptr_pair = arg_block.find(P_BUFFER_SIZE);

    if (p_string_size_ptr_pair == arg_block.end())
        throw socket_missing_argument_exception(caller_name, bad_block, "buffer pointer parameter present, size parameter missing");

    p_buffer = reinterpret_cast<char*>((*p_string_buffer_ptr_pair).second);
    p_unsigned_buffer_size = reinterpret_cast<unsigned*>((*p_string_size_ptr_pair).second);
}

//---------------------------------------------------------------------------
//  get_access_point_args()
//    recover pointer to service access point argument in arg block arg,
//    throwing exception on failure
//---------------------------------------------------------------------------
//
void socket_class::get_access_point_args(
    argument_block_type& arg_block,
    socket_service_access_point_class*& p_service_access_point,
    const string& caller_name
)
{
    // **** set up static data for error messages ****

    static const string bad_block = "service access point parameter missing from arg block";

    // **** get service access pointer ***

    argument_block_type::iterator p_string_sap_ptr_pair = arg_block.find(P_SERVER_ACCESS_POINT);
    if (p_string_sap_ptr_pair == arg_block.end())
        throw socket_missing_argument_exception(caller_name, bad_block);

    p_service_access_point = reinterpret_cast<socket_service_access_point_class*>((*p_string_sap_ptr_pair).second);
}

//------------------------------------------------------------
//  get_recv_count_args()
//    recover pointer to receive count argument in arg block arg,
//    setting receive count parameter to null on failure
//------------------------------------------------------------
//
void socket_class::get_recv_count_args(argument_block_type& arg_block, unsigned*& p_unsigned_receive_count)
{
    // *** return receive count ptr if there, else null ptr ***

    argument_block_type::iterator p_string_receive_count_ptr_pair = arg_block.find(P_RECEIVE_COUNT);

    if (p_string_receive_count_ptr_pair == arg_block.end())
        p_unsigned_receive_count = 0;
    else
        p_unsigned_receive_count = reinterpret_cast<unsigned*>((*p_string_receive_count_ptr_pair).second);
}

// ========================================================
//   other (logical) operators
// ========================================================

//-----------------------------------------------------------------------------
//  operator== - check if *this == right_operand, producing TRUE/FALSE value
//-----------------------------------------------------------------------------
//
int socket_class::operator==(const socket_class& right_operand) const
{
    // equality iff underlying sockets are same
    //
    return (this->socket_ == right_operand.socket_);
}

//-----------------------------------------------------------------------------
//  operator!= - check if *this != right_operand, producing TRUE/FALSE value
//-----------------------------------------------------------------------------
//
int socket_class::operator!=(const socket_class& right_operand) const
{
    // equality iff underlying sockets are same
    //
    return (this->socket_ != right_operand.socket_);
}

// ========================================================
//   facilitators
// ========================================================

//----------------------------------------------------------------------------------
//  insert_socket_class_data() - dump data on current socket to specified ostream
//
// === needed: a first-class data dump utility ===
//
// finished version of this routine should report protocol suite (PF_INET),
//    protocol type, and protocol name
//    (but there might not be any socket API calls for recovering type and
//     name data from an initialized socket?)
//
// finished version of this routine should also use
// -.   getsockopt() to discover socket operating modes
// -.   ioctlsocket() SIOCATMARK to check for pending out-of-band data
//----------------------------------------------------------------------------------
//
void socket_class::insert_socket_class_data(ostream& os)
{
    //  **** initialize static constant data ****
    //
    static const string my_name = "insert_socket_class_data::extract";
    static const string no_write = "could not put value to outstream";

    //  **** dump socket-related data to stream--whatever we can get ****
    //
    os << "socket number: " << socket_ << endl;

    try {
        const socket_service_access_point_class local_sap = service_access_point();
        os << "local service access point: " << local_sap << endl;
    }
    catch (const socket_exception&) {
    }
    try {
        const socket_service_access_point_class remote_sap = peer_service_access_point();
        os << "remote service access point: " << remote_sap << endl;
    }
    catch (const socket_exception&) {
    }
    try {
        unsigned long pending = bytes_pending();
        os << "bytes pending: " << pending << endl;
    }
    catch (const socket_exception&) {
    }

    //  **** finish with "dump succeeded" check ****
    //
    if (!os.good()) throw outstream_io_error_exception(os, my_name, no_write);
}

//=====================================================================
//  destructors
//=====================================================================
//
socket_class::~socket_class(void)
{
    //  can't automatically close socket on destruct, so long as
    //  *.  copy and assignment operators are supported, and
    //  *.  no mechanism is in place for reference counting by port number
}

// +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=
//  socket class auxiliary operators
// +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=

// ------------------------------------------------------------------------
// dump data regarding specified socket to specified ostream
// ------------------------------------------------------------------------

ostream& operator<<(ostream& os, socket_class& sock)
{
    sock.insert_socket_class_data(os);
    return os;
}

// **************************************************************************
// exception classes for basic socket routines  [boilerplate]
// **************************************************************************

socket_exception::socket_exception(
    const string& name, const string& short_message, const string& long_message,
    const basic_exception& prior_exception)
    : basic_exception(name, short_message, long_message, prior_exception) {}

socket_exception::socket_exception(const socket_exception& source)
    : basic_exception(source) {}

socket_exception::~socket_exception(void) {}

socket_missing_argument_exception::socket_missing_argument_exception(
    const string& name, const string& short_message, const string& long_message,
    const basic_exception& prior_exception)
    : socket_exception(name, short_message, long_message, prior_exception) {}

socket_missing_argument_exception::socket_missing_argument_exception(const socket_missing_argument_exception& source)
    : socket_exception(source) {}

socket_missing_argument_exception::~socket_missing_argument_exception(void) {}

socket_service_access_point_exception::socket_service_access_point_exception(
    const string& name, const string& short_message, const string& long_message,
    const basic_exception& prior_exception)
    : socket_exception(name, short_message, long_message, prior_exception) {}

socket_service_access_point_exception::socket_service_access_point_exception(const socket_service_access_point_exception& source)
    : socket_exception(source) {}

socket_service_access_point_exception::~socket_service_access_point_exception(void) {}

socket_send_exception::socket_send_exception(
    const string& name, const string& short_message, const string& long_message,
    const basic_exception& prior_exception)
    : socket_exception(name, short_message, long_message, prior_exception) {}

socket_send_exception::socket_send_exception(const socket_send_exception& source)
    : socket_exception(source) {}

socket_send_exception::~socket_send_exception(void) {}

socket_recv_exception::socket_recv_exception(
    const string& name, const string& short_message, const string& long_message,
    const basic_exception& prior_exception)
    : socket_exception(name, short_message, long_message, prior_exception) {}

socket_recv_exception::socket_recv_exception(const socket_recv_exception& source)
    : socket_exception(source) {}

socket_recv_exception::~socket_recv_exception(void) {}

socket_bytes_pending_exception::socket_bytes_pending_exception(
    const string& name, const string& short_message, const string& long_message,
    const basic_exception& prior_exception)
    : socket_exception(name, short_message, long_message, prior_exception) {}

socket_bytes_pending_exception::socket_bytes_pending_exception(const socket_bytes_pending_exception& source)
    : socket_exception(source) {}

socket_bytes_pending_exception::~socket_bytes_pending_exception(void) {}

socket_option_exception::socket_option_exception(
    const string& name, const string& short_message, const string& long_message,
    const basic_exception& prior_exception)
    : socket_exception(name, short_message, long_message, prior_exception) {}

socket_option_exception::socket_option_exception(const socket_option_exception& source)
    : socket_exception(source) {}

socket_option_exception::~socket_option_exception(void) {}

socket_close_exception::socket_close_exception(
    const string& name, const string& short_message, const string& long_message,
    const basic_exception& prior_exception)
    : socket_exception(name, short_message, long_message, prior_exception) {}

socket_close_exception::socket_close_exception(const socket_close_exception& source)
    : socket_exception(source) {}

socket_close_exception::~socket_close_exception(void) {}

socket_empty_recv_exception::socket_empty_recv_exception(
    const string& name, const string& short_message, const string& long_message,
    const basic_exception& prior_exception)
    : socket_recv_exception(name, short_message, long_message, prior_exception) {}

socket_empty_recv_exception::socket_empty_recv_exception(const socket_empty_recv_exception& source)
    : socket_recv_exception(source) {}

socket_empty_recv_exception::~socket_empty_recv_exception(void) {}

socket_deficient_recv_exception::socket_deficient_recv_exception(
    const string& name, const string& short_message, const string& long_message,
    const basic_exception& prior_exception)
    : socket_recv_exception(name, short_message, long_message, prior_exception) {}

socket_deficient_recv_exception::socket_deficient_recv_exception(const socket_deficient_recv_exception& source)
    : socket_recv_exception(source) {}

socket_deficient_recv_exception::~socket_deficient_recv_exception(void) {}

// *===*====*===*====*===*====*===*====*===*====*===*====*===*====*===*====
// auxiliary operators  [boilerplate]
// *===*====*===*====*===*====*===*====*===*====*===*====*===*====*===*====

ostream& operator<<(ostream& os, const socket_exception& ex)
{
    ex.insert(os);
    return os;
}

ostream& operator<<(ostream& os, const socket_missing_argument_exception& ex)
{
    ex.insert(os);
    return os;
}

ostream& operator<<(ostream& os, const socket_service_access_point_exception& ex)
{
    ex.insert(os);
    return os;
}

ostream& operator<<(ostream& os, const socket_send_exception& ex)
{
    ex.insert(os);
    return os;
}

ostream& operator<<(ostream& os, const socket_recv_exception& ex)
{
    ex.insert(os);
    return os;
}

ostream& operator<<(ostream& os, const socket_option_exception& ex)
{
    ex.insert(os);
    return os;
}

ostream& operator<<(ostream& os, const socket_bytes_pending_exception& ex)
{
    ex.insert(os);
    return os;
}

ostream& operator<<(ostream& os, const socket_close_exception& ex)
{
    ex.insert(os);
    return os;
}

ostream& operator<<(ostream& os, const socket_empty_recv_exception& ex)
{
    ex.insert(os);
    return os;
}

ostream& operator<<(ostream& os, const socket_deficient_recv_exception& ex)
{
    ex.insert(os);
    return os;
}