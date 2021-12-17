#include "SimplexMotion-com.h"

#include <string.h>
#include <hidapi/hidapi.h>
#include <iostream>

#define S1(x) #x
#define S2(x) S1(x)
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define LOG std::cout <<"# " <<__FILENAME__ <<":" <<S2(__LINE__) <<": "

struct SimplexMotion_Communication_Self{
  hid_device *handle;
};

SimplexMotion_Communication::SimplexMotion_Communication(const char* devPath, unsigned short vendor_id, unsigned short product_id){
  self = new SimplexMotion_Communication_Self;

  // Initialize the hidapi library
  int res = hid_init();
  if(res<0){
    LOG <<"hid init error" <<std::endl;
    exit(0);
  }

  // Open the device using the VID, PID,
  // and optionally the Serial number.
  if(devPath){
    self->handle = hid_open_path(devPath);
  }else{
    self->handle = hid_open(vendor_id, product_id, NULL);
  }
  if(!self->handle){
    LOG <<"hid open error" <<std::endl;
    exit(0);
  }

  // Read the Manufacturer String
  wchar_t wstr[64];
  res = hid_get_manufacturer_string(self->handle, wstr, 64);
  if(res<0) LOG <<"error: " <<hid_error(self->handle) <<std::endl;
  wcstombs((char*)buf, wstr, 64);
  LOG <<"Manufacturer String: '" <<buf <<"'" <<std::endl;

  // Read the Product String
  res = hid_get_product_string(self->handle, wstr, 64);
  if(res<0) LOG <<"error: " <<hid_error(self->handle) <<std::endl;
  wcstombs((char*)buf, wstr, 64);
  LOG <<"Product String: '" <<buf <<"'" <<std::endl;

  // Read the Serial Number String
  res = hid_get_serial_number_string(self->handle, wstr, 64);
  if(res<0) LOG <<"error: " <<hid_error(self->handle) <<std::endl;
  wcstombs((char*)buf, wstr, 64);
  LOG <<"Serial Number: '" <<buf <<"'" <<std::endl;

  // Read Indexed String 1
//  res = hid_get_indexed_string(self->handle, 1, wstr, 64);
//  if(res<0) LOG  <<"error: " <<hid_error(self->handle);
//  LOG <<"Indexed String: '" <<wstr <<"'" <<std::endl;
}

SimplexMotion_Communication::~SimplexMotion_Communication(){
  // Close the device
  hid_close(self->handle);

  // Finalize the hidapi library
  hid_exit();

  delete self;
}

bool SimplexMotion_Communication::writeBuf(int len){
  int res = hid_write(self->handle, buf, len);
  if(res!=len){
    LOG <<"write error: " <<errno <<" written bytes:" <<res <<" wanted bytes:" <<len <<std::endl;
    return false;
  }
  return true;
}

bool SimplexMotion_Communication::readBuf(int len){
  int res = hid_read(self->handle, buf, len);
  if(res!=len){
    LOG <<"read error: " <<errno <<" read bytes:" <<res <<" wanted bytes:" <<len <<std::endl;
    return false;
  }
  return true;
}

