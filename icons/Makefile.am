## automake.am
##
## icons/automake.am for LameView
##
## (C) Copyright 2009 Fred Gleason <fredg@salemradiolabs.com>
##
##      $Id: Makefile.am,v 1.2 2011/03/12 03:57:38 cvs Exp $
##
##   This program is free software; you can redistribute it and/or modify
##   it under the terms of the GNU General Public License version 2 as
##   published by the Free Software Foundation.
##
##   This program is distributed in the hope that it will be useful,
##   but WITHOUT ANY WARRANTY; without even the implied warranty of
##   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
##   GNU General Public License for more details.
##
##   You should have received a copy of the GNU General Public
##   License along with this program; if not, write to the Free Software
##   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
##
##
## Use automake to process this into a Makefile.in

install-exec-am:	
	mkdir -p $(DESTDIR)@prefix@/share/icons/hicolor/16x16/apps
	cp lameview-16x16.png $(DESTDIR)@prefix@/share/icons/hicolor/16x16/apps/lameview.png
	mkdir -p $(DESTDIR)@prefix@/share/icons/hicolor/32x32/apps
	cp lameview-32x32.png $(DESTDIR)@prefix@/share/icons/hicolor/32x32/apps/lameview.png
	mkdir -p $(DESTDIR)@prefix@/share/icons/hicolor/48x48/apps
	cp lameview-48x48.png $(DESTDIR)@prefix@/share/icons/hicolor/48x48/apps/lameview.png

uninstall:	
	rm -f $(DESTDIR)@prefix@/share/icons/hicolor/16x16/apps/lameview.png
	rm -f $(DESTDIR)@prefix@/share/icons/hicolor/32x32/apps/lameview.png
	rm -f $(DESTDIR)@prefix@/share/icons/hicolor/48x48/apps/lameview.png

EXTRA_DIST = LameView.ico\
             lameview-16x16.png\
             lameview-16x16.xpm\
             lameview-32x32.png\
             lameview-48x48.png

CLEANFILES = *~
MAINTAINERCLEANFILES = *~ Makefile.in configure aclocal.m4
