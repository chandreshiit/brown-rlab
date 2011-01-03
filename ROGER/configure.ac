AC_PREREQ(2.59)

AC_INIT([ROGER], [1.6], [dang@cs.brown.edu])

AM_INIT_AUTOMAKE([-Wall -Werror foreign])

AC_PROG_CXX
AC_PROG_RANLIB
AC_CHECK_FUNCS([floor])
AC_CHECK_HEADERS([string.h])
AC_C_CONST
AC_HEADER_STDBOOL
AC_HEADER_STDC


AC_CHECK_LIB([SOGP],[main],,[echo SOGP NOT DETECTED.  Please set LDFLAGS and CPPFLAGS to point to it, or install it])
AC_CHECK_LIB([newmat],[main],,[echo newmat NOT DETECTED.  Please set LDFLAGS and CPPFLAGS to point to it, or install it])

AC_CONFIG_FILES([Makefile lib/Makefile tests/Makefile utils/Makefile])
AC_OUTPUT

