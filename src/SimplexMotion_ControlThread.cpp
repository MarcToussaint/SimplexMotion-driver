#include "SimplexMotion_ControlThread.h"
#include <iostream>
#include <fstream>

SimplexMotion_ControlThread::SimplexMotion_ControlThread(const char* devPath, unsigned short vendor_id, unsigned short product_id)
  : M(devPath, vendor_id, product_id),
    th(&SimplexMotion_ControlThread::loop, this){
  while(getCtrlTime()<.1){
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}

SimplexMotion_ControlThread::~SimplexMotion_ControlThread(){
  {
    std::lock_guard<std::mutex> lock(mx);
    stop=true;
  }
  th.join();
  if(fil) delete fil;
}

void SimplexMotion_ControlThread::loop(){
  CtrlCmd _cmd;
  CtrlState _state;

  //torque mode
  M.runReset(); //resets position counter
  M.runTorque(0.); //starts torque mode

  //setup metronome
  auto ticTime = std::chrono::high_resolution_clock::now() + std::chrono::duration<double>(0.);

  for(;;){
    //check stop
    {
      std::lock_guard<std::mutex> lock(mx);
      if(stop) break;
    }

    //wait for tic
    ticTime += std::chrono::duration<double>(tau);
    std::this_thread::sleep_until(ticTime);

    //get state
    _state.q = M.getMotorPosition();
    _state.qDot = M.getMotorSpeed();
    _state.ctrlTime += tau;

    //get cmd
    {
      std::lock_guard<std::mutex> lock(mx);
      _cmd = cmd;
    }

    //PD
    _state.u = _cmd.u_b;
    _state.u += _cmd.Kp*(_cmd.q_ref-_state.q);
    _state.u += _cmd.Kd*(_cmd.qDot_ref-_state.qDot);

    M.setTarget(_state.u/M.maxTorque*32767);

    //publish state
    {
      std::lock_guard<std::mutex> lock(mx);
      state = _state;
    }

    if(fil) (*fil) <<_state.ctrlTime <<' ' <<_state.q <<' ' <<_state.qDot <<' ' <<_state.u <<std::endl;
  }

  //stop
  M.runStop();
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  M.runOff();
}

void SimplexMotion_ControlThread::setLogFile(const char* filename){
  if(!fil) fil = new std::ofstream(filename);
}

void SimplexMotion_ControlThread::setCmd(CtrlCmd _cmd){
  std::lock_guard<std::mutex> lock(mx);
  cmd = _cmd;
}

double SimplexMotion_ControlThread::getPosition(){
  std::lock_guard<std::mutex> lock(mx);
  return state.q;
}

double SimplexMotion_ControlThread::getCtrlTime(){
  std::lock_guard<std::mutex> lock(mx);
  return state.ctrlTime;
}
