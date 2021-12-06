#include "SimplexMotion.h"
#include "SimplexMotion-com.h"

#include <stdio.h>
#include <wchar.h>

#include <hidapi/hidapi.h>
#include <iostream>
#include <iomanip>

using std::cerr;
using std::cout;
using std::endl;

struct SimplexMotion_Communication_Self{
  unsigned char buf[65];
  hid_device *handle;
  int i;
};

SimplexMotion_Communication::SimplexMotion_Communication(const char* devPath){
  self = new SimplexMotion_Communication_Self;

  // Initialize the hidapi library
  int res = hid_init();

  // Open the device using the VID, PID,
  // and optionally the Serial number.
  self->handle = hid_open(0x04d8, 0xf79a, NULL);

  // Read the Manufacturer String
  wchar_t wstr[256];
  res = hid_get_manufacturer_string(self->handle, wstr, 256);
//	wprintf(L"Manufacturer String: %s\n", wstr);

  // Read the Product String
  res = hid_get_product_string(self->handle, wstr, 256);
//	wprintf(L"Product String: %s\n", wstr);

  // Read the Serial Number String
  res = hid_get_serial_number_string(self->handle, wstr, 256);
//	wprintf(L"Serial Number String: (%d) %s\n", wstr[0], wstr);

  // Read Indexed String 1
  res = hid_get_indexed_string(self->handle, 1, wstr, 256);
//	wprintf(L"Indexed String 1: %s\n", wstr);
}

SimplexMotion_Communication::~SimplexMotion_Communication(){
  // Close the device
  hid_close(self->handle);

  // Finalize the hidapi library
  hid_exit();

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

  int res = hid_write(self->handle, self->buf, buflen);
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

  int res = hid_write(self->handle, self->buf, 4);
  if(res!=4){
    cerr <<"write error: " <<errno <<" written bytes:" <<res <<" wanted bytes:" <<4 <<endl;
    return 0;
  }

  res = hid_read(self->handle, self->buf, readlen);
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
