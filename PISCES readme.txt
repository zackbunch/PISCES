Author:  Phil Pfeiffer
Last updated:  April 2020

This directory contains the PISCES C++ socket API library, together with sample C++ 
programs that demonstrate library functionality.  These programs have most recently
been compiled under Visual Studio 2019 under Winsock 1.1.

Earlier versions were compiled under GNU C++ v2.8.1 under IRIX 6.5 IP27.  As of this
writing, the Unix-based versions of these codes have not been retested.

To set the target platform, follow directions in the file platform.h.

Specific programs include the following:

self check.cpp:           return network characteristics of the local host.
IP check.cpp:             return network characteristics of a host with a specified IP address.
IP address sequence.cpp:  return DNS names of hosts with specified IP addresses
                          for a user-specified start and end address.
DNS check.cpp:            return network characteristics of the specified host.

udp fixed message length client.cpp, udp fixed message length server.cpp:
      exchange fixed-length messages between a client-server pair. 
      the server receives messages from the client and returns the reversed messages to the client.

tcp variable message length client.cpp, tcp variable message length server.cpp:
      exchange variable-length messages between a client-server pair. 
      the server receives messages from the client and returns the reversed messsages to the client.

tcp fixed message length client.cpp, tcp fixed message length server.cpp:
      exchange fixed-length messages between a client-server pair. 
      the server receives messages from the client and returns the reversed messsages to the client.

tcp fixed message length client.cpp, tcp fixed message length n-way server.cpp:
      exchange fixed-length messages between multiple clients and a supporting server.
      the server receives messages from the client and returns the reversed messages to the client.

-----

The following is a bottom-up characterization of the PISCES class hierarchy, adapted from
the PISCES sample program .sln files.  In some cases, numbers have been added to ensure 
that Visual Studio 2019, which orders source directories ("filters") in lexicographical
order, orders the hierarchy's elements as they're structured.

00. base configuration\
      bool.h
      platform.h
      std.h
      types.h

01. exception management classes\
      exception.cpp
      exception.h
      stream_exception.cpp
      stream_exception.h

02. utility classes
      console dialogue\
        query.cpp
        query.h
      strstream handle class\
        string_stream_handle.cpp
        string_stream_handle.h
      time management\
        daytime.cpp
        daytime.h
        elapsed_time.cpp
        elapsed_time.h
      command line processing\
        getopt.cpp
        getopt.h
      interval management template\
        interval.tpl
        interval.h
      message buffering\
        message_buffer.cpp
        message_buffer.h

03. socket API adapter layer\
      DLL management\
        dllinit.cpp
        dllinit.h
      socket error message handling\
        socket_error_message.cpp
        socket_error_message.h
      socket_apis.cpp
      socket_apis.h

04. service access point management\
      DNS exception classes\
        dns_exception.cpp
        dns_exception.h
      ip address object\
        ip_address.cpp
        ip_address.h
      port number object\
        port_number.cpp
        port_number.h
      service access point object\
        service_access_point.cpp
        service_access_point.h
      DNS name object\
        dns_name.cpp
        dns_name.h
      host data (DNS+IP) object\
        host_data.cpp
        host_data.h

05. vanilla TCP/UDP sockets classes\
      basic socket class\
        socket.cpp
        socket.h
      socket functionality mixins\
        client-defining classes\
          socket_client.cpp
          socket_client.h
        server-defining classes\
          socket_server.cpp
          socket_server.h
        tcp-defining classes\
          socket_tcp.cpp
          socket_tcp.h
        udp-defining classes\
          socket_udp.cpp
          socket_udp.h
      top-level socket classes\
          tcp classes\
            tcp client socket class\
              tcp_client.cpp
              tcp_client.h
            tcp server socket class\
              tcp_server.cpp
              tcp_server.h
            tcp rollover socket class\
              tcp_rollover.cpp
              tcp_rollover.h
          udp classes\
            udp client (send-only) socket\
              udp_client.cpp
              udp_client.h
            udp peer (send-recv) socket\
              udp_peer.cpp
              udp_server.h
            udp server (recv-only) socket\
              udp_server.cpp
              udp_peer.h

06. service access point console dialogue\
      query object + sap queries\
        query_access_point.cpp
        query_access_point.h
      query object + client sap setup\
        query_client.cpp
        query_client.h
      query object + server sap setup\
        query_server.cpp
        query_server.h

07. format-aware message exchange\
      tcp fixed-length message exchange\
        tcp_fixed_message_length_client.cpp
        tcp_fixed_message_length_client.h
        tcp_fixed_message_length_rollover.cpp
        tcp_fixed_message_length_rollover.h
        tcp_fixed_message_length_server.cpp
        tcp_fixed_message_length_server.h
      tcp variable-length message exchange\
        tcp_variable_message_length_client.cpp
        tcp_variable_message_length_client.h
        tcp_variable_message_length_rollover.cpp
        tcp_variable_message_length_rolloverr.h
        tcp_variable_message_length_server.cpp
        tcp_variable_message_length_server.h
      udp fixed-length message exchange\
        udp_fixed_message_length_client.cpp
        udp_fixed_message_length_client.h
        udp_fixed_message_length_server.cpp
        udp_fixed_message_length_server.h

08. select()-based socket processing\
      event-selectable socket class\
        08.1 selectable sockets base classes\
          socket_select.cpp
          socket_select.h
        08.2 selectable vanilla TCP/UDP socket classes\
          selectable tcp client socket\
            selectable_tcp_client.cpp
            selectable_tcp_client.h
          selectable tcp rollover socket\
            selectable_tcp_rollover.cpp
            selectable_tcp_rollover.h
          selectable tcp server socket\
            selectable_tcp_server.cpp
            selectable_tcp_server.h
          selectable udp client socket\
            selectable_udp_client.cpp
            selectable_udp_client.h
          selectable udp peer socket\
            selectable_udp_peer.cpp
            selectable_udp_peer.h
          selectable udp server socket\
            selectable_udp_server.cpp
            selectable_udp_server.h
        08.3 selectable format-aware socket classes\
          tcp_fixed_message_length_selectable_rollover.cpp
          tcp_fixed_message_length_selectable_rollover.h
          tcp_fixed_message_length_selectable_server.cpp
          tcp_fixed_message_length_selectable_server.h
      select sockets, based on specified events\
        multiple_socket_select.cpp
        multiple_socket_select.h
