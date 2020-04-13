//--------------tcp_server.h-------------------------------------------------------
//
//   Author:   Phil Pfeiffer
//   Date:     August 1999
//   Last modified:  April 2020
//
//   Simplify management of a TCP communication
//   by implementing a TCP server endpoint as an object
//
//   A TCP server socket combines
//     the functionality of a TCP socket (i.e., the initialization methods)
//     with the functionality of server sockets (i.e., bind)
//     and TCP server functions (i.e., listen(), accept())
//--------------------------------------------------------------------------------

#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#ifndef SOCKET_SERVER_H
#include "socket_server.h"
#endif

#ifndef SOCKET_TCP_H
#include "socket_tcp.h"
#endif

class tcp_rollover_socket_class;

// *********************************************************************************
//  the TCP server socket class proper
//
//  definition of a TCP server socket:
//    a socket that supports TCP init methods + Winsock server methods
//    (i.e., bind()) + TCP-specific server methods:
//
//    -.  listen -  announce willingness to receive incoming connections
//    -.  accept -  receive connection, obtaining new port for communications
// *********************************************************************************

// **** important: in inheritance list below,
// ****     tcp_socket_class must precede server_socket_class
// ****     to ensure inheritance of the proper sendto()/recvfrom() methods

class tcp_server_socket_class : public tcp_socket_class, public server_socket_class
{
public:
    // =====================================
    //  constructor
    // =====================================
    //
    //  first form - standard constructor
    //  second form - copy constructor
    //
    tcp_server_socket_class(void);
    tcp_server_socket_class(const tcp_server_socket_class&);

    // ===========================================================
    //  manipulators
    // ===========================================================
    //
    //  the bind socket is the socket that is advertised when a process
    //  offers to accept connections.  The bind socket is also
    //  the socket that is closed when a service finally terminates.
    //
    //  listen- limits number of pending connections in queue at one time.
    //          (API-specified maximum = 5)
    //
    //  accept- obtains an actual connection with an end user --
    //          returning *new* socket for use in end-to-end communications
    //
    void listen(const unsigned = 5);
    void accept(tcp_rollover_socket_class&);

    // ======================================
    //  destructor
    // ======================================
    //
    virtual ~tcp_server_socket_class(void);
};

// *******************************************************************
//  exception classes for tcpServerClass
// *******************************************************************
//
// base class, socket_exception, defined in socket.h.

// derived (specific) exceptions:
//
//   socket_listen_exception - couldn't listen for specified no. of connections
//   socket_accept_exception - couldn't accept connection
//
class socket_listen_exception : public socket_exception
{
public:
    socket_listen_exception(
        const string & = "", const string & = "", const string & = "",
        const basic_exception & = socket_exception());
    socket_listen_exception(const socket_listen_exception& source);
    virtual ~socket_listen_exception(void);
};

class socket_accept_exception : public socket_exception
{
public:
    socket_accept_exception(
        const string & = "", const string & = "", const string & = "",
        const basic_exception & = basic_exception());
    socket_accept_exception(const socket_accept_exception& source);
    virtual ~socket_accept_exception(void);
};

// =*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
//  auxiliary operators for exception classes
// =*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*

ostream& operator<<(ostream&, const socket_listen_exception&);
ostream& operator<<(ostream&, const socket_accept_exception&);

#endif
