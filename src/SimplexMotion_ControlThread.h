#pragma once

#include "SimplexMotion.h"

#include <thread>
#include <mutex>

struct SimplexMotion_ControlThread{
  struct CtrlCmd{   double q_ref=0., qDot_ref=0., Kp=0., Kd=0., u_b=0.; };
  struct CtrlState{ double ctrlTime=0., q, qDot, u; };

  SimplexMotion_ControlThread(const char* devPath = "/dev/hidraw0", unsigned short vendor_id = 0x04d8, unsigned short product_id = 0xf79a);
  ~SimplexMotion_ControlThread();

  void loop();

  void setLogFile(const char* filename);
  void setCmd(CtrlCmd _cmd);
  double getPosition();
  double getCtrlTime();

private:
  SimplexMotion M;
  std::thread th;
  double tau=.01;

  //mutexed ctrl cmd and state
  std::mutex mx;
  std::ofstream *fil=0;
  bool stop=false;
  CtrlCmd cmd;
  CtrlState state;
};
