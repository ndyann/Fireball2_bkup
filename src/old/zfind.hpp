#ifndef rubble_mint
#define rubble_mint

// We need to cout messages
#include <iostream>

// We need to access to camera, log functions
#include "camera.hpp"
#include "log.hpp"
// We need to use specific parameters
#include "parameters.hpp"
// We need to save images as fits
#include "fitswrapper.hpp"

//General includes that were used at some point in the past
//#include <stdio.h>
//#include <time.h>
//#include <stdint.h>
//#include <stdlib.h>
//#include <signal.h>
//#include <termios.h>
//#include <cstdlib>
//#include <cmath>

//Specific includes that were used at some point in the past
//#include "sun.hpp"
//#include "gps.hpp"

//Function declarations
void ImageInvert(uint16_t *pImage, int ImSize, uint16_t MaxValue);

#endif
