!#/bin/bash
rm bin/testcam
rm log.o
rm fireball_camera.o
make -f Makefile.m testcam
./bin/testcam
