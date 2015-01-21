// settingbox.cpp
//
// LameView settings combo box.
//
// (C) Copyright 2009 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: settingbox.cpp,v 1.1 2010/01/04 19:32:12 cvs Exp $
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

#include <settingbox.h>

SettingBox::SettingBox(QWidget *parent)
  : QComboBox(parent)
{
}


unsigned SettingBox::value() const
{
  return itemData(currentIndex()).toUInt();
}


bool SettingBox::setValue(unsigned val)
{
  for(int i=0;i<count();i++) {
    if(itemData(i).toUInt()==val) {
      setCurrentIndex(i);
      return true;
    }
  }
  return false;
}
