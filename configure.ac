dnl configure.in
dnl
dnl Autoconf configuration for LameView.
dnl
dnl Use autoconf to process this into a configure script
dnl
dnl   (C) Copyright 2009 Fred Gleason <fredg@paravelsystems.com>
dnl
dnl      $Id: configure.in,v 1.7 2011/03/12 03:57:38 cvs Exp $
dnl
dnl   This program is free software; you can redistribute it and/or modify
dnl   it under the terms of the GNU General Public License version 2 as
dnl   published by the Free Software Foundation.
dnl
dnl   This program is distributed in the hope that it will be useful,
dnl   but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl   GNU General Public License for more details.
dnl
dnl   You should have received a copy of the GNU General Public
dnl   License along with this program; if not, write to the Free Software
dnl   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
dnl

AC_INIT([lameview],m4_esyscmd(cat PACKAGE_VERSION))
AM_INIT_AUTOMAKE
AC_SUBST(RPM_RELEASE,1)
m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])
AC_CONFIG_MACRO_DIR([m4])

#
# Some Fully Resolved Install Paths
# (for the packaging system and friends)
#
if test ${prefix} = "NONE" ; then
  AC_SUBST(LOCAL_PREFIX,/usr/local)
else
  AC_SUBST(LOCAL_PREFIX,${prefix})
fi

#
# Basic Compiler Checks
#
AC_PROG_CC
AC_PROG_CXX
AC_LANG(C++)

#
# Option Switches
#
AC_ARG_WITH(debug,[  --with-debug            include debugging symbols in RPMs],[].[STRIP=yes])

# to avoid rpath usage :
# http://wiki.debian.net/index.cgi?RpathIssue
#case ${host} in
#  *-pc-linux-gnu)
#    AC_MSG_RESULT([Fixing libtool for -rpath problems.])
#    sed < libtool > libtool-2 \
#    's/^hardcode_libdir_flag_spec.*$/hardcode_libdir_flag_spec=" -D__LIBTOOL_IS_A_FOOL__ "/'
#    mv libtool-2 libtool
#    chmod 755 libtool
#  ;;
#esac

#
# Determine the target architecture
#
AR_GCC_TARGET()
AC_SUBST(VENDOR,$ar_gcc_distro)
AC_SUBST(ARCH,$ar_gcc_arch)

#
# Determine Distro
#
AR_GET_DISTRO()

#
# Debug Build Configuration
#
if test -n $STRIP ; then
  AC_SUBST(STRIP_DEBUG,yes)
else
  AC_SUBST(STRIP_DEBUG,"")
fi

#
# Check for Qt4 (Mandatory)
#
AC_CHECK_HEADER(QtCore/QtCore,[],[AC_MSG_ERROR([*** Qt4 not found ***])])
AC_CHECK_PROG(MOC_NAME,moc-qt4,[moc-qt4],[moc])
AC_SUBST(MOC,$MOC_NAME)

#
# Check for Libsndfile (Mandatory)
#
AC_CHECK_HEADER(sndfile.h,[],[AC_MSG_ERROR([*** libsndfile not found ***])])

#
# Check for Secret Rabbit Code (Mandatory)
#
AC_CHECK_HEADER(samplerate.h,[],[*** libsamplerate not found ***])

#
# Check for TwoLame (Optional)
#
AC_CHECK_HEADER(twolame.h,[TWOLAME_FOUND=yes],[])
if test $TWOLAME_FOUND ; then
  AC_DEFINE(TWOLAME,yes)
  AC_SUBST(LIBTWOLAME,-ltwolame)
else
  AC_SUBST(LIBTWOLAME,"")
fi

#
# Check for Lame (Optional)
#
AC_CHECK_HEADER(lame/lame.h,[LAME_FOUND=yes],[])
if test $LAME_FOUND ; then
  AC_DEFINE(LAME,yes)
  AC_SUBST(LIBLAME,-lmp3lame)
else
  AC_SUBST(LIBLAME,"")
fi

#
# Configure Linker Flags
#
if test $ar_gcc_os = "darwin10" ; then
  AC_SUBST(QTLIBS,"")
  AC_SUBST(QTFLAGS,"-framework QtCore -framework QtGui")
else
  AC_SUBST(QTLIBS,"-lQtCore -lQtGui")
  AC_SUBST(QTFLAGS,"")
fi

#
# Configure RPM Build
#
AC_CHECK_PROG(RPMBUILD_FOUND,rpmbuild,[yes],[])
if test -z $RPMBUILD_FOUND ; then
  AC_SUBST(RPMBUILD,rpm)
else
  AC_SUBST(RPMBUILD,rpmbuild)
fi
if test -d /usr/src/redhat ; then
  AC_SUBST(RPM_ROOT,/usr/src/redhat)
  AC_SUBST(VENDOR,redhat)
else
  AC_SUBST(RPM_ROOT,/usr/src/packages)
  AC_SUBST(VENDOR,suse)
fi 
if test $ar_gcc_arch = x86_64 ; then
  AC_SUBST(LW_LIB_PATH,lib64)
else
  AC_SUBST(LW_LIB_PATH,lib)
fi

AC_CONFIG_FILES([ \
    lameview.spec \
    icons/Makefile \
    xdg/Makefile \
    lameview/Makefile \
    make_bundle.sh \
    Info.plist \
    Makefile \
])
AC_OUTPUT()
chmod 755 make_bundle.sh

#
# Configuration Results
#
AC_MSG_NOTICE()
AC_MSG_NOTICE(|----------------------------------------|)
AC_MSG_NOTICE(| *** LAMEVIEW CONFIGURATION SUMMARY *** |)
AC_MSG_NOTICE(|----------------------------------------|)
AC_MSG_NOTICE(| Configured Encoders:                   |)
if test -z $TWOLAME_FOUND; then
AC_MSG_NOTICE(|                 MPEG Layer 2 ... No    |)
else
AC_MSG_NOTICE(|       MPEG Layer 2 (TwoLAME) ... Yes   |)
fi
if test -z $LAME_FOUND; then
AC_MSG_NOTICE(|                 MPEG Layer 3 ... No    |)
else
AC_MSG_NOTICE(|          MPEG Layer 3 (LAME) ... Yes   |)
fi
AC_MSG_NOTICE(|----------------------------------------|)
AC_MSG_NOTICE()
AC_MSG_NOTICE()
AC_MSG_NOTICE(Now enter 'make' to build the software.)
AC_MSG_NOTICE()
