#pragma once

//low-level communication interface, for which there are multiple implementations (hidraw, hidapi, modbus)
struct SimplexMotion_Communication{
  enum RegType { uns16, uns32, int16, int32, string };

  SimplexMotion_Communication(const char* devPath = "/dev/hidraw0");
  ~SimplexMotion_Communication();

  void writeRegister(int regNumber, RegType regType, int data);
  int readRegister(int regNumber, RegType regType);
  void readString(int regNumber, int n, char* str);

private:
  struct SimplexMotion_Communication_Self* self=0;
};

