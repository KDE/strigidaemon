# - Try to find the low-level D-Bus library
# Once done this will define
#
#  DBUS_FOUND - system has D-Bus
#  DBUS_INCLUDE_DIRS - the D-Bus include directory
#  DBUS_LIBRARIES - the libraries needed to use D-Bus
#  DBUS_DBUSLAUNCH_EXECUTABLE - the path to the dbus-launch executable for testing

# Copyright (c) 2011, Raphael Kubo da Costa <kubito@gmail.com>
# modeled after FindDBus.cmake in kde-workspace:
# Copyright (c) 2008, Kevin Kofler, <kevin.kofler@chello.at>
# which was modeled after FindLibArt.cmake:
# Copyright (c) 2006, Alexander Neundorf, <neundorf@kde.org>
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# 1. Redistributions of source code must retain the copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. The name of the author may not be used to endorse or promote products
#    derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
# NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

find_package(PkgConfig)
pkg_check_modules(PC_DBUS dbus-1)

find_path(DBUS_INCLUDE_DIR NAMES dbus/dbus.h
  HINTS ${PC_DBUS_INCLUDEDIR}
        ${PC_DBUS_INCLUDE_DIRS}
  PATH_SUFFIXES dbus-1.0
)

find_path(DBUS_ARCH_INCLUDE_DIR dbus/dbus-arch-deps.h
  HINTS ${PC_DBUS_INCLUDEDIR}
        ${PC_DBUS_INCLUDE_DIRS}
  PATHS /usr/lib${LIB_SUFFIX}/include
        /usr/lib${LIB_SUFFIX}/dbus-1.0/include
        /usr/lib64/include
        /usr/lib64/dbus-1.0/include
        /usr/lib/include
        /usr/lib/dbus-1.0/include
)

set(DBUS_INCLUDE_DIRS ${DBUS_INCLUDE_DIR} ${DBUS_ARCH_INCLUDE_DIR})

find_library(DBUS_LIBRARIES NAMES dbus-1 dbus
  HINTS ${PC_DBUS_LIBDIR}
        ${PC_DBUS_LIBRARY_DIRS}
)

find_program(DBUS_DBUSLAUNCH_EXECUTABLE NAMES dbus-launch)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(DBUS DEFAULT_MSG DBUS_LIBRARIES DBUS_INCLUDE_DIRS DBUS_DBUSLAUNCH_EXECUTABLE)

mark_as_advanced(DBUS_LIBRARIES DBUS_INCLUDE_DIRS DBUS_DBUSLAUNCH_EXECUTABLE)
