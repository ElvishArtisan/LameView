// profileline.h
//
// A container class for profile lines.
//
// (C) Copyright 2002-2007,2009 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: profileline.h,v 1.1 2010/01/04 19:32:12 cvs Exp $
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU Library General Public License 
//   version 2 as published by the Free Software Foundation.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public
//   License along with this program; if not, write to the Free Software
//   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//

#ifndef PROFILELINE_H
#define PROFILELINE_H

#include <QtCore/QtCore>

class ProfileLine
{
 public:
  ProfileLine();
  QString tag() const;
  void setTag(QString tag);
  QString value() const;
  void setValue(QString value);
  void clear();

 private:
  QString line_tag;
  QString line_value;
};


#endif  // PROFILELINE_H
