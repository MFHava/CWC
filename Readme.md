C++ with Components

(c) Michael Florian Hava

Released under the Boost Software License - Version 1.0, see "LICENSE_1_0.txt" for details.

Introduction
============

C++ with Components (from here on simply called CWC) is a library- and tool-based extension to ISO-C++ that introduces support for cross-platform, cross-compiler component oriented software development. Key aspect of CWC is the integration of component orientation into the semantics and syntax of regular C++.
CWC introduces a stable binary representation of components and related types. Based on this representation it's possible to deploy and use components in precompiled form. CWC was originally released as a master thesis in 2013 and has been in active development ever since.

Requirements
============ 
CWC uses CMake as its build system and requires a conformant C++14 implementation and (fairly) complete C++14 standard library. Whereas CWC and projects using it have no external dependencies, tools and tests depend heavily on several (precompiled) Boost libraries. The documentation is based on Doxygen.