#ifndef IMAGE_ANALYSIS_H
#define IMAGE_ANALYSIS

#include "frameblob.h"
//#include "../../fbguider/guide_data.h"
//#include "fireball_camera2_par.h"
#include <algorithm>

//Temporary defines for testing, will be in guide_data
#define MAXSTARS  8

void acq_func(uint16_t *buf, int width, int height, double *x, double *y, double *flux, char *valid, int *nstars);

void track(uint16_t * buf, int width, int height, float *tx, float *ty, char *valid, uint16_t ***boxes, int *bhs, char *starmask, float *x, float *y);


#endif  //#ifndef IMAGE_ANALYSIS_H
