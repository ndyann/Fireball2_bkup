#ifndef GUIDING_SETTINGS_H_
#define GUIDING_SETTINGS_H_

#define MAXSTARS 8

typedef enum guiding_modes_t {
  NOOP, // do nothing.
  ACQUISITION_PHASE1,
  ACQUISITION_PHASE2,
  ACQUISITION_PHASE3,
  ACQUISITION_PHASE4,
  MULTI_STAR,
  MULTI_STAR_FOLLOW
} guiding_modes_t;

typedef struct guiding_settings_t {
  unsigned int exptime;
  guiding_modes_t guide_mode;
} guiding_settings_t;


// functions to load and save guiding settings
void load_guiding_settings(guiding_settings_t *guiding_settings_t);
void save_guiding_settings(guiding_settings_t *guiding_settings_t);
void init_guiding_settings(guiding_settings_t *gs);


#endif //GUIDING_SETTINGS_H_