C++ with Components

(c) Michael Florian Hava

Released under the Boost Software License - Version 1.0, see "LICENSE_1_0.txt" for details.

Introduction
============

C++ with Components (from here on simply called CWC) is a library- and tool-based extension to ISO-C++ that introduces support for cross-platform, cross-compiler component oriented software development. Key aspect of CWC is the integration of component orientation into the semantics and syntax of regular C++.
CWC introduces a stable binary representation of components and related types. Based on this representation it's possible to deploy and use components in precompiled form. Wrapper classes are used to integrate components into the syntax and semantics of C++ and allow them to be used exactly like traditional objects. CWC was originally released as a master thesis in 2013 and has been in active development ever since.

Requirements
============ 
CWC uses CMake as its build system and requires a conformant C++11 implementation. Some workarounds are employed to support partially conformat implementations (e.g. MSVC 2013 and GNU without Std.Regex) - these workarounds introduce additional dependencies! If no workarounds are necessary, all static libraries of CWC depend only on the C++11 standard library. The included tools aswell as the unit tests always depend on Boost libraries.