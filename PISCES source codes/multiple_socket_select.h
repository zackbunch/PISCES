//--------------multiple_socket_select.h--------------------------------------------------------
//
//  Author:  Phil Pfeiffer
//  Date:    Fall 1999
//  Last modified:  April 2020
//
//   Monitor activity on multiple selectable sockets, using the BSD/WinSock select() method.
//   Methods coded here
//      -.  poll sockets for activity, using select()
//      -.  identify all active sockets on return from select(), together with types of activity
//   The poll for activity may be for an indefinite or a limited period of time,
//   subject to a user timeout parameter.
//-----------------------------------------------------------------------------------------------

#ifndef MULTIPLE_SOCKET_SELECT_H
#define MULTIPLE_SOCKET_SELECT_H

#include <string>
#include <vector>
#include <map>
#include <set>
#include <iostream>

#ifndef PLATFORM_H
#include "platform.h"
#endif

#ifndef STD_H
#include "std.h"
#endif

#ifndef BOOL_H
#include "bool.h"
#endif

#ifndef SOCKET_APIS_H
#include "socket_apis.h"      // defines fd_set data type
#endif

#ifndef SOCKET_H
#include "socket.h"          // defines base class
#endif

#ifndef SOCKET_SELECT_H
#include "socket_select.h"
#endif

// ***********************************************************************
//  socket event flags class
// ***********************************************************************
//
// a socket event can be one of three kinds of occurrences:
// -.  a read
// -.  a write
// -.  an exception
//
// this class defines a vector with flags corresponding to the three events.
// Private inheritance (i.e., a "wasA" relationship) is used to disable
// access to the underlying vector by other classes

// =*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
//  socket event flags class proper
// =*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*

class socket_event_flags_class : private vector< unsigned char >
{
public:
    //  ==============================
    //   static data members
    //  ==============================
    //
    //  basic event combinations, as static objects.
    //  logical "or", "and" yield combinations of events

    static const socket_event_flags_class no_event_flags_;
    static const socket_event_flags_class read_event_flag_;
    static const socket_event_flags_class write_event_flag_;
    static const socket_event_flags_class exception_event_flag_;

    //  ==============================
    //   constructors
    //  ==============================
    //
    //   first form:   create event vector with specified event flags
    //   second form:  copy constructor

    socket_event_flags_class(
        const unsigned char read_event_flag = FALSE,
        const unsigned char write_event_flag = FALSE,
        const unsigned char exception_event_flag = FALSE
    );
    socket_event_flags_class(const socket_event_flags_class&);

    //  ===============================
    //   assignment operators
    //  ==============================
    //
    //  standard object copy

    socket_event_flags_class& operator=(const socket_event_flags_class&);

    //  ===============================
    //   inspectors
    //  ==============================

    //  check for presence of specific socket events
    //
    int read_flag(void) const;
    int write_flag(void) const;
    int exception_flag(void) const;

    //  ===============================
    //   manipulators
    //  ==============================

    //  signal presence, or absence, of specific socket events
    //
    void clear_read_flag();
    void clear_write_flag();
    void clear_exception_flag();
    void set_read_flag();
    void set_write_flag();
    void set_exception_flag();

    //  ===============================
    //   other operators
    //  ==============================
    //
    //  equality and inequality--based on identical settings of flags

    int operator==(const socket_event_flags_class&) const;
    int operator!=(const socket_event_flags_class&) const;

    //  memberwise or, and, not -- based on combinations of individual flags

    socket_event_flags_class operator&(const socket_event_flags_class&) const;
    socket_event_flags_class operator|(const socket_event_flags_class&) const;
    socket_event_flags_class operator~(void) const;

    //  memberwise or, and, with assignment

    socket_event_flags_class operator&=(const socket_event_flags_class&);
    socket_event_flags_class operator|=(const socket_event_flags_class&);

    //  ===============================
    //   facilitators
    //  ==============================
    //
    //  display an event flag object on an ostream

    void insert(ostream&) const;

    //  ===============================
    //   destructors
    //  ==============================
    //
    virtual ~socket_event_flags_class(void);

private:
    // ================================
    //   enumerated constants
    // ================================

    //  positions in event flags vector for R/W/X flags
    //
    enum { enum_read_flag = 0, enum_write_flag = 1, enum_exception_flag = 2 };
};

// =*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
//  socket event flags auxiliary operators
// =*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*

//  unparsing method

ostream& operator<<(ostream&, const socket_event_flags_class&);

// ***********************************************************************
//  multiple socket select class
// ***********************************************************************

// =*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
//  multiple socket select class proper
// =*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*

//----------------------------------------------------------------------------
// WARNING:
//    The event-tracking association socket_event_set_type MUST have the pointer
//      type selectable_socket_class* as its domain.
//    If selectable_socket_class were to be used as the domain instead, sockets
//      passed to the class would be converted to type selectable_socket_class
//      before being added to map<>, using the selectable_socket_class copy
//      constructor.
//    This conversion, in turn, would kill any polymorphic properties of
//      read_event(), write_event(), and exception_event() methods
//----------------------------------------------------------------------------

typedef map< selectable_socket_class*, socket_event_flags_class > socket_event_set_type;
typedef set< selectable_socket_class > selectable_socket_class_set;

class multiple_socket_select_class
{
public:
    //  ==============================
    //   constructors
    //  ==============================
    //
    //   first form:   create empty socket group.
    //   second form:  create socket group with specified set of sockets.
    //                 default mode for each socket:  check for read only
    //   third form:   copy constructor.

    multiple_socket_select_class(void);
    multiple_socket_select_class(selectable_socket_class_set&);
    multiple_socket_select_class(const multiple_socket_select_class&);

    //  ===============================
    //   assignment operators
    //  ==============================
    //
    //   standard object-to-object copy

    multiple_socket_select_class& operator= (const multiple_socket_select_class&);

    //  ===============================
    //   inspectors
    //  ==============================

    //  check for selection conditions on specified socket.
    //  if socket not in selection set, return "no selection" flag set
    //
    void get_scan_flags(selectable_socket_class&, socket_event_flags_class&);

    //   dump scan flags for each socket in scan
    //
    void insert_multiple_socket_select_class_data(ostream&);

    //  ===============================
    //   manipulators
    //  ==============================

    //  update set of sockets being scanned
    //  -.  if events to check argument is "noEventsToCheck", drop socket from scan
    //  -.  otherwise
    //      -.  if socket not yet in set, add socket to set of sockets being scanned
    //      -.  associate socket with specified events
    //
    void update_scan(
        selectable_socket_class&,
        const socket_event_flags_class & = socket_event_flags_class::read_event_flag_
    );

    // poll for socket activity, returning data on active sockets
    //
    // -.  first form:   wait indefinitely for activity
    // -.  second form:  wait up to sec seconds, usec microseconds
    //                   (0,0 => check for activity, and return immediately)
    //
    void check_for_events(socket_event_set_type&);
    void check_for_events(socket_event_set_type&, long sec, long usec = 0);

    //=====================================
    //  destructor
    //=====================================
    //
    virtual ~multiple_socket_select_class(void);

private:
    //============================================
    //  member functions
    //============================================

    //  tracks sockets being monitored, and events to check
    //
    socket_event_set_type monitored_socket_events_;

    //  select():
    //    interfaces to timed, untimed versions of socket select() function
    //
    void select(socket_event_set_type&, struct timeval*);
};

// *===*====*===*====*===*====*===*====*===*====*===*====*===*====*===*====
// auxiliary operators
// *===*====*===*====*===*====*===*====*===*====*===*====*===*====*===*====

// === monitored socket set dump operator ====
//
ostream& operator<<(ostream&, multiple_socket_select_class&);

// **************************************************************************
// exception classes for socket monitoring routines
// **************************************************************************
//
// base class, socket_event_check_exception, derived from basic_exception

class socket_event_check_exception : public basic_exception
{
public:
    socket_event_check_exception(
        const string & = "", const string & = "", const string & = "",
        const basic_exception & = basic_exception()
    );
    socket_event_check_exception(const socket_event_check_exception&);
    virtual ~socket_event_check_exception(void);
};

// derived (specific) exceptions:
//
// -.  socket_select_exception:  call to select() failed
// -.  socket_timeout_exception:     check for socket activity timed out

class socket_select_exception : public socket_event_check_exception
{
public:
    socket_select_exception(
        const string & = "", const string & = "", const string & = "",
        const basic_exception & = basic_exception()
    );
    socket_select_exception(const socket_select_exception&);
    virtual ~socket_select_exception(void);
};

class socket_timeout_exception : public socket_event_check_exception
{
public:
    socket_timeout_exception(
        const string & = "", const string & = "", const string & = "",
        const basic_exception & = basic_exception()
    );
    socket_timeout_exception(const socket_timeout_exception&);
    virtual ~socket_timeout_exception(void);
};

// *===*====*===*====*===*====*===*====*===*====*===*====*===*====*===*====
// auxiliary operators
// *===*====*===*====*===*====*===*====*===*====*===*====*===*====*===*====

ostream& operator<<(ostream&, const socket_event_check_exception&);
ostream& operator<<(ostream&, const socket_select_exception&);
ostream& operator<<(ostream&, const socket_timeout_exception&);

#endif
