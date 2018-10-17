AC_INIT([misw], 1.0, [emcconville@emcconville.com])
AM_INIT_AUTOMAKE([foreign])
AC_PROG_CC
AC_FUNC_MALLOC
AC_CHECK_HEADERS([ncurses.h], [], [AC_MSG_ERROR([unable to find ncurses.h])])
AC_SEARCH_LIBS([initscr],[ncurses], [], [AC_MSG_ERROR([unable to find ncurses library])])
AC_CONFIG_FILES(Makefile)
AC_OUTPUT
