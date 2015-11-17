Node++ [![Build Status](https://travis-ci.org/beached/nodepp.svg?branch=master)](https://travis-ci.org/beached/nodepp)
====== <a href="https://scan.coverity.com/projects/beached-nodepp">
  <img alt="Coverity Scan Build Status"
       src="https://scan.coverity.com/projects/7045/badge.svg"/>
</a>

A modern C++ Event I/O library with a high level of abstraction.

Requires: CMake, Boost

It has been tested on g++, clang and msvc.  Requires a C++ 11 compiler.

[example_net_server.cpp](example_net_server.cpp) - An example async server that shows lisening/sending/receiving data

[example_net_client.cpp](example_net_client.cpp) - An example async client that shows connecting/sending/receiving data

[example_web_service.cpp](example_web_service.cpp) - A partial example of importing/exporting data to/from json and serving web documents

[example_web_client.cpp](example_web_client.cpp) - An example of a http client(WIP)

[example_event_emitter.cpp](example_event_emitter.cpp) - An example of adding callbacks/emitting to events


