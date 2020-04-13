//------------------------------ service_access_point.h ------------------------------
//
//   Author:   Phil Pfeiffer
//   Date:     August 1999
//   Last modified:  April 2020
//
//   Capture a TCP/UDP service access point, consisting of
//   -.  a host specification (IP address),
//   -.  a port (unsigned integer between 0 and 65535, inclusive)
//   along with display and initialization methods, as an object.
//------------------------------------------------------------------------------------

#ifndef SERVICE_ACCESS_POINT_H
#define SERVICE_ACCESS_POINT_H

#include <iostream>
#include <string>

#ifndef PLATFORM_H
#include "platform.h"
#endif

#ifndef STD_H
#include "std.h"
#endif

#ifndef TYPES_H
#include "types.h"         // unsigned32, etc.
#endif //TYPES_H

#ifndef DLLINIT_H
#include "dllinit.h"       // self-loading DLL module
#endif //DLLINIT_H

#ifndef IPADDR_H
#include "ip_address.h"        // IP address specifier object
#endif

#ifndef PORT_NUMBER_H
#include "port_number.h"       // port number object
#endif

// **************************************************************************
// socket_service_access_point_class
// **************************************************************************

// =*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
// socket_service_access_point_class proper
// =*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*

class socket_service_access_point_class
{
public:
    // ======================================
    //  constructors
    // ======================================
    //
    // first form:
    //    instantiate object with specified ip_address object, port.
    //    address defaults to current host;  port defaults to 0.
    //
    // second form:
    //    instantiate object with specified port.
    //    ip_address default to current host.
    //
    // third form:   copy constructor

    socket_service_access_point_class(const ip_address_class & = ip_address_class(), const port_number_class & = port_number_class());
    socket_service_access_point_class(const port_number_class&, const ip_address_class & = ip_address_class());
    socket_service_access_point_class(const socket_service_access_point_class&);

    // ======================================
    //  assignment operators
    // ======================================
    //
    // first form:   standard object copy
    // second form:  update ipHost member of class
    // third form:   update port member of class

    socket_service_access_point_class& operator=(const socket_service_access_point_class&);
    socket_service_access_point_class& operator=(const ip_address_class&);
    socket_service_access_point_class& operator=(const port_number_class&);

    // ======================================
    //  other operators
    // ======================================
    //
    //  for equality testing

    int operator==(const socket_service_access_point_class&) const;
    int operator!=(const socket_service_access_point_class&) const;

    // ======================================
    //  inspectors
    // ======================================

    // get host, port objects respectively

    ip_address_class   ip_address(void)  const;
    port_number_class  port_object(void) const;

    // ======================================
    //  manipulators
    // ======================================
    //
    //  parsing and unparsing methods

    void extract(istream & = cin);
    void insert(ostream & = cout) const;

    // ======================================
    //  destructor
    // ======================================
    //
    virtual ~socket_service_access_point_class();

private:
    // ============================================================
    // member data
    // ============================================================

    //  for ensuring that supporting socket DLL is loaded and unloaded
    //    default (void) constructor =>  load default version of DLL.
    //  make this static since DLL only needs to be loaded once.
    //
    sockets_dll_init_class dll_init_object_;

    //   IP address, DNS name corresponding to specified host
    //
    ip_address_class   ip_address_;
    port_number_class  port_;
};

// =*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
// socket_service_access_point_class auxiliary methods
// =*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*

istream& operator>>(istream&, socket_service_access_point_class&);
ostream& operator<<(ostream&, const socket_service_access_point_class&);

#endif
