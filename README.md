# SimplexMotion driver

IN HEAVY DEVELOPMENT

Notes:
* For Linux, there are 4 alternative connection implementations
  * via hidraw, using the direct kernel modul (based on https://github.com/torvalds/linux/blob/master/samples/hidraw/hid-example.c)
  * via the hidapi library, https://github.com/libusb/hidapi, crossplatform, on Linux largely equivalent to the above
  * using the modbus protocol over serial (not yet tested)
  * using the modbus protocol over CAN (not yet tested
  
* We develop/test this for the SH100B
* https://simplexmotion.com/products/simplex-motion-servomotors/sh-serie/

## Installation

* standard cmake build

* Plug in the motor via USB, check `dmesg`, device paths need general write permission
```
sudo chmod a+rw /dev/hidraw*
```

* test the `simplexMotionTest` (check source at src/test.cpp first)


