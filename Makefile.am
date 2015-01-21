## automake.am
##
## Top level automake.am for LameView
##
##   (C) Copyright 2009 Fred Gleason <fredg@paravelsystems.com>
##
##      $Id: Makefile.am,v 1.3 2011/03/12 03:57:38 cvs Exp $
##
##    This program is free software; you can redistribute it and/or modify
##    it under the terms of the GNU General Public License version 2 as
##    published by the Free Software Foundation.
## 
##    This program is distributed in the hope that it will be useful,
##    but WITHOUT ANY WARRANTY; without even the implied warranty of
##    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
##    GNU General Public License for more details.
## 
##    You should have received a copy of the GNU General Public
##    License along with this program; if not, write to the Free Software
##    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
## 
## Use automake to process this into a Makefile.in

ACLOCAL_AMFLAGS = -I m4

SUBDIRS = icons xdg lameview

maintainer-clean-local:	
	rm -rf m4

dmg:	all
	./make_bundle.sh

rpm:	dist
	cp lameview-$(VERSION).tar.gz $(HOME)/rpmbuild/SOURCES/
	$(RPMBUILD) -ba --target $(ARCH)-$(VENDOR)-linux lameview.spec
	mv $(HOME)/rpmbuild/RPMS/$(ARCH)/lameview-$(VERSION)-@RPM_RELEASE@.$(ARCH).rpm $(top_srcdir)/
	mv $(HOME)/rpmbuild/SRPMS/lameview-$(VERSION)-@RPM_RELEASE@.src.rpm $(top_srcdir)/
	rm $(HOME)/rpmbuild/SOURCES/lameview-$(VERSION).tar.gz
	rm -rf $(HOME)/rpmbuild/BUILD/lameview-$(VERSION)

slack:	all
	./make_slack

EXTRA_DIST = autogen.sh lameview.spec.in PACKAGE_VERSION\
             get_target.sh get_distro.sh make_bundle.sh.in Info.plist.in

CLEANFILES = *~ *.tar.gz *.tgz *.mo *.rpm *.zip *.dmg
DISTCLEANFILES = *.spec config.guess config.sub Info.plist make_bundle.sh
MAINTAINERCLEANFILES = *~ Makefile.in configure aclocal.m4 *.tar.gz\
                       config.guess config.status config.sub install-sh\
                       libtool ltmain.sh missing mkinstalldirs depcomp\
                       *.spec
