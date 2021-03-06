//20151203 MM: Based on StaticDIAOv5 by JG
// -- took out digital in (DI) for now, kept analog out (AO).
// -- broke single function up into initialization, destruction, and writes
// -- we may need digital out (DO) or TTL (if available) in the future for lamps/shutters

// necessary includes
#include <cstdlib>
#include <cstdio>


// this library should be moved somewhere a little less hidden
#include "/home/salo/xJG/DAC/DACtest/examples/C++_Console/inc/compatibility.h"
#include "/home/salo/xJG/DAC/DACtest/inc/bdaqctrl.h"

#include "staticAO.h"

// variable declarations
int32_t startPort = 0;
int32_t portCount = 1;

int32_t channelStart = 1;
int32_t channelCount = 1;

int AOportopen = 0;

InstantAoCtrl *instantAoCtrl;

// offsets. Sometimes, when we ask for 0, we don't get 0; then we need
// to correct

const float volt_offset[2] = {0.00, 0.00};



 
// opens the port
int OpenAOPort(){
  static  ErrorCode ret;
  // Create a 'InstantAoCtrl' for Static AO function.
  instantAoCtrl = AdxInstantAoCtrlCreate();
  
  // Select a device by device number or device description and specify access mode.
  
  DeviceInformation devInfo(deviceDescription);
  ret = instantAoCtrl -> setSelectedDevice(devInfo);
  //  CHK_RESULT(ret);
  // ok, now we want to set a feature (namely, the voltage range)

  ret = instantAoCtrl->getChannels()->getItem(0).setValueRange(VRANGE);
  //  CHK_RESULT(ret);
  ret = instantAoCtrl->getChannels()->getItem(1).setValueRange(VRANGE);
  //  CHK_RESULT(ret);

  AOportopen=1;
#ifdef DEBUG
  printf("Opening port. %d, ret=%d\n",AOportopen,ret);
#endif
  
  return 0;
};

int CloseAOPort(){
  instantAoCtrl->Dispose();
  AOportopen=0;
  #ifdef DEBUG
  printf("Closing port. %d\n",AOportopen);
  #endif
  return 0;
};


// write to ports (raw)                                
int AOWriteBoth(double cevolt, double elvolt){
  static double volts[2];
  if (AOportopen) { 
    // range check
    volts[0]=cevolt+volt_offset[0]; 
    volts[1]=elvolt+volt_offset[1];
    if (volts[0] < MINV) volts[0] = MINV;
    if (volts[0] > MAXV) volts[0] = MAXV;
    
    if (volts[1] < MINV) volts[1] = MINV;
    if (volts[1] > MAXV) volts[1] = MAXV;
    
    
    // output to ports
    instantAoCtrl->Write(0,2,volts);
    //CHK_RESULT(ret);
    
#ifdef DEBUG
    printf("AOWriteBoth outputted: %lf %lf\n",volts[0],volts[1]);
#endif

    // exit
    return 0;
  } else {
#ifdef DEBUG
    printf("Port was not opened!\n");
#endif
    return -2;
  };
};
  
int AOWrite(int port, double value){
  static double volt;
  if (AOportopen) { 
    // if invalid port, exit immediately, without changing stuff
    if (port <0 || port > 1) return -1;
    volt = value+volt_offset[port];
    // range checks -- if the port is specified incorrectly, do nothing
    if (volt < MINV) volt = MINV;
    if (volt > MAXV) volt = MAXV;
    
    instantAoCtrl->Write(port,1,&volt);
    //    CHK_RESULT(ret);
#ifdef DEBUG
    printf("AOWrite outputted: %lf\n",volt);
#endif
    return 0;
  } else {
#ifdef DEBUG
    printf("Port was not opened!\n");
#endif
    return -2;
  };
};

// write to ports (arcseconds), just a wrapper
int AOWriteArcsec(float cearcsec, float elarcsec){
  return AOWriteBoth(cearcsec*ARCSECTOV, elarcsec*ARCSECTOV);
};

int AOWriteArcsecPort(int port, float arcsec){
  return AOWrite(port, arcsec*ARCSECTOV);
};

int AOWriteArcsecEL(float arcsec){
  return AOWrite(ELCHANNEL,arcsec*ARCSECTOV);
};

int AOWriteArcsecCE(float arcsec){
  return AOWrite(CECHANNEL,arcsec*ARCSECTOV);
};
