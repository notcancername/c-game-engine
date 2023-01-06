#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
AC_INIT([c-game-engine], [0.1.0])
AC_CONFIG_SRCDIR([engine])

AC_PROG_CC
AC_PROG_RANLIB

AM_INIT_AUTOMAKE([foreign subdir-objects silent-rules])
AM_SILENT_RULES([yes])

AC_SEARCH_LIBS([cpSpaceNew], [chipmunk], [], [AC_MSG_ERROR([install chipmunk2d])])
AC_SEARCH_LIBS([SDL_Init], [SDL2 SDL2main sdl2], [], [AC_MSG_ERROR([install SDL2])])
AC_SEARCH_LIBS([IMG_Load], [SDL2_image SDL2 SDL2main], [], [AC_MSG_ERROR([install SDL2_image])])
AC_SEARCH_LIBS([Mix_Init], [SDL2_mixer SDL2 SDL2main], [], [AC_MSG_ERROR([install SDL2_mixer])])
AC_CHECK_HEADERS([SDL2/SDL.h], [], [AC_MSG_ERROR([install SDL2 headers])])
AC_CHECK_HEADERS([SDL2/SDL_image.h], [], [AC_MSG_ERROR([install SDL2_image headers])])
AC_CHECK_HEADERS([SDL2/SDL_mixer.h], [], [AC_MSG_ERROR([install SDL2_mixer headers])])
AC_CHECK_HEADERS([chipmunk/chipmunk.h], [], [AC_MSG_ERROR([install chipmunk headers])])

AC_CHECK_PROGS(FREETYPE, freetype-config)
if test -n "$FREETYPE"; then
  LIBFREETYPE_CFLAGS="`$FREETYPE --cflags`"
  LIBFREETYPE_LIBS="`$FREETYPE --libs`"
else
  AC_MSG_WARN([freetype-config not found; guessing flags])
  LIBFREETYPE_CFLAGS="-I/usr/include/freetype2"
  LIBFREETYPE_LIBS="-lfreetype"
fi
AC_SUBST(LIBFREETYPE_CFLAGS)
AC_SUBST(LIBFREETYPE_LIBS)

dnl system-specific stuff
AC_CANONICAL_BUILD
AS_IF([test "$build_os" = "w32" -o "$build_os" = "w64"], [
dnl todo
    AC_SEARCH_LIBS([glewInit], [glew32], [], [AC_MSG_WARN([can't find GLEW])])
], [
    AC_SEARCH_LIBS([sqrt], [m], [], [AC_MSG_WARN([incomplete libm?])])
    AC_SEARCH_LIBS([glXCreateContext], [GL], [], [AC_MSG_WARN([can't find libGL])])
    AC_SEARCH_LIBS([glewInit], [GLEW], [], [AC_MSG_WARN([can't find GLEW])])
])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
