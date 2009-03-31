AC_PREREQ(2.57)

AC_INIT([chai3d], [1.62], [support@chai3d.org])

# libtool version: current:revision:age
#
# If the library source code has changed at all since the last update, then
# increment revision (`c:r:a' becomes `c:r+1:a').
#
# If any interfaces have been added, removed, or changed since the last update,
# increment current, and set revision to 0.
#
# If any interfaces have been added since the last public release, then
# increment age.
#
# If any interfaces have been removed since the last public release, then set
# age to 0.
SO_VERSION=1:0:0

AM_CONFIG_HEADER([config.h])

AM_INIT_AUTOMAKE

AC_ENABLE_STATIC(no)
AC_ENABLE_SHARED(yes)
AC_SUBST(SO_VERSION)

# Checks for programs.
AC_PROG_CXX
AM_PROG_LIBTOOL
AM_PROG_CC_C_O
AC_CHECK_PROG([DOXYGEN], [doxygen], [doc], [])
AC_SUBST(DOXYGEN)

# Checks for libraries.
AC_CHECK_LIB([gl], [glBegin])
AC_CHECK_FUNC([log], [], [AC_CHECK_LIB([m],[log])])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T

AC_CONFIG_FILES([
	Makefile
	../src/Makefile
])
AC_OUTPUT()
