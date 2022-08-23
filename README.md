# SimplexMotion driver

Status: Tested with one and two motors, with threaded computer-side PD torque controllers

This package implements control interfaces for SimplexMotion servo
motors. It was developed on a standard Linux (Ubuntu) and tested for
SH100B, but some interface implementations should be crossplattform
(in particular via the hdiapi library). For Linux, there are 4
alternative communication options:
* via hidraw, using the direct kernel modul (based on https://github.com/torvalds/linux/blob/master/samples/hidraw/hid-example.c)
* via the hidapi library, https://github.com/libusb/hidapi, crossplatform, on Linux largely equivalent to the above
* using the modbus protocol over serial (not yet implemented)
* using the modbus protocol over CAN (not yet implemented)

The `SimplexMotion` class provides high-level methods (e.g. `setPID`
or `runSpeed` or running a torque-control loop with your own
torque-computation callback). The `SimplexMotion_Communication` class
provides direct write/read access to all SimplexMotion registers, but
still agnostic to the choice of communication/implementation.

## Related

* SimplexMotion manual (including register documentation): https://www.simplexmotion.com/documentation/manual/SimplexMotionManual.pdf
* SimplexMotion servo motors: https://simplexmotion.com/products/simplex-motion-servomotors/
* Python package using the Modbus RTU protocol: https://github.com/srikanthsrnvs/pysimplex

## Installation

* standard cmake build:
```
git clone https://github.com/MarcToussaint/SimplexMotion-driver.git

cd SimplexMotion-driver
mkdir build
cd build
# by default, the hdiapi is compiled call `ccmake ..` to switch to another implementation

cmake ..
make
```

* Plug in the motor via USB, check `dmesg`, device paths need general write permission
```
sudo chmod a+rw /dev/hidraw*
```

## Testing & Calibration

The `test/test.cpp` implements basic testing of the motors, including simple methods to calibrate cogging, rund constant speed, run direct torque controllers, run the threaded computer-side torque controller. Please look into the source code.

A basic test (running small constant speed) is
```
./simplexMotionTest /dev/hidraw0
```

Cogging calibration is
```
./simplexMotionTest /dev/hidraw0 cogging
```

To test threads and multi-motors, change the source directly.
