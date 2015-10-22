# On some platforms we need to explicitly link with -liconv in order to use iconv() and related functions.
#
# CHECK_ICONV([ACTION-IF-NEEDED])
#
# If no action is given, adds -liconv to LIBS

m4_define([_CHECK_ICONV_testbody], [[
  #include <iconv.h>

  int main() {
    iconv_t ret = iconv_open(0, 0);
    return (ret == (iconv_t)-1) ? 1 : 0;
  }
]])

AC_DEFUN([CHECK_ICONV], [

  AC_LANG_PUSH(C++)

  AC_MSG_CHECKING([whether iconv can be used without link library])

  AC_LINK_IFELSE([AC_LANG_SOURCE([_CHECK_ICONV_testbody])],[
      AC_MSG_RESULT([yes])
    ],[
      AC_MSG_RESULT([no])
      OLDLIBS="$LIBS"
      LIBS="$LIBS -liconv"
      AC_MSG_CHECKING([whether iconv needs -liconv])
      AC_LINK_IFELSE([AC_LANG_SOURCE([_CHECK_ICONV_testbody])],[
          AC_MSG_RESULT([yes])
	  LIBS="$OLDLIBS"
          m4_default([$1], [LIBS="$LIBS -liconv])
        ],[
          AC_MSG_RESULT([no])
          AC_MSG_FAILURE([cannot figure our how to use iconv])
        ])
    ])

  AC_LANG_POP
])
