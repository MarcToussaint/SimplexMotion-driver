#include "../src/SimplexMotion.h"
#include "../src/SimplexMotion_ControlThread.h"

#include <unistd.h>
#include <iostream>
#include <string.h>

using std::cout;
using std::endl;

void miniTest(const char* devPath, int argc, char **argv){
  SimplexMotion M(devPath);

  cout <<"model name: '" <<M.getModelName() <<"'" <<endl;
  cout <<"serial number: '" <<M.getSerialNumber() <<"'" <<endl;
  cout <<"address: '" <<M.getAddress() <<"'" <<endl;
  cout <<"motor temperature: " <<M.getMotorTemperature() <<endl;
  cout <<"voltage: " <<M.getVoltage() <<endl;

  //-- calibrate motor cogging
  if(!strcmp(argv[argc-1], "cogging")){
    cout <<"Cogging Calibration... " <<endl;
    M.runCoggingCalibration();
    return; //motor continuous until done
  }

  //-- set speed or torque command
  if(!strcmp(argv[argc-1], "torque")){
    cout <<"Constant Torque... " <<endl;
    M.runTorque(.02);
  }else{
    cout <<"Constant Speed... " <<endl;
    M.setPID(300, 0, 200, 100, 2, 0); //low gain...
    M.runSpeed(1.);
  }

  //-- wait 1sec
  for(uint t=0;t<100;t++){
    cout <<t <<" pos:" <<M.getMotorPosition() <<" vel:" <<M.getMotorSpeed() <<endl;
    usleep(10000);
  }

  //-- stop
  M.runStop();
  usleep(500000);
  M.runOff();
}

void threadTest(const char* devPath){
  SimplexMotion_ControlThread M(devPath);

  double q0 = M.getPosition();

  M.setCmd({q0, 0., .05, 0.002, 0.});

  sleep(5);
}

void threadMultiple(const char* devPath1, const char* devPath2){
  SimplexMotion_ControlThread M1(devPath1);
  SimplexMotion_ControlThread M2(devPath2);

  M2.setCtrlTime( M1.getCtrlTime() );

  M1.setLogFile("M1.dat");
  M2.setLogFile("M2.dat");

  double q0 = M1.getPosition();
  q0 += 1.;
  M1.setCmd({q0, 0., .05, 0.002, 0.});

  q0 = M2.getPosition();
  q0 -= 1.;
  M2.setCmd({q0, 0., .05, 0.002, 0.});

  sleep(5);
}

int main(int argc, char **argv){

  const char* devPath = "/dev/hidraw3";
  if(argc>1) devPath = argv[1];

//  miniTest(devPath, argc, argv);
//  threadTest(devPath);
  threadMultiple("/dev/hidraw0", "/dev/hidraw1");

  cout <<"BYE BYE" <<endl;

  return 0;
}
