#include "guiding_settings.h"

void init_guiding_settings(guiding_settings_t *gs){
  (*gs).exptime = 200; //ms before, 150
  (*gs).guide_mode = (guiding_modes_t) 1; //useless mode 
};

void save_guiding_settings(guiding_settings_t *guiding_settings_t){
};
void load_guiding_settings(guiding_settings_t *guiding_settings_t){
};
