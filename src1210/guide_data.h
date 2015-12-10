#ifndef GUIDE_DATA_H_
#define GUIDE_DATA_H_

#include "guiding_settings.h"
#include "fireball_camera2.h"
#include "message_data.h"

#include "image_function.h"

#include "display.h"

// max number of stars we can guide on
#define MAXSTARS 8

struct message_data_t;

typedef struct guide_data_t guide_data_t;
typedef struct mpf_data_t mpf_data_t;

typedef struct guide_data_t{
  camera_parameters_t *cam_data; // pointer to the camera
  guiding_settings_t *guide_settings;
  guiding_settings_t *next_guide_settings;
  unsigned char update_guide_settings;
  unsigned char update_guide_data;
  display_settings_t *disp_settings;
  guide_data_t *next_guide_data;
  mpf_data_t *mpf_data;

  char nstars;// number of stars to use <= MAXSTARS
  float lastdx, lastdy, lastdrot; 
 // target positions to use
  float tx[MAXSTARS], ty[MAXSTARS], tf[MAXSTARS];
  char valid[MAXSTARS];
  // mask for which of the 8 stars to use, now all (=255)
  unsigned char starmask[MAXSTARS];
  float tweight[MAXSTARS];
  float x[MAXSTARS], y[MAXSTARS], f[MAXSTARS];
  float lastx[MAXSTARS], lasty[MAXSTARS], lastf[MAXSTARS];

  float dx, dy, dEL, dCE;

  unsigned short ***boxes;
  unsigned short boxhs; // box half size
  char boxes_initialized;
  
  unsigned long framenumber;
};


// allocate and free memory for the boxes
//int allocate_boxes(unsigned short ***box);
//int free_boxes(unsigned short ***box);

// compute weight for observations
inline float compute_weight(float val); // done

// guider x-y to focal plane x-y
int forward_transform(float xin, float yin, float *xout, float *yout);

// focal plane x-y to guider x-y
int reverse_transform(float xin, float yin, float *xout, float *yout);

// guider x-y to gondola ce-el
int sky_transform(float xin, float yin, float *ce, float *el);

// find sources in an image
void find_sources(unsigned short int *buffer, float *x, float *y, float *f, char *valid, int *count);

// centroid multiple boxes and return star centroids and fluxes
void centroid_boxes(unsigned short int ***box, char *valid, float *x, float *y, float *f);

// save frame data to disk
void save_frame_info();
// save frame image to disk
void save_frame_image();


// load guide data
void save_guide_data(guide_data_t *gd);
void load_guide_data(guide_data_t *gd);
void init_guide_data(guide_data_t *gd);

void copy_guide_data(guide_data_t *gt, guide_data_t *gs);
#endif //GUIDE_DATA_H_
