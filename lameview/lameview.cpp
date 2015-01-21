// lameview.cpp
//
// GUI front end of audio encoders.
//
//   (C) Copyright 2009 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: lameview.cpp,v 1.8 2011/03/11 23:45:56 cvs Exp $
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

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <samplerate.h>

#include <cmdswitch.h>

#include <profile.h>
#include <lameview.h>

#include <../icons/lameview-16x16.xpm>

MainWidget::MainWidget(QWidget *parent)
  : QWidget(parent)
{
  view_encoding=false;
  view_aborting=false;
  new CmdSwitch(qApp->argc(),qApp->argv(),"lameview",LAMEVIEW_USAGE);

  //
  // Default Settings
  //
  view_encoder=DEFAULT_ENCODER;
  view_mode=DEFAULT_MODE;
  view_bitrate=DEFAULT_BITRATE;
  view_samprate=DEFAULT_SAMPRATE;
  view_samprate_converter=DEFAULT_SAMPRATE_CONVERTER;
  view_delete_source=DEFAULT_DELETE_SOURCE;

  if(getenv("HOME")==NULL) {
    view_destination_dir="/";
    view_source_dir="/";
  }
  else {
    view_destination_dir=getenv("HOME");
    view_source_dir=getenv("HOME");
  }
  LoadSettings();

  //
  // Window Appearance
  //
  setWindowTitle("LameView");
  setMinimumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setWindowIcon(QPixmap(lameview_xpm));

  //
  // Fonts
  //
  QFont label_font("helvetica",12,QFont::Bold);
  label_font.setPixelSize(12);
  QFont section_font("helvetica",16,QFont::Bold);
  section_font.setPixelSize(16);

  //
  // Source Files
  //
  view_src_label=new QLabel(tr("Source Files"),this);
  view_src_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
  view_src_label->setFont(label_font);
  view_src_list=new QListWidget(this);
  view_src_list->setSelectionBehavior(QAbstractItemView::SelectRows);
  view_src_list->setSelectionMode(QAbstractItemView::ExtendedSelection);
  view_add_button=new QPushButton(tr("Add"),this);
  connect(view_add_button,SIGNAL(clicked()),this,SLOT(sourceAddData()));
  view_remove_button=new QPushButton(tr("Remove"),this);
  connect(view_remove_button,SIGNAL(clicked()),this,SLOT(sourceRemoveData()));
  view_src_list->setSortingEnabled(true);

  //
  // Delete Source Checkbox
  //
  view_delete_label=new QLabel(tr("Delete source files after encoding"),this);
  view_delete_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
  view_delete_label->setFont(label_font);
  view_delete_check=new QCheckBox(this);

  //
  // Destination Directory
  //
  view_dst_label=new QLabel(tr("Destination Directory:"),this);
  view_dst_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  view_dst_label->setFont(label_font);
  view_dst_edit=new QLineEdit(this);
  view_select_button=new QPushButton(tr("Select"),this);
  connect(view_select_button,SIGNAL(clicked()),
	  this,SLOT(destinationSelectData()));

  //
  // Encoder Settings Section
  //
  view_encoder_label=new QLabel(tr("Encoder Settings"),this);
  view_encoder_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
  view_encoder_label->setFont(section_font);
  view_algorithm_label=new QLabel(tr("Algorithm:"),this);
  view_algorithm_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  view_algorithm_label->setFont(label_font);
  view_algorithm_box=new SettingBox(this);
#ifdef LAME
  view_algorithm_box->insertItem(0,tr("MPEG Layer 3"),
				 MainWidget::Layer3Encoder);
#endif  // LAME
#ifdef TWOLAME
  view_algorithm_box->insertItem(0,tr("MPEG Layer 2"),
				 MainWidget::Layer2Encoder);
#endif  // TWOLAME
  connect(view_algorithm_box,SIGNAL(activated(int)),
	  this,SLOT(algorithmActivatedData(int)));
  view_mode_label=new QLabel(tr("Mode:"),this);
  view_mode_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  view_mode_label->setFont(label_font);
  view_mode_box=new SettingBox(this);
  view_bitrate_label=new QLabel(tr("Bit Rate:"),this);
  view_bitrate_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  view_bitrate_label->setFont(label_font);
  view_bitrate_box=new SettingBox(this);
  connect(view_bitrate_box,SIGNAL(activated(int)),
	  this,SLOT(bitrateActivatedData(int)));
  view_samprate_label=new QLabel(tr("Sample Rate:"),this);
  view_samprate_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  view_samprate_label->setFont(label_font);
  view_samprate_box=new SettingBox(this);
  view_samprate_box->insertItem(0,tr("48000 s/sec"),48000);
  view_samprate_box->insertItem(0,tr("44100 s/sec"),44100);
  view_samprate_box->insertItem(0,tr("32000 s/sec"),32000);
  view_samprate_box->insertItem(0,tr("Automatic"),0);

  view_converter_label=new QLabel(tr("Sample Rate Converter:"),this);
  view_converter_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  view_converter_label->setFont(label_font);
  view_converter_box=new SettingBox(this);
  int c=0;
  while(src_get_name(c)!=NULL) {
    view_converter_box->
      insertItem(view_converter_box->count(),src_get_name(c),c);
    c++;
  }

  //
  // Progress Bars
  //
  view_filebar_label=new QLabel(tr("File Progress"),this);
  view_filebar_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
  view_filebar_label->setFont(label_font);
  view_filebar=new QProgressBar(this);
  view_totalbar_label=new QLabel(tr("Total Progress"),this);
  view_totalbar_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
  view_totalbar_label->setFont(label_font);
  view_totalbar=new QProgressBar(this);

  //
  // Start Button
  //
  view_start_button=new QPushButton(tr("Start Encoding"),this);
  view_start_button->setFont(section_font);
  view_start_button->setDisabled(true);
  connect(view_start_button,SIGNAL(clicked()),
	  this,SLOT(startClickedData()));

  //
  // Load Settings
  //
  view_dst_edit->setText(view_destination_dir);
  view_algorithm_box->setValue(view_encoder);
  view_samprate_box->setValue(view_samprate);
  view_converter_box->setValue(view_samprate_converter);
  view_delete_check->setChecked(view_delete_source);
  SetModes(view_encoder,view_bitrate,false);
  view_mode_box->setValue(view_mode);
  view_bitrate_box->setValue(view_bitrate);
}


QSize MainWidget::sizeHint() const
{
  return QSize(440,480);
}


QSizePolicy MainWidget::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void MainWidget::sourceAddData()
{
  QStringList files;
  files=QFileDialog::getOpenFileNames(this,tr("Select Files to Encode"),
				      view_source_dir,
				      "Audio Files (*.aiff *.AIFF *.au *.AU *.voc *.VOC *.flac *.FLAC *.wav *.WAV)\nAll Files (*.*)\nAIFF Files (*.aiff *.AIFF)\nAU Files (*.au *.AU)\nFree Lossless Audio Codec [FLAC] Files (*.flac *.FLAC)\nVOC Files (*.voc *.VOC)\nWAV Files (*.wav *.WAV)");
  if(files.size()==0) {
    return;
  }

  //
  // Update Source Directory
  //
  view_source_dir=files[0].left(files[0].lastIndexOf("/"));

  //
  // Remove duplicates
  //
  for(int i=0;i<view_src_list->count();i++) {
    for(int j=files.size()-1;j>=0;j--) {
      if(view_src_list->item(i)->text()==files[j]) {
	files.erase(files.begin()+j);
      }
    }
  }

  //
  // Add new entries
  //
  for(int i=0;i<files.size();i++) {
    new QListWidgetItem(files[i],view_src_list);
  }
  view_start_button->setEnabled(view_src_list->count()>0);
}


void MainWidget::sourceRemoveData()
{
  for(int i=view_src_list->count()-1;i>=0;i--) {
    if(view_src_list->item(i)->isSelected()) {
      delete view_src_list->item(i);
    }
  }
  view_start_button->setEnabled(view_src_list->count()>0);
}


void MainWidget::destinationSelectData()
{
  QString dirname;
  dirname=
    QFileDialog::getExistingDirectory(this,tr("Select Destination Directory"),
				      view_dst_edit->text());
  if(!dirname.isEmpty()) {
    view_dst_edit->setText(dirname);
  }
}


void MainWidget::algorithmActivatedData(int index)
{
  SetModes((MainWidget::Encoder)view_algorithm_box->itemData(index).toUInt(),
	   view_bitrate_box->itemData(view_bitrate_box->currentIndex()).
	   toUInt(),true);
}


void MainWidget::bitrateActivatedData(int index)
{
  SetModes((MainWidget::Encoder)view_algorithm_box->
	   itemData(view_algorithm_box->currentIndex()).toUInt(),
	   view_bitrate_box->itemData(index).toUInt(),true);
}


void MainWidget::startClickedData()
{
  if(view_encoding) {
    AbortEncoding();
  }
  else {
    StartEncoding();
  }
}


void MainWidget::resizeEvent(QResizeEvent *e)
{
  view_src_label->setGeometry(15,18,sizeHint().width()/2,20);
  view_add_button->setGeometry(size().width()-180,8,80,25);
  view_remove_button->setGeometry(size().width()-95,8,80,25);
  view_src_list->setGeometry(10,36,size().width()-20,size().height()-350);
  view_delete_check->setGeometry(15,size().height()-313,20,20);
  view_delete_label->setGeometry(35,size().height()-312,size().width()-45,20);
  view_dst_label->setGeometry(10,size().height()-284,140,20);
  view_dst_edit->setGeometry(155,size().height()-288,size().width()-255,24);
  view_select_button->setGeometry(size().width()-95,size().height()-287,80,21);
  view_encoder_label->setGeometry(20,size().height()-256,size().width()-40,20);
  view_algorithm_label->setGeometry(10,size().height()-203,80,20);
  view_algorithm_box->setGeometry(95,size().height()-203,120,20);
  view_mode_label->setGeometry(10,size().height()-230,80,20);
  view_mode_box->setGeometry(95,size().height()-230,120,20);
  view_bitrate_label->setGeometry(230,size().height()-203,80,20);
  view_bitrate_box->setGeometry(315,size().height()-203,110,20);
  view_samprate_label->setGeometry(230,size().height()-230,80,20);
  view_samprate_box->setGeometry(315,size().height()-230,110,20);
  view_converter_label->setGeometry(10,size().height()-176,170,20);
  view_converter_box->setGeometry(185,size().height()-176,200,20);
  view_filebar_label->setGeometry(15,size().height()-140,size().width()-30,20);
  view_filebar->setGeometry(10,size().height()-122,size().width()-20,20);
  view_totalbar_label->setGeometry(15,size().height()-98,size().width()-30,20);
  view_totalbar->setGeometry(10,size().height()-80,size().width()-20,20);
  view_start_button->setGeometry(10,size().height()-54,size().width()-20,44);
}


void MainWidget::closeEvent(QCloseEvent *e)
{
  view_encoder=(MainWidget::Encoder)view_algorithm_box->value();
  view_mode=(MainWidget::Mode)view_mode_box->value();
  view_bitrate=view_bitrate_box->value();
  view_samprate=view_samprate_box->value();
  view_samprate_converter=view_converter_box->value();
  view_delete_source=view_delete_check->isChecked();
  view_destination_dir=view_dst_edit->text();
  SaveSettings();
  qApp->quit();
}


void MainWidget::StartEncoding()
{
  QStringList files;

  if(view_delete_check->isChecked()) {
    if(QMessageBox::warning(this,tr("ViewLame - Source Delete"),
			    tr("Source files will be deleted after encoding!\nDo you still wish to proceed?"),QMessageBox::Yes,QMessageBox::No)!=QMessageBox::Yes) {
      return;
    }
  }
  view_encoding=true;
  view_aborting=false;
  view_start_button->setText(tr("Abort Encoding"));
  for(int i=0;i<view_src_list->count();i++) {
    files.push_back(view_src_list->item(i)->text());
  }
  view_totalbar->setRange(0,view_src_list->count());
  for(int i=0;i<files.size();i++) {
    view_totalbar->setValue(i);
    qApp->processEvents();
    if(Encode(files[i],
	      (MainWidget::Encoder)view_algorithm_box->value(),
	      (MainWidget::Mode)view_mode_box->value(),
	      view_bitrate_box->value(),
	      view_samprate_box->value(),
	      view_converter_box->value())) {
      if(view_delete_check->isChecked()) {
	QFile::remove(files[i]);
      }
      for(int j=view_src_list->count()-1;j>=0;j--) {
	if(files[i]==view_src_list->item(j)->text()) {
	  delete view_src_list->item(j);
	}
      }
    }
    if(view_aborting) {
      view_filebar->reset();
      view_totalbar->reset();
      view_start_button->setText(tr("Start Encoding"));
      view_encoding=false;
      view_start_button->setEnabled(view_src_list->count()>0);
      QMessageBox::warning(this,tr("LameView - Progress"),
			   tr("Encoding aborted!"));
      return;
    }
  }
  view_totalbar->reset();
  view_start_button->setText(tr("Start Encoding"));
  view_encoding=false;
  view_start_button->setEnabled(view_src_list->count()>0);
  QMessageBox::information(this,tr("LameView - Progress"),
			   tr("Encoding complete!"));
}


void MainWidget::AbortEncoding()
{
  view_aborting=true;
}


bool MainWidget::Encode(const QString &srcfile,MainWidget::Encoder coder,
			MainWidget::Mode mode,unsigned bitrate,
			unsigned samprate,unsigned srcconv)
{
  switch(coder) {
  case MainWidget::Layer2Encoder:
    return EncodeLayer2(srcfile,
			(MainWidget::Mode)view_mode_box->value(),
			view_bitrate_box->value(),
			view_samprate_box->value(),
			view_converter_box->value());

  case MainWidget::Layer3Encoder:
    return EncodeLayer3(srcfile,
			(MainWidget::Mode)view_mode_box->value(),
			view_bitrate_box->value(),
			view_samprate_box->value(),
			view_converter_box->value());
  }
  return false;
}


void MainWidget::SetBitrates(MainWidget::Encoder coder)
{
  unsigned bitrate=
    view_bitrate_box->itemData(view_bitrate_box->currentIndex()).toUInt();
  view_bitrate_box->clear();
  switch(coder) {
  case MainWidget::Layer2Encoder:
    view_bitrate_box->
      insertItem(view_bitrate_box->count(),tr("32 kbits/sec"),32);
    view_bitrate_box->
      insertItem(view_bitrate_box->count(),tr("48 kbits/sec"),48);
    view_bitrate_box->
      insertItem(view_bitrate_box->count(),tr("56 kbits/sec"),56);
    view_bitrate_box->
      insertItem(view_bitrate_box->count(),tr("64 kbits/sec"),64);
    view_bitrate_box->
      insertItem(view_bitrate_box->count(),tr("80 kbits/sec"),80);
    view_bitrate_box->
      insertItem(view_bitrate_box->count(),tr("96 kbits/sec"),96);
    view_bitrate_box->
      insertItem(view_bitrate_box->count(),tr("112 kbits/sec"),112);
    view_bitrate_box->
      insertItem(view_bitrate_box->count(),tr("128 kbits/sec"),128);
    view_bitrate_box->
      insertItem(view_bitrate_box->count(),tr("160 kbits/sec"),160);
    view_bitrate_box->
      insertItem(view_bitrate_box->count(),tr("192 kbits/sec"),192);
    view_bitrate_box->
      insertItem(view_bitrate_box->count(),tr("224 kbits/sec"),224);
    view_bitrate_box->
      insertItem(view_bitrate_box->count(),tr("256 kbits/sec"),256);
    view_bitrate_box->
      insertItem(view_bitrate_box->count(),tr("320 kbits/sec"),320);
    view_bitrate_box->
      insertItem(view_bitrate_box->count(),tr("384 kbits/sec"),384);
    break;

  case MainWidget::Layer3Encoder:
    view_bitrate_box->
      insertItem(view_bitrate_box->count(),tr("32 kbits/sec"),32);
    view_bitrate_box->
      insertItem(view_bitrate_box->count(),tr("40 kbits/sec"),40);
    view_bitrate_box->
      insertItem(view_bitrate_box->count(),tr("48 kbits/sec"),48);
    view_bitrate_box->
      insertItem(view_bitrate_box->count(),tr("56 kbits/sec"),56);
    view_bitrate_box->
      insertItem(view_bitrate_box->count(),tr("64 kbits/sec"),64);
    view_bitrate_box->
      insertItem(view_bitrate_box->count(),tr("80 kbits/sec"),80);
    view_bitrate_box->
      insertItem(view_bitrate_box->count(),tr("96 kbits/sec"),96);
    view_bitrate_box->
      insertItem(view_bitrate_box->count(),tr("112 kbits/sec"),112);
    view_bitrate_box->
      insertItem(view_bitrate_box->count(),tr("128 kbits/sec"),128);
    view_bitrate_box->
      insertItem(view_bitrate_box->count(),tr("160 kbits/sec"),160);
    view_bitrate_box->
      insertItem(view_bitrate_box->count(),tr("192 kbits/sec"),192);
    view_bitrate_box->
      insertItem(view_bitrate_box->count(),tr("224 kbits/sec"),224);
    view_bitrate_box->
      insertItem(view_bitrate_box->count(),tr("256 kbits/sec"),256);
    view_bitrate_box->
      insertItem(view_bitrate_box->count(),tr("320 kbits/sec"),320);
    break;
  }
  for(int i=0;i<view_bitrate_box->count();i++) {
    if(bitrate<=view_bitrate_box->itemData(i).toUInt()) {
      view_bitrate_box->setCurrentIndex(i);
      return;
    }
  }
}


void MainWidget::SetModes(MainWidget::Encoder coder,unsigned bitrate,bool warn)
{
  MainWidget::Mode mode=(MainWidget::Mode)view_mode_box->
    itemData(view_mode_box->currentIndex()).toUInt();
  view_mode_box->clear();
  switch(coder) {
  case MainWidget::Layer2Encoder:
    switch(bitrate) {
    case 32:
    case 48:
    case 56:
    case 80:
      view_mode_box->
	insertItem(0,tr("Single Channel"),MainWidget::SingleChannelMode);
      break;

    case 224:
    case 256:
    case 320:
    case 384:
      view_mode_box->
	insertItem(0,tr("Dual Channel"),MainWidget::DualChannelMode);
      view_mode_box->
	insertItem(0,tr("Joint Stereo"),MainWidget::JointStereoMode);
      view_mode_box->insertItem(0,tr("Stereo"),MainWidget::StereoMode);
      break;

    default:
      view_mode_box->
	insertItem(0,tr("Single Channel"),MainWidget::SingleChannelMode);
      view_mode_box->
	insertItem(0,tr("Dual Channel"),MainWidget::DualChannelMode);
      view_mode_box->
	insertItem(0,tr("Joint Stereo"),MainWidget::JointStereoMode);
      view_mode_box->insertItem(0,tr("Stereo"),MainWidget::StereoMode);
      break;
    }
    break;

  case MainWidget::Layer3Encoder:
    view_mode_box->
      insertItem(0,tr("Single Channel"),MainWidget::SingleChannelMode);
    view_mode_box->insertItem(0,tr("Dual Channel"),MainWidget::DualChannelMode);
    view_mode_box->insertItem(0,tr("Joint Stereo"),MainWidget::JointStereoMode);
    view_mode_box->insertItem(0,tr("Stereo"),MainWidget::StereoMode);
    break;
  }
  if(!view_mode_box->setValue(mode)) {
    if(warn) {
      QMessageBox::information(this,tr("LameView - Settings Changed"),
			       tr("Warning - Encoding mode has been changed!\n\n(Previous setting incompatible with new Algorithm/Bitrate settings.)"));
    }
  }
  SetBitrates(coder);
}


void MainWidget::LoadSettings()
{
  if(getenv("HOME")==NULL) {
    return;
  }
  Profile *p=new Profile();
  p->setSource(QString(getenv("HOME"))+"/"+LAMEVIEW_RCDIR+LAMEVIEW_RCFILE);
  view_encoder=
    (MainWidget::Encoder)p->intValue("Settings","Encoder",view_encoder);
  view_mode=
    (MainWidget::Mode)p->intValue("Settings","Mode",view_mode);
  view_bitrate=p->intValue("Settings","Bitrate",view_bitrate);
  view_samprate=p->intValue("Settings","Samplerate",view_samprate);
  view_samprate_converter=
    p->intValue("Settings","SamplerateConverter",view_samprate_converter);
  view_delete_source=p->intValue("Settings","DeleteSource",view_delete_source);
  view_destination_dir=p->stringValue("Settings","DestinationDirectory",
				      view_destination_dir);
  view_source_dir=p->stringValue("Settings","SourceDirectory",
				      view_source_dir);
  delete p;
}


void MainWidget::SaveSettings()
{
  FILE *f=NULL;
  if(getenv("HOME")==NULL) {
    return;
  }
  QDir dir(QString(getenv("HOME"))+"/"+LAMEVIEW_RCDIR);
  if(!dir.exists()) {
    mkdir(dir.absolutePath().toAscii(),S_IRUSR|S_IWUSR|S_IXUSR);
  }
  if((f=fopen((QString(getenv("HOME"))+"/"+LAMEVIEW_RCDIR+LAMEVIEW_RCFILE).
	      toAscii(),"w"))==NULL) {
    return;
  }
  fprintf(f,"[Settings]\n");
  fprintf(f,"Encoder=%u\n",view_encoder);
  fprintf(f,"Mode=%u\n",view_mode);
  fprintf(f,"Bitrate=%u\n",view_bitrate);
  fprintf(f,"Samplerate=%u\n",view_samprate);
  fprintf(f,"SamplerateConverter=%u\n",view_samprate_converter);
  fprintf(f,"DeleteSource=%u\n",view_delete_source);
  fprintf(f,"SourceDirectory=%s\n",(const char *)view_source_dir.toAscii());
  fprintf(f,"DestinationDirectory=%s\n",
	  (const char *)view_destination_dir.toAscii());
  fclose(f);
}


QString MainWidget::MakeDestinationName(const QString &srcname,
					const QString &ext)
{
  QStringList path=srcname.split("/");
  QString basename=path[path.size()-1];
  basename=basename.left(basename.lastIndexOf("."));
  QString dir=view_dst_edit->text();
  if(dir.right(1)!="/") {
    dir+="/";
  }
  return dir+basename+"."+ext;
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv);
  
  //
  // Start Event Loop
  //
  MainWidget *w=new MainWidget(NULL);
  //  a.setMainWidget(w);
  w->show();
  return a.exec();
}
