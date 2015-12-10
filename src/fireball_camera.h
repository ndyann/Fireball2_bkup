#ifndef FIREBALL_CAMERA_H_
#define FIREBALL_CAMERA_H_

// i think this should be a class that wraps the MINT stuff for fireball

#include <stdint.h>
#include "VersionNo.h"
#include "pco_cl_include.h"
#include "Cpco_cl_com.h"
#include "Cpco_me4.h"
#include "file12.h"
#include "../siso_include/clser.h"



#include <string>
#include <cstdio>
//#include "camera.hpp"

#define FATAL 0xF0000000
#define MAX_RETRY 10
#define WARNING 0x20000000
#define OK 0x00000000

//CPco_Log pco_log("pco_edge_grab.log");

// Structure with parameters we want to change
typedef struct camera_parameters_t{
  
  uint16_t act_recstate;    // 1==Recording, 0==Non recording
  int loopcount;            // Number of images to acquire
  uint32_t width;           // Image width in pixels
  uint32_t height;          // Image height in pixels
  uint32_t exp;             // Exposure time in ms
  uint32_t delay;           // Delay time in ms
  int timeout;              // Time for grabber to wait for image
  uint32_t pixelrate;       // Pixel rate
  uint16_t *picbuf;

};

uint32_t PCO_GetBinning(CPco_cl_com *cam, uint16_t *binningx, uint16_t *binningy);
uint32_t PCO_SetBinning(CPco_cl_com *cam, uint16_t binningx, uint16_t binningy);
uint32_t PCO_SetROI(CPco_cl_com *cam, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);



class fireball_camera { 
 private:
  CPco_cl_com camera;
  CPco_me4_edge grabber;
  PCO_SC2_CL_TRANSFER_PARAM clpar;
  // status bits
  char camera_open;
  char camera_acquiring;
  int picnum;
  int timeout;
  uint16_t *adr;
  uint32_t err;
 public:
  camera_parameters_t camera_parameters;
  // whatever is required to open the camera and grabber (constructor)
  fireball_camera();
  void fireball_camera_cam();
  
  // whatever is requried to close the camera and grabber(destructor)
  ~fireball_camera();

  // set parameters
  //int set_params(camera_parameters_t *campar);
  int set_params();
  
  // start and stop acquisition
  int start();
  int stop();

  int grab_single();
  int temperature();

  // whatever other functions one may want. 
};


#endif // #ifndef FIREBALL_CAMERA_H
