#ifndef IMAGE_FUNCTION_H
#define IMAGE_FUNCTION_H

#include "guide_data.h"

typedef struct guide_data_t;

#define DEFAULTBOX 40
#define XY_RATIO 1.0
#define PLATESCALE 1.22

// thies is the worker function for image analysis.
void *image_function(void *guide_data);

// functions to allocate memory for boxes and set pointers.
int allocate_boxes(guide_data_t *imdat);
int free_boxes(guide_data_t *imdat);

int compute_mapping(float *x0, float *y0, float *x1, float *y1, float *weight,unsigned char *valid, char points, float *dx, float *dy, float *rot);

int compute_elce(float specrot_radians, float dx, float dy, float *el, float *ce);


void acq_func(uint16_t *buf, int width, int height, float *x, float *y, float *flux, unsigned char *valid, unsigned char *nstars);


void track(uint16_t * buf, int width, int height, float *tx, float *ty, unsigned char *valid, uint16_t ***boxes, unsigned short bhs, unsigned char *starmask, float *x, float *y);




#endif
