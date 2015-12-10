#ifndef MESSAGE_DATA_H_
#define MESSAGE_DATA_H_

#include "mpf_message_format.h"
#include "tmtc_message_format.h"
#include "guide_data.h"
#include "display.h"

#define TMTC_MESSAGES 6
#define DET_MESSAGES 1

typedef struct guide_data_t guide_data_t;

typedef struct mpf_data_t { 
  unsigned short int counter;
  unsigned int period[MPF_MESSAGES];
  mpf_m000_t mpf000;
  mpf_m001_t mpf001;
  mpf_m002_t mpf002;
  mpf_m003_t mpf003;
  mpf_m004_t mpf004;
} mpf_data_t;

typedef struct tmtc_data_t { 
  unsigned short int counter;
  unsigned int period[TMTC_MESSAGES];
  tmtc_m000_t tmtc000; // exit_guider
  tmtc_m001_t tmtc001; // acquisition mode 
  tmtc_m002_t tmtc002; // multistar mode # stars, standard
  tmtc_m001_t tmtc003; // multistar mode # stars, follow
  tmtc_m002_t tmtc004; // change guiding coords 
  tmtc_m005_t tmtc005; // change guiding rotation
} tmtc_data_t;

typedef struct gps_data_t { 
  unsigned char gps_on;
} gps_data_t;

typedef struct det_data_t {
  unsigned short int counter;
  unsigned int period[DET_MESSAGES];
  //  det_m000_t det000; // something... anything...
} det_data_t;


typedef struct message_data_t {   
  guide_data_t *guide_data;
  guide_data_t *next_guide_data;
  guiding_settings_t *guide_settings;
  guiding_settings_t *next_guide_settings;
  mpf_data_t *mpf_data;
  camera_parameters_t *cam_data;
  gps_data_t *gps_data;
  det_data_t *det_data;
  display_settings_t *disp;
  
  double time;

  unsigned char quit;
  unsigned char self_quit;
  
} message_data_t;

void init_message_data(message_data_t *md);
void init_mpf_data(mpf_data_t *mp);
void init_det_data(det_data_t *dd);
void init_gps_data(gps_data_t *gd);

// mpf functions

// tmtc functions
inline unsigned short int get_tmtc_counter(tmtc_data_t *td);
inline void inc_tmtc_counter(tmtc_data_t *td);




#endif // MESSAGE_DATA_H_

