dnl @synopsis AC_CXX_EXPLICIT
dnl
dnl If the compiler can be asked to prevent using implicitly one argument
dnl constructors as converting constructors with the explicit
dnl keyword, define HAVE_EXPLICIT.
dnl
dnl @version $Id: aclocal.m4,v 1.4 2006/01/15 20:56:41 emgertz Exp $
dnl @author Luc Maisonobe
dnl
AC_DEFUN([AC_CXX_EXPLICIT],
[AC_CACHE_CHECK(whether the compiler supports the explicit keyword,
ac_cv_cxx_explicit,
[AC_LANG_SAVE
 AC_LANG_CPLUSPLUS
 AC_TRY_COMPILE([class A{public:explicit A(double){}};],
[double c = 5.0;A x(c);return 0;],
 ac_cv_cxx_explicit=yes, ac_cv_cxx_explicit=no)
 AC_LANG_RESTORE
])
if test "$ac_cv_cxx_explicit" = yes; then
  AC_DEFINE(HAVE_EXPLICIT,,[define if the compiler supports the explicit keyword])
fi
])
dnl @synopsis AC_CXX_MEMBER_TEMPLATES
dnl
dnl If the compiler supports member templates, define HAVE_MEMBER_TEMPLATES.
dnl
dnl @version $Id: aclocal.m4,v 1.4 2006/01/15 20:56:41 emgertz Exp $
dnl @author Luc Maisonobe
dnl
AC_DEFUN([AC_CXX_MEMBER_TEMPLATES],
[AC_CACHE_CHECK(whether the compiler supports member templates,
ac_cv_cxx_member_templates,
[AC_LANG_SAVE
 AC_LANG_CPLUSPLUS
 AC_TRY_COMPILE([
template<class T, int N> class A
{ public:
  template<int N2> A<T,N> operator=(const A<T,N2>& z) { return A<T,N>(); }
};],[A<double,4> x; A<double,7> y; x = y; return 0;],
 ac_cv_cxx_member_templates=yes, ac_cv_cxx_member_templates=no)
 AC_LANG_RESTORE
])
if test "$ac_cv_cxx_member_templates" = yes; then
  AC_DEFINE(HAVE_MEMBER_TEMPLATES,,[define if the compiler supports member templates])
fi
])

dnl AC_CHECK_OBJ(LINKOBJ, FUNCTION, VARIABLE, OTHER_LIBS)
AC_DEFUN(AC_CHECK_OBJ,
[AC_MSG_CHECKING([for $2 in $1])
ac_save_LIBS="$LIBS"
LIBS="$1 $4 $LIBS"
AC_TRY_LINK(dnl
ifelse(_AC_LANG, [FORTRAN77], ,
ifelse([$2], [main], , dnl Avoid conflicting decl of main.
[/* Override any gcc2 internal prototype to avoid an error.  */
]ifelse(_AC_LANG, C++, [#ifdef __cplusplus
extern "C"
#endif
])dnl
[/* We use char because int might match the return type of a gcc2
    builtin and then its argument prototype would still apply.  */
char $2();
])),
	    [$2()],
	    $3=yes,
	    $3=no)
LIBS="$ac_save_LIBS"
if test $blas_ok = yes; then
  AC_MSG_RESULT(yes)
else
  AC_MSG_RESULT(no)
fi
])
