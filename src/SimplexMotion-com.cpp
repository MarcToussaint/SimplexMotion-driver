#include "SimplexMotion-com.h"

#include <cstdint>
#include <iostream>

#define S1(x) #x
#define S2(x) S1(x)
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define LOG cout <<"# " <<__FILENAME__ <<":" <<S2(__LINE__) <<": "

using std::cerr;
using std::cout;
using std::endl;

void SimplexMotion_Communication::writeRegister(int regNumber, RegType regType, int value){
  int buflen=-1;
  if(regType==uns16){
    uint16_t data = value;
    buf[0] = 0x21; //transfer mode ('report number')
    buf[4] = data; //LOWEST BYTE FIRST!!! &0xff is implicit
    buf[5] = data>>8;
    buflen=6;
  }else if(regType==int16){
    int16_t data = value;
    buf[0] = 0x21;
    buf[4] = data;
    buf[5] = data>>8;
    buflen=6;
  }else if(regType==uns32){
    uint32_t data = value;
    buf[0] = 0x22;
    buf[4] = data;
    buf[5] = data>>8;
    buf[6] = data>>16;
    buf[7] = data>>24;
    buflen=8;
  }else if(regType==int32){
    int32_t data = value;
    buf[0] = 0x22;
    buf[4] = data;
    buf[5] = data>>8;
    buf[6] = data>>16;
    buf[7] = data>>24;
    buflen=8;
  }else{
    cerr <<"can't write string" <<endl; return;
  }
  buf[1] = 1;
  buf[2] = regNumber&0xff;
  buf[3] = regNumber>>8;

  writeBuf(buflen);
}

int SimplexMotion_Communication::readRegister(int regNumber, RegType regType){
  int readlen=-1;
  if(regType==uns16 || regType==int16){
    buf[0] = 0x11; //transfer mode ('report number')
    readlen=2;
  }else if(regType==uns32 || regType==int32){
    buf[0] = 0x12;
    readlen=4;
  }else{
    cerr <<"can't read string" <<endl;
    return 0;
  }
  buf[1] = 1;
  buf[2] = regNumber&0xff;
  buf[3] = regNumber>>8;

  if(!writeBuf(4)) return 0;

  if(!readBuf(readlen)) return 0;

  if(regType==uns16){
    uint16_t data;
    data = buf[1];
    data = (data<<8) | buf[0];
    return data;
  }else if(regType==int16){
    int16_t data;
    data = buf[1];
    data = (data<<8) | buf[0];
    return data;
  }else if(regType==uns32){
    uint32_t data;
    data = buf[3];
    data = (data<<8) | buf[2];
    data = (data<<8) | buf[1];
    data = (data<<8) | buf[0];
    return data;
  }else if(regType==int32){
    int32_t data;
    data = buf[3];
    data = (data<<8) | buf[2];
    data = (data<<8) | buf[1];
    data = (data<<8) | buf[0];
    return data;
  }
  return 0;
}

const char* SimplexMotion_Communication::readString(int regNumber, int n){
  int numReg = n/2 + (n&1);
  buf[0] = 0x11;
  buf[1] = numReg;
  buf[2] = regNumber&0xff;
  buf[3] = regNumber>>8;

  if(!writeBuf(4)) return 0;

  if(!readBuf(2*numReg)) return 0;

  buf[n]=0;
  return (char*)buf;
}
