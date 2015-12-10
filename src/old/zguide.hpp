//Includes for zguide

#ifndef rubble_mint
#define rubble_mint

//Specific includes
//We need to read general parameters
#include "parameters.hpp"
//We need to take images with the camera
#include "camera.hpp"
//We may need to save the images in FITS
#include "fitswrapper.hpp"
//We may need to record messages on logs
#include "log.hpp"
//WE need to identify sources for centroiding
#include "frameblob.h"

//General includes
//We need to cout messages
#include <iostream>
//And do some math
#include <math.h>

//Includes needed for old functionalities
//#include "sun.hpp"
//#include "gps.hpp"
//#include <cstdlib>
//#include <stdio.h>
//#include <cmath>
//#include <time.h>
//#include <stdint.h>
//#include <stdlib.h>
//#include <signal.h>
//#include <termios.h>

//Function declarations

//void terminate (int signum);
void ImageInvert(uint16_t *pImage, int ImSize, uint16_t MaxValue);
void ImageWindow(uint16_t *pImage, uint16_t *pWindow, int nRows, int nCols, double x, double y, int sizeX, int sizeY, int origin_x, int origin_y);
void ImageWindow(uint16_t *pImage, uint16_t *pWindow, int nRows, int nCols, int sizeX, int sizeY, int origin_x, int origin_y);
void ImageWindow(uint16_t *pImage, uint16_t *pWindow, int nRows, int nCols, int margin);
void ZeroWindow(uint16_t *pWindow, int nSize);
void SaveFITS(uint16_t * p_img, const int& CamCols, const int& CamRows);
void Centroid(uint16_t * win, const int & windowSize, const uint16_t & CamSat, double & x_centroid, double & y_centroid);

#endif
