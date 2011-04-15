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
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

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
