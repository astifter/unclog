#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
# Don't forget to update PREVIOUS_VERSION at the end of the file.
AC_INIT([unclog], [0.2], [unclog@astifter.com])
AC_CONFIG_SRCDIR([src])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
#AC_PROG_RANLIB # Obsoleted by LT_INIT later on.
AM_PROG_AR # Used by LT_INIT later on.

# Checks for libraries.
AC_CHECK_LIB([pthread], [pthread_rwlock_wrlock])
AC_CHECK_LIB([cunit], [CU_initialize_registry])

# Checks for header files.
AC_CHECK_HEADERS([limits.h stddef.h stdint.h stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T
AC_TYPE_UINT32_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([clock_gettime memset strchr strdup])

# Configure directories.
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([build-aux])

# Initialize automake and libtool.
AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects])
LT_INIT([shared disable-static])

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 src/examples/Makefile
                 src/inih/Makefile
                 src/tests/Makefile
                 src/unclog/Makefile])
AC_SUBST(PREVIOUS_VERSION, '0.1')
AC_OUTPUT
