#include "SimplexMotion.h"

#include <linux/types.h>
#include <linux/input.h>
#include <linux/hidraw.h>

#ifndef HIDIOCSFEATURE
#warning Please have your distro update the userspace kernel headers
#define HIDIOCSFEATURE(len)    _IOC(_IOC_WRITE|_IOC_READ, 'H', 0x06, len)
#define HIDIOCGFEATURE(len)    _IOC(_IOC_WRITE|_IOC_READ, 'H', 0x07, len)
#endif

#define S1(x) #x
#define S2(x) S1(x)
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define LOG cout <<"# " <<__FILENAME__ <<":" <<S2(__LINE__) <<": "

#include <fcntl.h>
#include <unistd.h>

#include <string.h>
#include <errno.h>
#include <iostream>
#include <iomanip>
#include <cstdint>

using std::cerr;
using std::cout;
using std::endl;

struct SimplexMotion_Communication_Self{
  int fd;
  unsigned char buf[256];
  hidraw_report_descriptor rpt_desc;
  hidraw_devinfo info;
};

SimplexMotion_Communication::SimplexMotion_Communication(const char* devPath){
  self = new SimplexMotion_Communication_Self;

  self->fd = open(devPath, O_RDWR); //|O_NONBLOCK);

  if(self->fd<0) {
    cerr <<"Unable to open device " <<devPath <<endl;
    return;
  }

  memset(&self->rpt_desc, 0x0, sizeof(self->rpt_desc));
  memset(&self->info, 0x0, sizeof(self->info));
  memset(self->buf, 0x0, sizeof(self->buf));

#if 0
  /* Get Report Descriptor Size */
  int desc_size = 0;
  int res = ioctl(self->fd, HIDIOCGRDESCSIZE, &desc_size);
  if(res<0) cerr <<"HIDIOCGRDESCSIZE error" <<endl;
  else cout <<"Report Descriptor Size: " <<desc_size <<endl;

  /* Get Report Descriptor */
  self->rpt_desc.size = desc_size;
  res = ioctl(self->fd, HIDIOCGRDESC, &self->rpt_desc);
  if(res<0) cerr <<"HIDIOCGRDESC error" <<endl;
  else{
    cout <<"Report Descriptor: ";
    for(int i=0; i<desc_size; i++) cout <<"0x" <<std::hex <<self->rpt_desc.value[i];
    cout <<endl;
  }

  /* Get Physical Location */
  res = ioctl(self->fd, HIDIOCGRAWPHYS(256), self->buf);
  if(res<0) cerr <<"HIDIOCGRAWPHYS error" <<endl;
  else cout <<"Raw Phys: " <<self->buf <<endl;
#endif

  /* Get Raw Name */
  int res = ioctl(self->fd, HIDIOCGRAWNAME(256), self->buf);
  if(res<0) cerr <<"HIDIOCGRAWNAME error" <<endl;
  else LOG <<"Raw Name: " <<self->buf <<endl;

  /* Get Raw Info */
  res = ioctl(self->fd, HIDIOCGRAWINFO, &self->info);
  if(res<0) cerr <<"HIDIOCGRAWINFO error" <<endl;
  else{
    LOG <<"Raw Info: bustype: " <<self->info.bustype <<" (";
    if(self->info.bustype==BUS_USB) cout <<"USB";
    else if(self->info.bustype==BUS_HIL) cout <<"HIL";
    else if(self->info.bustype==BUS_BLUETOOTH) cout <<"Bluetooth";
    else if(self->info.bustype==BUS_VIRTUAL) cout <<"Virtual";
    else cout <<"Other";
    cout <<") vendor: 0x" <<std::hex <<self->info.vendor;
    cout <<" product: 0x" <<std::hex <<self->info.product <<endl;
  }
}

SimplexMotion_Communication::~SimplexMotion_Communication(){
  close(self->fd);
  delete self;
}

void SimplexMotion_Communication::writeRegister(int regNumber, RegType regType, int value){
  int buflen=-1;
  if(regType==uns16){
    uint16_t data = value;
    self->buf[0] = 0x21;
    self->buf[4] = data; //LOWEST BYTE FIRST!!! &0xff is implicit
    self->buf[5] = data>>8;
    buflen=6;
  }else if(regType==int16){
    int16_t data = value;
    self->buf[0] = 0x21;
    self->buf[4] = data;
    self->buf[5] = data>>8;
    buflen=6;
  }else if(regType==uns32){
    uint32_t data = value;
    self->buf[0] = 0x22;
    self->buf[4] = data;
    self->buf[5] = data>>8;
    self->buf[6] = data>>16;
    self->buf[7] = data>>24;
    buflen=8;
  }else if(regType==int32){
    int32_t data = value;
    self->buf[0] = 0x22;
    self->buf[4] = data;
    self->buf[5] = data>>8;
    self->buf[6] = data>>16;
    self->buf[7] = data>>24;
    buflen=8;
  }else{
    cerr <<"can't write string" <<endl; return;
  }
  self->buf[1] = 1;
  self->buf[2] = regNumber&0xff;
  self->buf[3] = regNumber>>8;

  int res = write(self->fd, self->buf, buflen);
  if(res!=buflen) cerr <<"write error: " <<errno <<" written bytes:" <<res <<" wanted bytes:" <<buflen <<endl;
}

int SimplexMotion_Communication::readRegister(int regNumber, RegType regType){
  int readlen=-1;
  if(regType==uns16 || regType==int16){
    self->buf[0] = 0x11;
    readlen=2;
  }else if(regType==uns32 || regType==int32){
    self->buf[0] = 0x12;
    readlen=4;
  }else{
    cerr <<"can't read string" <<endl;
    return 0;
  }
  self->buf[1] = 1;
  self->buf[2] = regNumber&0xff;
  self->buf[3] = regNumber>>8;

  int res = write(self->fd, self->buf, 4);
  if(res!=4){
    cerr <<"write error: " <<errno <<" written bytes:" <<res <<" wanted bytes:" <<4 <<endl;
    return 0;
  }

  res = read(self->fd, self->buf, readlen);
  if(res!=readlen){
    cerr <<"read error: " <<errno <<" read bytes:" <<res <<" wanted bytes:" <<readlen <<endl;
    return 0;
  }

  if(regType==uns16){
    uint16_t data;
    data = self->buf[1];
    data = (data<<8) | self->buf[0];
    return data;
  }else if(regType==int16){
    int16_t data;
    data = self->buf[1];
    data = (data<<8) | self->buf[0];
    return data;
  }else if(regType==uns32){
    uint32_t data;
    data = self->buf[3];
    data = (data<<8) | self->buf[2];
    data = (data<<8) | self->buf[1];
    data = (data<<8) | self->buf[0];
    return data;
  }else if(regType==int32){
    int32_t data;
    data = self->buf[3];
    data = (data<<8) | self->buf[2];
    data = (data<<8) | self->buf[1];
    data = (data<<8) | self->buf[0];
    return data;
  }
  return 0;
}

void SimplexMotion_Communication::readString(int regNumber, int n, char* str){
  //not implemented yet
}
