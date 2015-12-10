#ifndef IMAGE_ANALYSIS_H
#define IMAGE_ANALYSIS

#include "frameblob.h"
//#include "../../fbguider/guide_data.h"
//#include "fireball_camera2_par.h"
#include <algorithm>

//Temporary defines for testing, will be in guide_data
#define MAXSTARS  8

void acq_func(uint16_t *buf, int width, int height, double *x, double *y, double *flux, char *valid, int *nstars);

void track(uint16_t * buf, int width, int height, double *tx, double *ty, char *valid, uint16_t ***boxes, int *bhs, char *starmask, double *x, double *y);


#endif  //#ifndef IMAGE_ANALYSIS_H
