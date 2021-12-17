#include "../src/SimplexMotion.h"

#include <unistd.h>
#include <iostream>

using std::cout;
using std::endl;

int main(int argc, char **argv){

  const char* devPath = "/dev/hidraw2";
  if(argc>1) devPath = argv[1];

  SimplexMotion M(devPath);

  cout <<"model name: '" <<M.getModelName() <<"'" <<endl;
  cout <<"serial number: '" <<M.getSerialNumber() <<"'" <<endl;
  cout <<"address: '" <<M.getAddress() <<"'" <<endl;
  cout <<"motor temperature: " <<M.getMotorTemperature() <<endl;
  cout <<"voltage: " <<M.getVoltage() <<endl;

//  M.runCoggingCalibration(); return 0; //motor continuous until done

  M.setPID(200, 0, 200, 100, 2, 0);
  M.runSpeed(2.);
//  M.runPosition(0.);
//  M.runTorque(.5);

  for(uint t=0;t<200;t++){
    cout <<t <<" pos:" <<M.getMotorPosition() <<" vel:" <<M.getMotorSpeed() <<endl;
    usleep(10000);
  }

  M.runStop();

  usleep(500000);
  M.runOff();

  cout <<"BYE BYE" <<endl;

  return 0;
}
