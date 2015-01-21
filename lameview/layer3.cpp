// layer3.cpp
//
// MPEG Layer 3 encoding routines 
//
//   (C) Copyright 2009 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: layer3.cpp,v 1.2 2010/01/05 17:08:55 cvs Exp $
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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include <sndfile.h>
#include <samplerate.h>
#ifdef LAME
#include <lame/lame.h>
#endif  // LAME

#include <lameview.h>

bool MainWidget::EncodeLayer3(const QString &srcfile,MainWidget::Mode mode,
			      unsigned bitrate,unsigned samprate,
			      unsigned srcconv)
{
#ifdef LAME
  SNDFILE *sf=NULL;
  SF_INFO sfinfo;
  SRC_STATE *srcstate=NULL;
  SRC_DATA sd;
  int dst_fd=0;
  lame_global_flags *lameopts=NULL;
  sf_count_t n;
  int s;
  int err;
  float *pcm=NULL;
  float pcm1[32768];
  float pcm2[32768];
  short pcmout[32768];
  unsigned char mpeg[2048];
  int count=0;
  int step=0;
  int stepsize=0;
  QStringList srcpath;

  //
  // Open Source File
  //
  memset(&sfinfo,0,sizeof(sfinfo));
  if((sf=sf_open(srcfile.toAscii(),SFM_READ,&sfinfo))==NULL) {
    QMessageBox::warning(this,tr("LameView - Input File Error"),
			 tr("Unable to open file \"")+srcfile+"\"!");
    return false;
  }

  //
  // Open Destination File
  //
  if((dst_fd=open(MakeDestinationName(srcfile,LAMEVIEW_LAYER3_EXTENSION).
		  toAscii(),O_WRONLY|O_CREAT|O_TRUNC,
		  S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH))<0) {
    QMessageBox::warning(this,tr("LameView - Output File Error"),strerror(errno));
    sf_close(sf);
    return false;
  } 

  //
  // Determine sample rates and (perhaps) init a converter
  //
  if(samprate==0) {  // Calculate optimum output rate
    if(sfinfo.samplerate<=32000) {
      samprate=32000;
    }
    else {
      if(sfinfo.samplerate<=44100) {
	samprate=44100;
      }
      else {
	samprate=48000;
      }
    }
  }
  if((int)samprate!=sfinfo.samplerate) {  // Converter needed
    if((srcstate=src_new(srcconv,sfinfo.channels,&err))==NULL) {
      QMessageBox::warning(this,tr("LameView - SRC Error"),
			   src_strerror(err));
      ::close(dst_fd);
      sf_close(sf);
      return false;
    }
    memset(&sd,0,sizeof(sd));
    sd.data_in=pcm1;
    sd.data_out=pcm2;
    sd.output_frames=32768/sfinfo.channels;
    sd.src_ratio=(double)samprate/(double)sfinfo.samplerate;
    pcm=pcm2;
  }
  else {
    pcm=pcm1;
  }

  //
  // Generate Encoder
  //
  if((lameopts=lame_init())==NULL) {
    QMessageBox::warning(this,tr("LameView - Encoder Error"),
			 tr("Unable to initialize Lame encoder!"));
    if(srcstate!=NULL) {
      src_delete(srcstate);
    }
    ::close(dst_fd);
    sf_close(sf);
    return false;
  }
  lame_set_mode(lameopts,(MPEG_mode)mode);
  lame_set_num_channels(lameopts,sfinfo.channels);
  lame_set_in_samplerate(lameopts,samprate);
  lame_set_out_samplerate(lameopts,samprate);
  lame_set_brate(lameopts,bitrate);
  if(lame_init_params(lameopts)!=0) {
    QMessageBox::warning(this,tr("LameView - Encoder Error"),
			 tr("Invalid Lame parameters!"));
    if(srcstate!=NULL) {
      src_delete(srcstate);
    }
    lame_close(lameopts);
    ::close(dst_fd);
    sf_close(sf);
    return false;
  }

  //
  // Encode
  //
  stepsize=sfinfo.frames/10;
  view_filebar->setRange(0,10);
  view_filebar->setValue(0);
  srcpath=srcfile.split("/");
  view_filebar_label->
    setText(tr("File Progress")+" - "+srcpath[srcpath.size()-1]);
  while((n=sf_readf_float(sf,pcm1,1152))>0) {
    if(srcstate!=NULL) {
      sd.input_frames=n;
      if(n<1152) {
	sd.end_of_input=1;
      }
      else {
	sd.end_of_input=0;
      }
      if((err=src_process(srcstate,&sd))!=0) {
	fprintf(stderr,"SRC Error: %s\n",src_strerror(err));
      }
      n=sd.output_frames_gen;
    }
    src_float_to_short_array(pcm,pcmout,n*sfinfo.channels);
    if((s=lame_encode_buffer_interleaved(lameopts,pcmout,n,mpeg,2048))>=0) {
      write(dst_fd,mpeg,s);
    }
    else {
      fprintf(stderr,"Lame encode error\n");
    }
    count+=n;
    if(count>stepsize) {
      count=0;
      view_filebar->setValue(++step);
      qApp->processEvents();
    }
    if(view_aborting) {
      if(srcstate!=NULL) {
	src_delete(srcstate);
      }
      lame_close(lameopts);
      ::close(dst_fd);
      sf_close(sf);
      return false;
    }
  }
  if((s=lame_encode_flush(lameopts,mpeg,2048))>=0) {
      write(dst_fd,mpeg,s);
  }
  else {
    fprintf(stderr,"Lame encode error\n");
  }
  view_filebar_label->setText(tr("File Progress"));
  view_filebar->reset();

  //
  // Clean Up
  //
  if(srcstate!=NULL) {
    src_delete(srcstate);
  }
  lame_close(lameopts);
  ::close(dst_fd);
  sf_close(sf);
  return true;
#else
  return false;
#endif
}
