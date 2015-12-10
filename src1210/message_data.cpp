#include "message_data.h"

void init_message_data(message_data_t *md){
  (*md).quit = 0;
  (*md).self_quit = 0;
};


// MPF FUNCTIONS
void init_mpf_data(mpf_data_t *mp){
  (*mp).counter=0;
};

inline unsigned short int get_mpf_counter(mpf_data_t *td){
  return (*td).counter;
};

inline void inc_mpf_counter(mpf_data_t *td){
  (*td).counter++;
};


// GPS functions
void init_gps_data(gps_data_t *gd){
  (*gd).gps_on = 0;
};


// TMTC functions
void init_tmtc_data(tmtc_data_t *td){
  (*td).counter=0;
};
inline unsigned short int get_tmtc_counter(tmtc_data_t *td){
  return (*td).counter;
};
inline void inc_tmtc_counter(tmtc_data_t *td){
  (*td).counter++;
};


// DET functions
void init_det_data(det_data_t *dd){
  (*dd).counter = 0;
};
