#pragma once

#define RAI_2PI 6.283195307179587

//===========================================================================

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

//===========================================================================

//user interface
struct SimplexMotion{
  SimplexMotion_Communication com;

  SimplexMotion(const char* devPath = "/dev/hidraw0");

  double getVoltage();
  double getMotorTemperature();
  double getMotorPosition();
  double getMotorSpeed();
  double getMotorTorque();

  void setSpeedFiltering(int filter);

  //using the ramp & PID regulator that maps pos/vel to torques
  void setPID(int Kp, int Ki, int Kd, int KiLimit, int KdDelay, int Friction);
  void setRamps(double maxSpeed, double maxAcc);
  void setIntertia(int mass);
  double getPIDoutput(); //torque reference

  void setMode(int mode);
  void setTarget(int target);

  void runOff(){ setMode(0); }
  void runStop(){ setMode(5); }
  void runCoggingCalibration(){ setMode(110); }
  void runSpeed(double speed){ setTarget(256.*speed/RAI_2PI); setMode(33); } //using ramp & PID; activates SpeedRamp control mode
  void runPosition(double position){ setTarget(4096.*position/RAI_2PI); setMode(21); } //using ramps & PIDactivates PositionRamp control mode
  void runTorque(double torque){ setTarget(1000.*torque); setMode(40); }
};

//===========================================================================
