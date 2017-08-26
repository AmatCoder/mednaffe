#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([mednaffe], [0.9.0], [BUG-REPORT-ADDRESS])
AM_INIT_AUTOMAKE([foreign -Wall -Werror])
AC_CONFIG_SRCDIR([src/common.h])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AC_LANG_C

CFLAGS="-s -O2 -std=c99 -Wall -Wl,-export-dynamic $CFLAGS"

# Checks for libraries.
AC_ARG_ENABLE(gtk3, [AC_HELP_STRING([--enable-gtk3], [Compile against GTK 3.x explicitly])], [enable_gtk3=$enableval], [enable_gtk3="no"])

AC_CHECK_PROG(have_pkg_config, pkg-config, yes, no)
if test "x$have_pkg_config" = xno; then 
  AC_MSG_ERROR(pkg-config is required to compile Mednaffe)
else
  PKG_PROG_PKG_CONFIG
fi

if test "x$enable_gtk3" = xyes; then
  PKG_CHECK_MODULES([GTK],[gtk+-3.0 >= 3.4 glib-2.0 >= 2.32 gthread-2.0], AC_MSG_NOTICE([GTK 3.x selected explicitly]), AC_MSG_ERROR(Mednaffe needs GTK+ >= 3.4 development libraries))
  GTK_DEFINES="-DGTK3_ENABLED"
  AC_DEFINE([GTK3_ENABLED],[1],[Define to 1 if you have GTK 3])
else
  PKG_CHECK_MODULES([GTK],[gtk+-2.0 >= 2.24 glib-2.0 >= 2.32 gthread-2.0], AC_MSG_NOTICE([selecting GTK 2.x automatically]), AC_MSG_ERROR(Mednaffe needs GTK+ >= 2.24 development libraries))
  GTK_DEFINES="-DGTK2_ENABLED"
  AC_DEFINE([GTK2_ENABLED],[1],[Define to 1 if you have GTK 2])
fi

AC_SUBST(GTK_DEFINES)

AC_CONFIG_FILES([Makefile share/Makefile src/Makefile])
AC_OUTPUT
