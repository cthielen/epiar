PACKAGE='epiar'
VERSION='0.6.0'

dnl Init
dnl AC_INIT(README)
AC_INIT(epiar, 0.6.0)
AC_CONFIG_SRCDIR(README)
AC_PREREQ(2.50)

dnl Detect the canonical host and target build environment
AC_CANONICAL_HOST
AC_CANONICAL_TARGET

dnl AM_INIT_AUTOMAKE($PACKAGE, $VERSION)
AM_INIT_AUTOMAKE
AM_CONFIG_HEADER(config.h)

AM_INIT_AUTOMAKE([subdir-objects])

AC_PROG_CPP
AC_PROG_CXX
AC_PROG_CC
AC_PROG_RANLIB
AC_LANG(C++)

dnl Check for a C++ compiler
if test "$ac_cv_prog_cxx_g" == no
then
	AC_MSG_ERROR([Cannot find a working C++ compiler])
fi

dnl Set PKG_CONFIG if necessary
if test "$PKG_CONFIG" == ""
then
	PKG_CONFIG=`which pkg-config`
fi

dnl Check for Freetype
case "$target" in
	*-*-linux* | *-*-cygwin* | *-*-mingw32* | *-*-freebsd* | *-apple-darwin*)
	PKG_CHECK_MODULES([freetype2], [freetype2 >= 9.22])
	CFLAGS="$CFLAGS $freetype2_CFLAGS"
	LIBS="$LIBS $freetype2_LIBS"
esac

dnl Some additional flags for Freetype
dnl case "$target" in *-*-linux*)
dnl	CFLAGS="$CFLAGS -I/usr/include/freetype2"
dnl esac
dnl case "$target" in *-*-freebsd*)
dnl	CFLAGS="$CFLAGS -I/usr/local/include/freetype2"
dnl esac
dnl case "$target" in *-apple-darwin*)
dnl	CFLAGS="$CFLAGS -I/opt/local/include/ -I/opt/local/include/freetype"
dnl esac

dnl Check for PhysicsFS (physfs)
LIBS="$LIBS -lphysfs"
CFLAGS="$CFLAGS -DUSE_PHYSICSFS"
dnl case "$target" in *-*-linux*)
dnl 	CFLAGS="$CFLAGS -I/usr/include/"
dnl esac
dnl case "$target" in *-*-freebsd*)
dnl 	CFLAGS="$CFLAGS -I/usr/local/include/"
dnl esac
dnl case "$target" in *-apple-darwin*)
dnl 	CFLAGS="$CFLAGS -I/opt/local/include/"
dnl esac

AC_CHECK_HEADER(physfs.h, have_physfs_hdr=yes)
AC_CHECK_LIB(physfs, PHYSFS_init, have_physfs_lib=yes)

if test x$have_physfs_hdr = xyes -a x$have_physfs_lib = xyes; then
	enable_physfs="yes"
else
dnl if test "$ac_cv_prog_cxx_g" == no
dnl then
	AC_MSG_ERROR([Could not find PhysicsFS])
dnl fi
	enable_physfs="no"
fi


dnl Check for libxml2
case "$target" in
	*-*-linux* | *-*-cygwin* | *-*-mingw32* | *-*-freebsd*)
	PKG_CHECK_MODULES([libxml2], [libxml-2.0 >= 2.7.3])
	CFLAGS="$CFLAGS $libxml2_CFLAGS"
	LIBS="$LIBS $libxml2_LIBS"
esac
case "$target" in *-apple-darwin*)
	CFLAGS="$CFLAGS `xml2-config --cflags`"
	LIBS="$LIBS `xml2-config --libs`"
esac

dnl Check for SDL2
SDL_VERSION=2.0.3

case "$target" in
	*-*-linux* | *-*-cygwin* | *-*-mingw32* | *-*-freebsd* | *-apple-darwin*)
		AM_PATH_SDL2($SDL_VERSION, :, AC_MSG_ERROR([*** SDL version $SDL_VERSION not found!]))
		CFLAGS="$CFLAGS $SDL2_CFLAGS -DCOMP_GCC"
		LIBS="$LIBS $SDL2_LIBS"
esac

dnl Set up OpenGL for OS X
dnl case "$target" in
dnl 	*-apple-darwin*)
dnl 	CFLAGS="$CFLAGS -I/System/Library/Frameworks/OpenGL.framework/Headers"
dnl 	LIBS="$LIBS -framework OpenGL"
dnl esac

dnl Check for SDL_image
case "$target" in
	*-*-linux* | *-*-cygwin* | *-*-mingw32* | *-*-freebsd* | *-apple-darwin*)
	PKG_CHECK_MODULES([SDL2_image], [SDL2_image >= 2.0.0])
	CFLAGS="$CFLAGS $SDL2_image_CFLAGS"
	LIBS="$LIBS $SDL2_image_LIBS"
esac

dnl Check for SDL_mixer
case "$target" in
	*-*-linux* | *-*-cygwin* | *-*-mingw32* | *-*-freebsd* | *-apple-darwin*)
	PKG_CHECK_MODULES([SDL2_mixer], [SDL2_mixer >= 2.0.0])
	CFLAGS="$CFLAGS $SDL2_mixer_CFLAGS"
	LIBS="$LIBS $SDL2_mixer_LIBS"
esac

dnl Check for SDL_ttf
case "$target" in
	*-*-linux* | *-*-cygwin* | *-*-mingw32* | *-*-freebsd* | *-apple-darwin*)
	PKG_CHECK_MODULES([SDL2_ttf], [SDL2_ttf >= 2.0.12])
	CFLAGS="$CFLAGS $SDL2_ttf_CFLAGS"
	LIBS="$LIBS $SDL2_ttf_LIBS"
esac

dnl Set MS Windows specific options - For afont and ntohl (Win32 provides this through Winsock)
case "$target" in
	*-*-cygwin* | *-*-mingw32*)
		LIBS="$LIBS -lwsock32 -lz"
		;;
esac

dnl Set MinGW-specific options
case "$target" in
	*-*-mingw32*)
		LIBS="$LIBS -lopengl32"
		;;
esac

dnl Set Unix specific options
case "$target" in
	*-*-linux* | *-*-freebsd*)
		CFLAGS="$CFLAGS -DUNIX -I/usr/include/GL"
		LIBS="$LIBS -lGL -lGLU -lz"
		;;
esac

dnl Set OS X specific options
case "$target" in
	*-apple-darwin*)
		CFLAGS="$CFLAGS -DDARWIN"
		LIBS="$LIBS"
		;;
esac

CFLAGS="$CFLAGS -Wall"
CXXFLAGS="$CFLAGS"

AC_OUTPUT([Makefile])
