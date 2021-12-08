#include "SimplexMotion-com.h"

#include <linux/types.h>
#include <linux/input.h>
#include <linux/hidraw.h>

#ifndef HIDIOCSFEATURE
#warning Please have your distro update the userspace kernel headers
#define HIDIOCSFEATURE(len)    _IOC(_IOC_WRITE|_IOC_READ, 'H', 0x06, len)
#define HIDIOCGFEATURE(len)    _IOC(_IOC_WRITE|_IOC_READ, 'H', 0x07, len)
#endif

#include <fcntl.h>
#include <unistd.h>

#include <string.h>
#include <errno.h>
#include <iostream>

#define S1(x) #x
#define S2(x) S1(x)
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define LOG cout <<"# " <<__FILENAME__ <<":" <<S2(__LINE__) <<": "

using std::cerr;
using std::cout;
using std::endl;

struct SimplexMotion_Communication_Self{
  int fd;
};

SimplexMotion_Communication::SimplexMotion_Communication(const char* devPath,unsigned short vendor_id, unsigned short product_id){
  self = new SimplexMotion_Communication_Self;

  self->fd = open(devPath, O_RDWR); //|O_NONBLOCK);

  if(self->fd<0) {
    cerr <<"Unable to open device " <<devPath <<endl;
    return;
  }

  hidraw_report_descriptor rpt_desc;
  hidraw_devinfo info;
  memset(&rpt_desc, 0x0, sizeof(rpt_desc));
  memset(&info, 0x0, sizeof(info));
  memset(buf, 0x0, sizeof(buf));

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
  res = ioctl(self->fd, HIDIOCGRAWPHYS(256), buf);
  if(res<0) cerr <<"HIDIOCGRAWPHYS error" <<endl;
  else cout <<"Raw Phys: " <<buf <<endl;
#endif

  /* Get Raw Name */
  int res = ioctl(self->fd, HIDIOCGRAWNAME(256), buf);
  if(res<0) cerr <<"HIDIOCGRAWNAME error" <<endl;
  else LOG <<"Raw Name: " <<buf <<endl;

  /* Get Raw Info */
  res = ioctl(self->fd, HIDIOCGRAWINFO, &info);
  if(res<0) cerr <<"HIDIOCGRAWINFO error" <<endl;
  else{
    LOG <<"Raw Info: bustype: " <<info.bustype <<" (";
    if(info.bustype==BUS_USB) cout <<"USB";
    else if(info.bustype==BUS_HIL) cout <<"HIL";
    else if(info.bustype==BUS_BLUETOOTH) cout <<"Bluetooth";
    else if(info.bustype==BUS_VIRTUAL) cout <<"Virtual";
    else cout <<"Other";
    cout <<") vendor: 0x" <<std::hex <<info.vendor;
    cout <<" product: 0x" <<std::hex <<info.product <<endl;
  }
}

SimplexMotion_Communication::~SimplexMotion_Communication(){
  close(self->fd);
  delete self;
}

bool SimplexMotion_Communication::writeBuf(int len){
  int res = ::write(self->fd, buf, len);
  if(res!=len){
    cerr <<"write error: " <<errno <<" written bytes:" <<res <<" wanted bytes:" <<len <<endl;
    return false;
  }
  return true;
}

bool SimplexMotion_Communication::readBuf(int len){
  int res = ::read(self->fd, buf, len);
  if(res!=len){
    cerr <<"read error: " <<errno <<" read bytes:" <<res <<" wanted bytes:" <<len <<endl;
    return false;
  }
  return true;
}

