## automake.am
##
## xdg/automake.am for LameView
##
##   (C) Copyright 2009 Fred Gleason <fredg@paravelsystems.com>
##
##      $Id: Makefile.am,v 1.2 2010/12/09 18:32:35 cvs Exp $
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
## Use automake to process this into a Makefile.in

install-exec-am:	
	mkdir -p $(DESTDIR)@prefix@/share/applications
	cp lameview_lameview.desktop $(DESTDIR)@prefix@/share/applications/

uninstall:	
	rm -f $(DESTDIR)@prefix@/share/applications/lameview_lameview.desktop

EXTRA_DIST = lameview_lameview.desktop

CLEANFILES = *~
MAINTAINERCLEANFILES = *~ Makefile.in configure aclocal.m4
