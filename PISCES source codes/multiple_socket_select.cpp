//--------------multiple_socket_select.cpp----------------------------------------------------------
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
//--------------------------------------------------------------------------------------------------

#include "multiple_socket_select.h"

#ifndef BOOL_H
#include "bool.h"     // TRUE, FALSE defined here, if needed
#endif

#include <sstream>   // stringstream

#ifndef SOCKET_ERROR_MESSAGE_H
#include "socket_error_message.h"
#endif

#ifndef STREAM_EXCEPTION_H
#include "stream_exception.h"   // stream exceptions
#endif

// ***********************************************************************
//  socket event flags class
// ***********************************************************************

// =*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
//  socket event flags class static data
// =*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*

//  basic event combinations, as static objects.
//  logical "or", "and" yield combinations of events

const socket_event_flags_class socket_event_flags_class::no_event_flags_(FALSE, FALSE, FALSE);
const socket_event_flags_class socket_event_flags_class::read_event_flag_(TRUE, FALSE, FALSE);
const socket_event_flags_class socket_event_flags_class::write_event_flag_(FALSE, TRUE, FALSE);
const socket_event_flags_class socket_event_flags_class::exception_event_flag_(FALSE, FALSE, TRUE);

// =*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
//  socket event flags class proper
// =*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*

//  ==============================
//   constructors
//  ==============================

// -------------------------------------------------------------------------
//   constructor, first form:   create event vector with specified event flags
// -------------------------------------------------------------------------
//
socket_event_flags_class::socket_event_flags_class(
    const unsigned char read_event_flag,
    const unsigned char write_event_flag,
    const unsigned char exception_event_flag
)
    : vector< unsigned char >(3)
{
    (*this)[enum_read_flag] = read_event_flag;
    (*this)[enum_write_flag] = write_event_flag;
    (*this)[enum_exception_flag] = exception_event_flag;
}

// -------------------------------------------------------------------------
//   constructor, second form:   copy constructor
// -------------------------------------------------------------------------
//
socket_event_flags_class::socket_event_flags_class(const socket_event_flags_class& source)
    : vector< unsigned char >(source)
{
}

//  ===============================
//   assignment operators
//  ==============================

// -------------------------------------------------------------------------
//  operator=() - standard object copy
// -------------------------------------------------------------------------
//
socket_event_flags_class& socket_event_flags_class::operator=(const socket_event_flags_class& source)
{
    (*this)[enum_read_flag] = source[enum_read_flag];
    (*this)[enum_write_flag] = source[enum_write_flag];
    (*this)[enum_exception_flag] = source[enum_exception_flag];
    return *this;
}

//  ===============================
//   inspectors
//  ==============================

// --------------------------------------------------------------------------
//  read_flag() -       check value of read event flag  (TRUE/FALSE)
//  write_flag() -      check value of write event flag  (TRUE/FALSE)
//  exception_flag() -  check value of exception event flag  (TRUE/FALSE)
// --------------------------------------------------------------------------
//
int socket_event_flags_class::read_flag(void) const
{
    return (*this)[enum_read_flag];
}

int socket_event_flags_class::write_flag(void) const
{
    return (*this)[enum_write_flag];
}

int socket_event_flags_class::exception_flag(void) const
{
    return (*this)[enum_exception_flag];
}

//  ===============================
//   manipulators
//  ==============================

// -------------------------------------------------------------------
//   clear_read_flag( ) - signal absence of read event
//   clear_write_flag( ) - signal absence of write event
//   clear_exception_flag( ) - signal absence of exception event
//   set_read_flag( ) - signal presence of read event
//   set_write_flag( ) - signal presence of write event
//   set_exception_flag( ) - signal presence of exception event
// -------------------------------------------------------------------
//
void socket_event_flags_class::clear_read_flag(void)
{
    (*this)[enum_read_flag] = FALSE;
}

void socket_event_flags_class::clear_write_flag(void)
{
    (*this)[enum_write_flag] = FALSE;
}

void socket_event_flags_class::clear_exception_flag(void)
{
    (*this)[enum_exception_flag] = FALSE;
}

void socket_event_flags_class::set_read_flag(void)
{
    (*this)[enum_read_flag] = TRUE;
}

void socket_event_flags_class::set_write_flag(void)
{
    (*this)[enum_write_flag] = TRUE;
}

void socket_event_flags_class::set_exception_flag(void)
{
    (*this)[enum_exception_flag] = TRUE;
}

//  ===============================
//   other operators
//  ==============================

// -------------------------------------------------------------------------
//  operator== () - object equality (based on member data equality
//  operator!= () - object inequality (based on member data inequality
// -------------------------------------------------------------------------
//
int
socket_event_flags_class::operator==(const socket_event_flags_class& other) const
{
    return (*this)[enum_read_flag] == other[enum_read_flag] &&
        (*this)[enum_write_flag] == other[enum_write_flag] &&
        (*this)[enum_exception_flag] == other[enum_exception_flag];
}

int
socket_event_flags_class::operator!=(const socket_event_flags_class& other) const
{
    return (*this)[enum_read_flag] != other[enum_read_flag] ||
        (*this)[enum_write_flag] != other[enum_write_flag] ||
        (*this)[enum_exception_flag] != other[enum_exception_flag];
}

//----------------------------------------------------------------------------
//  memberwise or, and, not -- based on combinations of individual flags
//----------------------------------------------------------------------------
//
socket_event_flags_class
socket_event_flags_class::operator&(const socket_event_flags_class& other) const
{
    return socket_event_flags_class(
        (*this)[enum_read_flag] && other[enum_read_flag],
        (*this)[enum_write_flag] && other[enum_write_flag],
        (*this)[enum_exception_flag] && other[enum_exception_flag]
    );
}

socket_event_flags_class
socket_event_flags_class::operator|(const socket_event_flags_class& other) const
{
    return socket_event_flags_class(
        (*this)[enum_read_flag] || other[enum_read_flag],
        (*this)[enum_write_flag] || other[enum_write_flag],
        (*this)[enum_exception_flag] || other[enum_exception_flag]
    );
}

socket_event_flags_class
socket_event_flags_class::operator~(void) const
{
    return socket_event_flags_class(
        !(*this)[enum_read_flag],
        !(*this)[enum_write_flag],
        !(*this)[enum_exception_flag]
    );
}

//----------------------------------------------------------------------------
//  memberwise or, and, with assignment
//----------------------------------------------------------------------------
//
socket_event_flags_class
socket_event_flags_class::operator&=(const socket_event_flags_class& other)
{
    (*this) = (*this) & other;
    return (*this);
}

socket_event_flags_class
socket_event_flags_class::operator|=(const socket_event_flags_class& other)
{
    (*this) = (*this) | other;
    return (*this);
}

//  ===============================
//   facilitators
//  ==============================

//-----------------------------------------------------------------------
//  insert() - dump contents of object to stream
//-----------------------------------------------------------------------
//
void socket_event_flags_class::insert(ostream& os) const
{
    // **** **** declare statics for error messages **** ****
    //
    static const string my_name = "socket_event_flags_class::insert";
    static const string no_write = "can't output dump contents of flags object";

    //  **** ****  dump the object's flag values  **** ****
    //
    if ((*this) == no_event_flags_)
        os << "<no flags set>";
    else
    {
        if ((*this)[enum_read_flag])   os << "read";
        if ((*this)[enum_write_flag])
        {
            if ((*this)[enum_read_flag])   os << ",";
            os << " write";
        }
        if ((*this)[enum_exception_flag])
        {
            if ((*this)[enum_read_flag] || (*this)[enum_write_flag])   os << ",";
            os << " exception";
        }
    }

    if (!os.good())
        throw outstream_io_error_exception(os, my_name, no_write);
}

//  ===============================
//   destructors
//  ==============================
//
socket_event_flags_class::~socket_event_flags_class(void)
{
}

// =*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
//  socket event flags auxiliary operators
// =*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*

// ------------------------------------------------------------------------
//  unparsing method
// ------------------------------------------------------------------------
//
ostream& operator<<(ostream& os, const socket_event_flags_class& source)
{
    source.insert(os);
    return os;
}

// ***********************************************************************
//  multiple socket select class
// ***********************************************************************

// =*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
//  multiple socket select class proper
// =*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*

// ================
//  constructors
// ================

// ----------------------------------------------------------------
//  constructor, first form:  create empty scan group
// ----------------------------------------------------------------
//
multiple_socket_select_class::multiple_socket_select_class(void)
{
}

// -------------------------------------------------------------------------------
//  constructor, second form:  create socket group with specified socket set,
//  initializing each socket to check for read events only
// -------------------------------------------------------------------------------
//
multiple_socket_select_class::multiple_socket_select_class(selectable_socket_class_set& socket_set)
{
    // *** step through set of sockets, adding to set of sockets to observe ***
    //
    for (selectable_socket_class_set::iterator p_next_socket = socket_set.begin(); p_next_socket != socket_set.end(); p_next_socket++)
    {
        selectable_socket_class* p_socket = const_cast<selectable_socket_class*>(&(*p_next_socket));
        monitored_socket_events_[&(*p_socket)] = socket_event_flags_class::read_event_flag_;
    }
}

// ----------------------------------------------------------------
//  constructor, third form:  copy constructor
// ----------------------------------------------------------------
//
multiple_socket_select_class::multiple_socket_select_class(const multiple_socket_select_class& source)
    : monitored_socket_events_(source.monitored_socket_events_)
{
}

// =======================
//  assignment operators
//========================

// ----------------------------------------------------------------
//  assignment operator:  standard object-to-object copy
// ----------------------------------------------------------------
//
multiple_socket_select_class& multiple_socket_select_class::operator= (const multiple_socket_select_class& source)
{
    monitored_socket_events_ = source.monitored_socket_events_;
    return *this;
}

//  ===============================
//   inspectors
//  ==============================

// ------------------------------------------------------------------------
//  get_scan_flags() - check for selection conditions for specified socket
// ------------------------------------------------------------------------
//
void multiple_socket_select_class::get_scan_flags(selectable_socket_class& sock, socket_event_flags_class& flags)
{
    //  *** create an iterator that, we hope, points to the desired name

    socket_event_set_type::iterator pp_socket_event_flags_pair = monitored_socket_events_.find(&sock);

    //  *** if item pointed to by iterator is empty, return "undefined", ***
    //  *** else, produce desired name                                   ***
    //
    if (pp_socket_event_flags_pair == monitored_socket_events_.end())
        flags = socket_event_flags_class::no_event_flags_;
    else
        flags = (*pp_socket_event_flags_pair).second;
}

// ------------------------------------------------------------------------------------------------
//  insert_multiple_socket_select_class_data() -
//      dump data on sockets being scanned,  together with monitored events for each socket
// ------------------------------------------------------------------------------------------------
//
void multiple_socket_select_class::insert_multiple_socket_select_class_data(ostream& os)
{
    os << "monitored sockets dump" << endl;

    socket_event_set_type::iterator pp_socket_event_set_pair;
    unsigned i = 0;

    for (pp_socket_event_set_pair = monitored_socket_events_.begin();
        pp_socket_event_set_pair != monitored_socket_events_.end();
        pp_socket_event_set_pair++, i++
        )
    {
        selectable_socket_class* p_sock = (*pp_socket_event_set_pair).first;
        selectable_socket_class sock = *p_sock;
        const socket_event_flags_class sock_events = (*pp_socket_event_set_pair).second;
        os << "socket " << i << ":" << endl;
        os << "*. socket attributes: " << sock << endl;
        os << "*. socket flag set:   " << sock_events << endl;
        os << endl;
    }
}

//  ===============================
//   manipulators
//  ==============================

// ----------------------------------------------------------------------------
//  update_scan() - set events to scan for socket
//
//  -.  if socket missing from set, and flags is non-null, add socket to set
//  -.  if socket already from set, and flags is null, drop socket from set
//  -.  if socket already in set, simply replace flags to check in scan map
// ----------------------------------------------------------------------------
//
void multiple_socket_select_class::update_scan(selectable_socket_class& sock, const socket_event_flags_class& flags)
{
    if (flags == socket_event_flags_class::no_event_flags_) monitored_socket_events_.erase(&sock);
    else monitored_socket_events_[&sock] = flags;
}

// ----------------------------------------------------------------
//  check_for_events, first form --
//      form below waits indefinitely for socket activity,
//      then returns set of active events in first parameter.
// ----------------------------------------------------------------
//
void multiple_socket_select_class::check_for_events(socket_event_set_type& events)
{
    select(events, 0);       // 0 parameter -> wait indefinitely for events
}

// ----------------------------------------------------------------
//  check_for_events, second form --
//      form below waits up to "seconds" seconds, "useconds" microseconds
//      for activity;  passing 0,0 -> check for activity now, then return.
//
//      if no activity, method throws timeout exception,
//      else method returns set of active events in first parameter.
// ----------------------------------------------------------------
//
void multiple_socket_select_class::check_for_events(socket_event_set_type& events, long seconds, long useconds)
{
    // struct timeval defined in socket header file:  sets time to wait
    //
    struct timeval timeout_value;
    timeout_value.tv_sec = seconds;
    timeout_value.tv_usec = useconds;

    select(events, &timeout_value);
}

//====================================================================
//  destructor
//====================================================================
//
multiple_socket_select_class::~multiple_socket_select_class(void)
{
}

// ===============================================================
//  private methods
// ===============================================================

// ----------------------------------------------------------------
//  select():
//    interfaces to timed, untimed versions of socket select() function
// ----------------------------------------------------------------
//
void multiple_socket_select_class::select(socket_event_set_type& events_detected, struct timeval* p_timeout_struct)
{
    //  **** set up messages for exception conditions ****
    //
    static const string my_name = "multiple_socket_select_class::select";
    static const string select_error = "call to select() failed";
    static const string timeout = "call to select() timed out";

    //  **** set up to scan sockets for specified kinds of activity ****

    //  select accepts three sets of flags that identify activities to monitor.
    //  events that can be monitored include
    //
    //  -.  read complete (read_fds_)
    //  -.  write complete (write_fds_)
    //  -.  error or out-of-band transmission complete (exception_fds_)
    //
    //  sense of flags:
    //  -. zeroed flag-- ignore activity of specified type on specified socket
    //  -. set flag--    check activity of specified type on specified socket

    //  ==== loop to set flags for requested kinds of activity to monitor ====

    //  ---- first zero all "check for activity" flags ----
    //
    fd_set read_fds_, write_fds_, exception_fds_;

    FD_ZERO(&read_fds_);
    FD_ZERO(&write_fds_);
    FD_ZERO(&exception_fds_);

    //  loop must also obtain the ID of the largest socket that participates
    //  in the select call (required by BSD Sockets; ignored by WinSock)
    //
    unsigned max_socket_id = 0;

    //  ---- step through set of sockets, adding to set of sockets to observe ----
    //
    socket_event_set_type::iterator pp_socket_event_set_pair;

    for (pp_socket_event_set_pair = monitored_socket_events_.begin();
        pp_socket_event_set_pair != monitored_socket_events_.end();
        pp_socket_event_set_pair++
        )
    {
        selectable_socket_class* p_sock = (*pp_socket_event_set_pair).first;
        selectable_socket_class sock = *p_sock;
        const socket_event_flags_class sock_events = (*pp_socket_event_set_pair).second;

        if (sock_events.read_flag() || sock_events.write_flag() || sock_events.exception_flag())
        {
            if (sock.socket_ > max_socket_id) max_socket_id = sock.socket_;

            if (sock_events.read_flag())      sock.set(read_fds_);
            if (sock_events.write_flag())     sock.set(write_fds_);
            if (sock_events.exception_flag()) sock.set(exception_fds_);
        }
    }

    // **** invoke select ****
    //
    int select_status = ::select(max_socket_id + 1, &read_fds_, &write_fds_, &exception_fds_, p_timeout_struct);

    // **** evaluate result of call ****
    //
    //  select() returns
    //     -1  in the event of an error
    //      0  if select() timed out with no events
    //      otherwise n, a positive integer--the number of sockets that became active

    // ==== first, check for select() failure ====
    //
    if (select_status < 0) throw socket_select_exception(my_name, select_error, socket_error_message_class("select").get_message());

    // ==== then, check for inactivity ===
    //
    if (select_status == 0)
    {
        //  generate message for caller, quit
        //
        stringstream p_message;

        if (p_timeout_struct)
        {
            p_message << "timer expired after ";

            if (p_timeout_struct->tv_sec > 0 || p_timeout_struct->tv_usec == 0)
            {
                p_message << p_timeout_struct->tv_sec << ((p_timeout_struct->tv_sec == 1) ? " second" : " seconds");
                if (p_timeout_struct->tv_usec > 0) p_message << ",";
                p_message << ",";
            }

            if (p_timeout_struct->tv_usec != 0) p_message << " micro" << ((p_timeout_struct->tv_usec == 1) ? "second" : "seconds");
        }
        else
            p_message << "?? timeout should not have been signaled??";

        throw socket_timeout_exception(my_name, timeout, p_message.str());
    }

    // ==== select() found socket activity--summarize ====

    //   reset "next unreturned, active socket" index for companion method,
    //   getNextSelectedSocket(), that scans for active sockets
    //

    events_detected.clear();   // clear map of events

    //  ---- step through set of sockets, recording events on observed sockets ----
    //
    unsigned events_noted_by_select_discovered_here = 0;

    for (pp_socket_event_set_pair = monitored_socket_events_.begin();
        pp_socket_event_set_pair != monitored_socket_events_.end();
        pp_socket_event_set_pair++
        )
    {
        //--------------------------------------------------------------------
        // WARNING:
        //    Leave the first variable below as a selectable_socket_class* type.
        //    Converting p_sock to a variable of type selectable_socket_class
        //      would kill any polymorphic properties of the socket's
        //      associated send(), sendto(), recv(), and recvfrom() methods.
        //--------------------------------------------------------------------

        selectable_socket_class* p_sock = (*pp_socket_event_set_pair).first;
        const socket_event_flags_class sock_events = (*pp_socket_event_set_pair).second;

        const unsigned char read_check_succeeded = sock_events.read_flag() && p_sock->is_set(read_fds_);
        const unsigned char write_check_succeeded = sock_events.write_flag() && p_sock->is_set(write_fds_);
        const unsigned char exceptionCheckSucceeded = sock_events.exception_flag() && p_sock->is_set(exception_fds_);

        if (read_check_succeeded || write_check_succeeded || exceptionCheckSucceeded)
        {
            events_detected[p_sock] = socket_event_flags_class(read_check_succeeded, write_check_succeeded, exceptionCheckSucceeded);

            if (read_check_succeeded)    events_noted_by_select_discovered_here++;
            if (write_check_succeeded)   events_noted_by_select_discovered_here++;
            if (exceptionCheckSucceeded) events_noted_by_select_discovered_here++;

            //  if value returned by select == count of events found, we're done
            //
            if (static_cast<unsigned>(select_status) <= events_noted_by_select_discovered_here) break;
        }
    }
}

// *===*====*===*====*===*====*===*====*===*====*===*====*===*====*===*====
// auxiliary operators
// *===*====*===*====*===*====*===*====*===*====*===*====*===*====*===*====

// -------------------------------------------------------------------------
// monitored socket set dump operator
// -------------------------------------------------------------------------
//
ostream& operator<<(ostream& os, multiple_socket_select_class& sockset)
{
    sockset.insert_multiple_socket_select_class_data(os);
    return os;
}

// **************************************************************************
// exception classes for socket event check routines  [boilerplate]
// **************************************************************************

// *===*====*===*====*===*====*===*====*===*====*===*====*===*====*===*====
// exceptions proper [boilerplate]
// *===*====*===*====*===*====*===*====*===*====*===*====*===*====*===*====

socket_event_check_exception::socket_event_check_exception(
    const string& name,
    const string& short_description,
    const string& detailed_description,
    const basic_exception& prior_exception
)
    : basic_exception(name, short_description, detailed_description, prior_exception) {}

socket_event_check_exception::socket_event_check_exception(const socket_event_check_exception& source)
    : basic_exception(source) {}

socket_event_check_exception::~socket_event_check_exception(void) {}

socket_select_exception::socket_select_exception(
    const string& name,
    const string& short_description,
    const string& detailed_description,
    const basic_exception& prior_exception
)
    : socket_event_check_exception(name, short_description, detailed_description, prior_exception) {}

socket_select_exception::socket_select_exception(const socket_select_exception& source)
    : socket_event_check_exception(source) {}

socket_select_exception::~socket_select_exception(void) {}

socket_timeout_exception::socket_timeout_exception(
    const string& name,
    const string& short_description,
    const string& detailed_description,
    const basic_exception& prior_exception
)
    : socket_event_check_exception(name, short_description, detailed_description, prior_exception) {}

socket_timeout_exception::socket_timeout_exception(const socket_timeout_exception& source)
    : socket_event_check_exception(source) {}

socket_timeout_exception::~socket_timeout_exception(void) {}

// *===*====*===*====*===*====*===*====*===*====*===*====*===*====*===*====
// auxiliary operators [boilerplate]
// *===*====*===*====*===*====*===*====*===*====*===*====*===*====*===*====

ostream& operator<<(ostream& os, const socket_event_check_exception& ex)
{
    ex.insert(os);
    return os;
}

ostream& operator<<(ostream& os, const socket_select_exception& ex)
{
    ex.insert(os);
    return os;
}

ostream& operator<<(ostream& os, const socket_timeout_exception& ex)
{
    ex.insert(os);
    return os;
}