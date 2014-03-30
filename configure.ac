PACKAGE='epiar'
VERSION='0.5.1'

dnl Init
dnl AC_INIT(README)
AC_INIT(epiar, 0.5.1)
AC_CONFIG_SRCDIR(README)
AC_PREREQ(2.50)

dnl Detect the canonical host and target build environment
AC_CANONICAL_HOST
AC_CANONICAL_TARGET 

dnl AM_INIT_AUTOMAKE($PACKAGE, $VERSION)
AM_INIT_AUTOMAKE
AM_CONFIG_HEADER(config.h)

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
case "$target" in *-*-linux*)
	CFLAGS="$CFLAGS -I/usr/include/"
esac
case "$target" in *-*-freebsd*)
	CFLAGS="$CFLAGS -I/usr/local/include/"
esac
case "$target" in *-apple-darwin*)
	CFLAGS="$CFLAGS -I/opt/local/include/"
esac

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


dnl Include FTGL
case "$target" in
	*-*-linux* | *-*-cygwin* | *-*-mingw32* | *-*-freebsd* | *-apple-darwin*)
	PKG_CHECK_MODULES([FTGL], [ftgl >= 2.1.3])
	CFLAGS="$CFLAGS $FTGL_CFLAGS"
	LIBS="$LIBS $FTGL_LIBS"
esac

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

dnl Check for SDL
SDL_VERSION=1.2.10

case "$target" in
	*-*-linux* | *-*-cygwin* | *-*-mingw32* | *-*-freebsd*)
		AM_PATH_SDL($SDL_VERSION, :, AC_MSG_ERROR([*** SDL version $SDL_VERSION not found!]))
		CFLAGS="$CFLAGS $SDL_CFLAGS -DCOMP_GCC"
		LIBS="$LIBS $SDL_LIBS"
esac
case "$target" in
	*-apple-darwin*)
	CFLAGS="$CFLAGS -DCOMP_GCC -I/Library/Frameworks/SDL.Framework/Headers -I/System/Library/Frameworks/OpenGL.framework/Headers"
	LIBS="$LIBS -framework SDL -framework OpenGL -framework Cocoa SDLMain.m"
esac

dnl Check for SDL_image
case "$target" in
	*-*-linux* | *-*-cygwin* | *-*-mingw32* | *-*-freebsd*)
	PKG_CHECK_MODULES([SDL_image], [SDL_image >= 1.2.10])
	CFLAGS="$CFLAGS $SDL_image_CFLAGS"
	LIBS="$LIBS $SDL_image_LIBS"
esac
case "$target" in
	*-apple-darwin*)
	CFLAGS="$CFLAGS -I/Library/Frameworks/SDL_image.Framework/Headers"
	LIBS="$LIBS -framework SDL_image"
esac

dnl Check for SDL_mixer
case "$target" in
	*-*-linux* | *-*-cygwin* | *-*-mingw32* | *-*-freebsd*)
	PKG_CHECK_MODULES([SDL_mixer], [SDL_mixer >= 1.2.11])
	CFLAGS="$CFLAGS $SDL_mixer_CFLAGS"
	LIBS="$LIBS $SDL_mixer_LIBS"
esac
case "$target" in
	*-apple-darwin*)
	CFLAGS="$CFLAGS -I/Library/Frameworks/SDL_mixer.Framework/Headers"
	LIBS="$LIBS -framework SDL_mixer"
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