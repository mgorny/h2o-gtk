AC_PREREQ([2.60])
AC_INIT([h2o-gtk+], [0.1],
		[https://bitbucket.org/mgorny/h2o-gtk/issues/],
		[h2o-gtk],
		[https://bitbucket.org/mgorny/h2o-gtk/])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([1.6 foreign dist-bzip2 subdir-objects])

PKG_CHECK_MODULES([LIBH2OXX], [libh2oxx >= 0.2])
PKG_CHECK_MODULES([GTKMM], [gtkmm-2.4 sigc++-2.0])

AC_PROG_CXX

AC_ARG_WITH([plotmm],
	[AS_HELP_STRING([--without-plotmm],
		[Disable plotmm plotting support])])

AS_IF([test x"$with_plotmm" != x"no"], [
	AS_IF([test x"$with_plotmm" = x"yes"], [
		PKG_CHECK_MODULES([PLOTMM], [plotmm])
		AC_DEFINE([HAVE_PLOTMM], [1], [define if you have plotmm])
	], [
		PKG_CHECK_MODULES([PLOTMM], [plotmm], [
			AC_DEFINE([HAVE_PLOTMM], [1], [define if you have plotmm])
		], [
			AC_MSG_WARN([plotmm not found, no plotting will be possible])
		])
	])
])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
