#include "../src/SimplexMotion.h"

#include <iostream>
#include <chrono>
#include <thread>

using std::cout;
using std::endl;

int main(int argc, char **argv){

  SimplexMotion M("/dev/hidraw2");

  cout <<"motor temperature:" <<M.getMotorTemperature() <<std::dec <<endl;

//  M.runCoggingCalibration(); return 0; //motor continuous until done

  M.setPID(200, 0, 200, 100, 2, 0);
  M.runSpeed(10.);
//  M.runPosition(0.);
//  M.runTorque(.5);

  auto time = std::chrono::high_resolution_clock::now() + std::chrono::duration<double>(.1);

  for(uint t=0;t<200;t++){
    cout <<t <<" pos:" <<M.getMotorPosition() <<" vel:" <<M.getMotorSpeed() <<endl;
    time += std::chrono::duration<double>(.01);
    std::this_thread::sleep_until(time);
  }

  M.runStop();

  time += std::chrono::duration<double>(.5);
  std::this_thread::sleep_until(time);
  M.runOff();

  cout <<"BYE BYE" <<endl;

  return 0;
}
