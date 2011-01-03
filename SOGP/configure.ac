AC_PREREQ(2.59)

AC_INIT([SOGP], [2.1], [dang@cs.brown.edu])

AM_INIT_AUTOMAKE([-Wall -Werror foreign])

AC_PROG_CXX
AC_PROG_RANLIB
AC_CHECK_FUNCS([gettimeofday])
AC_CHECK_FUNCS([pow])
AC_CHECK_FUNCS([sqrt])
AC_CHECK_HEADERS([string.h]) 
AC_CHECK_HEADERS([sys/time.h])
AC_C_CONST
AC_HEADER_STDBOOL
AC_HEADER_STDC
AC_HEADER_TIME

AC_CHECK_LIB([newmat],[main],,[echo NEWMAT NOT DETECTED.  Please set LDFLAGS and CPPFLAGS to point to it, or install it])

AC_CONFIG_FILES([Makefile lib/Makefile tests/Makefile utils/Makefile])
AC_OUTPUT

