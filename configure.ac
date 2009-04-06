#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([FULL-PACKAGE-NAME], [VERSION], [BUG-REPORT-ADDRESS])

AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADERS([config.h])

AC_CONFIG_AUX_DIR([.])

AM_INIT_AUTOMAKE(lark, 1.0)

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LIBTOOL

AC_LANG([C++])
AX_BOOST_BASE([1.33.1])
AX_BOOST_SYSTEM
AX_BOOST_FILESYSTEM
AX_BOOST_THREAD

# Checks for libraries.
AC_CHECK_LIB([m], [main])
AC_CHECK_LIB([z], [main])

PKG_CHECK_MODULES(SQLITE3, [sqlite3])
PKG_CHECK_MODULES(THRIFT, [thrift])
PKG_CHECK_MODULES(GSTREAMER, [gstreamer-base-0.10 gstreamer-0.10])
PKG_CHECK_MODULES(UUID, [uuid])
PKG_CHECK_MODULES(TAG, [taglib])

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_SIZE_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T

AC_CONFIG_FILES([Makefile cpp/Makefile])

AC_OUTPUT
