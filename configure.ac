#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([mednaffe], [0.9.0])
AC_CONFIG_SRCDIR([src/mednaffe.c])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([subdir-objects])

# Checks for programs.
AC_PROG_CC
AC_LANG_C

# Check for host
AC_CANONICAL_HOST

case $host_os in
  linux*)
         AM_CONDITIONAL([LINUX], [true])
         AM_CONDITIONAL([WINDOWS], [false])
         AM_CONDITIONAL([MAC], [false])
         ;;
  cygwin*|mingw*)
         AM_CONDITIONAL([LINUX], [false])
         AM_CONDITIONAL([WINDOWS], [true])
         AM_CONDITIONAL([MAC], [false])
	;;
  darwin*)
         AM_CONDITIONAL([MAC], [true])
         AM_CONDITIONAL([LINUX], [false])
         AM_CONDITIONAL([WINDOWS], [false])
         ;;
  *)
         AM_CONDITIONAL([LINUX], [false])
         AM_CONDITIONAL([WINDOWS], [false])
         AM_CONDITIONAL([MAC], [false])
         ;;
esac

case $host in
  *-mingw32.static)
	DEFINES="-DSTATIC_BUILD"
         ;;
  *)
        DEFINES="" 
         ;;
esac

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T

# Checks for library functions.
AC_CHECK_FUNCS([atexit memset mkdir realpath strrchr])

# Checks for libraries.
AC_CHECK_PROG(have_pkg_config, pkg-config, yes, no)
if test "x$have_pkg_config" = xno; then
  AC_MSG_ERROR('pkg-config' is required to compile Mednaffe)
else
  PKG_PROG_PKG_CONFIG
fi

PKG_CHECK_MODULES([GTK],[gtk+-3.0 >= 3.16 glib-2.0 >= 2.42], AC_MSG_NOTICE([Checking for GTK+ 3 development libraries]), AC_MSG_ERROR(Mednaffe needs GTK+ >= 3.16 development libraries))

AC_SUBST(DEFINES)

AC_CONFIG_FILES([Makefile share/Makefile src/Makefile])
AC_OUTPUT
