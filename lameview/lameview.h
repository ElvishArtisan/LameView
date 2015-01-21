// lameview.h
//
// GUI front end of audio encoders.
//
//   (C) Copyright 2009 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: lameview.h,v 1.5 2010/01/05 16:10:11 cvs Exp $
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License version 2 as
//   published by the Free Software Foundation.
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

#ifndef LAMEVIEW_H
#define LAMEVIEW_H

#include <QtGui/QtGui>

#include <settingbox.h>

#define LAMEVIEW_USAGE "\n"
#define DEFAULT_ENCODER MainWidget::Layer2Encoder
#define DEFAULT_MODE MainWidget::StereoMode
#define DEFAULT_BITRATE 64
#define DEFAULT_SAMPRATE 0
#define DEFAULT_SAMPRATE_CONVERTER 1
#define DEFAULT_DELETE_SOURCE false
#define LAMEVIEW_RCDIR ".lameviewrc/"
#define LAMEVIEW_RCFILE "settings"
#define LAMEVIEW_LAYER2_EXTENSION "mp2"
#define LAMEVIEW_LAYER3_EXTENSION "mp3"

class MainWidget : public QWidget
{
  Q_OBJECT
 public:
  MainWidget(QWidget *parent=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void sourceAddData();
  void sourceRemoveData();
  void destinationSelectData();
  void algorithmActivatedData(int index);
  void bitrateActivatedData(int index);
  void startClickedData();

 protected:
  void resizeEvent(QResizeEvent *e);
  void closeEvent(QCloseEvent *e);

 private:
  enum Encoder {Layer2Encoder=2,Layer3Encoder=3};
  enum Mode {StereoMode=0,JointStereoMode=1,DualChannelMode=2,
	     SingleChannelMode=3};
  void StartEncoding();
  void AbortEncoding();
  bool Encode(const QString &srcfile,MainWidget::Encoder coder,
	      MainWidget::Mode mode,unsigned bitrate,unsigned samprate,
	      unsigned srcconv);
  bool EncodeLayer2(const QString &srcfile,MainWidget::Mode mode,
		    unsigned bitrate,unsigned samprate,unsigned srcconv);
  bool EncodeLayer3(const QString &srcfile,MainWidget::Mode mode,
		    unsigned bitrate,unsigned samprate,unsigned srcconv);
  void SetBitrates(MainWidget::Encoder coder);
  void SetModes(MainWidget::Encoder coder,unsigned bitrate,bool warn);
  void LoadSettings();
  void SaveSettings();
  QString MakeDestinationName(const QString &srcname,const QString &ext);
  QLabel *view_src_label;
  QListWidget *view_src_list;
  QLabel *view_delete_label;
  QCheckBox *view_delete_check;
  QLabel *view_dst_label;
  QLineEdit *view_dst_edit;
  QPushButton *view_add_button;
  QPushButton *view_remove_button;
  QPushButton *view_select_button;
  QLabel *view_encoder_label;
  QLabel *view_algorithm_label;
  SettingBox *view_algorithm_box;
  QLabel *view_mode_label;
  SettingBox *view_mode_box;
  QLabel *view_bitrate_label;
  SettingBox *view_bitrate_box;
  QLabel *view_samprate_label;
  SettingBox *view_samprate_box;
  QLabel *view_converter_label;
  SettingBox *view_converter_box;
  QPushButton *view_start_button;
  MainWidget::Encoder view_encoder;
  MainWidget::Mode view_mode;
  unsigned view_bitrate;
  unsigned view_samprate;
  QString view_source_dir;
  QString view_destination_dir;
  unsigned view_samprate_converter;
  bool view_delete_source;
  QLabel *view_filebar_label;
  QProgressBar *view_filebar;
  QLabel *view_totalbar_label;
  QProgressBar *view_totalbar;
  bool view_encoding;
  bool view_aborting;
};


#endif  // LAMEVIEW_H
