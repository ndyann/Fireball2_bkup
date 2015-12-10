-------------------------------------------------------------------
                 P C O  AG
     TECHNICAL  INFORMATION  DOCUMENT
-------------------------------------------------------------------

README FOR SOFTWARE:  
pco_me4

FOR PCO PRODUCT:
pco.edge or pco.camera connected to me4 grabber

DESCRIPTION:
This packages includes a communication anfd grabber classes and 
simple testprograms, which show the use of the class functions.

VERSION 1
SUBVERSION 1
SUBAGE 3
LNX 32

CONTENTS:

Archive File Name: PCO_me4_x32_1_01_xx.tar.gz

Installation Instructions:

Install driver and  runtime from Silicon Software and check functionality of me4 board.

Copy the pco_me4_x32_1_01_xx.tar.gz to a distinct directory (e.g. PCO). 
Use "tar -xpvzf pco__me4_1_01_xx.tar.gz" to get the sources.
After this you will find the following new directories and files as noted below.

Use "make" to compile all testprograms
If pco.edge GlobalShutter camera should be used copy 
the pco-hap file from ./PCO_me4/pco_hap to /opt/siso/Hardware Applets/mE4VD4-CL/ directory.


./PCO_me4/PCO_Include
PCO header files

./PCO_me4/siso_Include
SiliconSoftware header files from Runtime5

./PCO_me4/siso_lib
SiliconSoftware library files from Runtime5

./PCO_me4/pco_classes
Source code and header files for Class Cpco_cl_com
Source code and header files for Class Cpco_cl_me4
Source code and header files for Class Cpco_cl_me4_GS

./PCO_me4/pco_lib
PCO library files

./PCO_me4/pco_hap
hap-file for pco_edge Global shutter camera

./PCO_me4/pco_ser_con
Source files and makefile for creating console testprogram
Program for testing the serial communication

./PCO_me4/pco_edge_grab
Source files and makefile for creating console testprograms
Programs for grabbing images from an pco.edge Rolling Shutter camera
pcox_   shows grabbed images in an X-Window
 
./PCO_me4/pco_edge_grab_gs
Source files and makefile for creating console testprograms
Programs for grabbing images from an pco.edge Global Shutter camera
pcox_   shows grabbed images in an X-Window

./PCO_me4/pco_camera_grab
Source files and makefile for creating console testprograms
Programs for grabbing images from an pco.camera camera

./PCO_me4/bin
executable binaries


SYSTEM_VERSION:
This package was sucessfully tested in a Debian 5.0 system with kernel 2.6.26
with the following driver and runtime versions from Silicon Software
 menable_linuxdrv_src_3.9.14_4.0.3_r33421
 siso-rt5-5.1.2-1.i586



VERSION HISTORY:

ver01_01_03
with makefiles
added support for pco.edge GlobalShutter
new headerfiles in PCO_Include
new pco libraries


ver01_01_02
new example and new functions in class CPco_cl_com

ver01_01_01
initial project


KNOWN BUGS:

-------------------------------------------------------------------
 PCO AG
 DONAUPARK 11
 93309 KELHEIM / GERMANY
 PHONE +49 (9441) 20050
 FAX   +49 (9441) 200520
 info@pco.de, support@pco.de
 http://www.pco.de
-------------------------------------------------------------------
 DISCLAIMER
 THE ORIGIN OF THIS INFORMATION MAY BE INTERNAL OR EXTERNAL TO PCO.
 PCO MAKES EVERY EFFORT WITHIN ITS MEANS TO VERIFY THIS INFORMATION.
 HOWEVER, THE INFORMATION PROVIDED IN THIS DOCUMENT IS FOR YOUR
 INFORMATION ONLY. PCO MAKES NO EXPLICIT OR IMPLIED CLAIMS TO THE
 VALIDITY OF THIS INFORMATION.
-------------------------------------------------------------------
 Any trademarks referenced in this document are the property of
 their respective owners.
-------------------------------------------------------------------
