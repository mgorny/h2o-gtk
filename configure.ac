AC_PREREQ([2.60])
AC_INIT([h2o-gtk+], [0])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([1.6 foreign dist-bzip2])

PKG_CHECK_MODULES([LIBH2OXX], [libh2oxx])
PKG_CHECK_MODULES([GTKMM], [gtkmm-2.4 plotmm sigc++-2.0])

AC_PROG_CXX

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
