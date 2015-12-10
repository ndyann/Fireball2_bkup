/* FIREBALL Guider software */
/* December 8, 2015 */

#include <pthread.h>
#include "fireball_camera2.h"
#include <cstdio>
#include <cstdlib>
//#include "writelog.h"
#include "display.h"
#include "message_function.h"
#include "image_function.h"
#include "staticAO.h"


// do some locking
pthread_mutex_t guide_data_lock=PTHREAD_MUTEX_INITIALIZER;

int main(){
  fireball_camera2 fcam; // fireball camera
  char str[255]; // utility string

  // data structure used to define guide mode and camera configuration
  guiding_settings_t guide_settings;
  guiding_settings_t next_guide_settings;

  // data for communication with other systems
  mpf_data_t mpf_data;
  tmtc_data_t tmtc_data;
  det_data_t det_data;
  gps_data_t gps_data;
  message_data_t message_data;
  
  // data for display purposes
  display_settings_t disp_settings;

  // image dat for guiding
  guide_data_t guide_data, next_guide_data;



  // data structure to be passed to the image analysis thread
  
  // threads to do the communication stuff and message analysis
  pthread_t analysis_thread;
  pthread_t message_thread;

  //  StartLog(); // start the guider log
  
  //InitDisplay();
  
  ComSetup();

  OpenAOPort();

  // Cross link the structures to the message data
  message_data.guide_data = &guide_data;
  message_data.next_guide_data = &next_guide_data;
  message_data.mpf_data = &mpf_data;
  message_data.guide_settings = &guide_settings;
  message_data.next_guide_settings = &next_guide_settings;
  message_data.det_data = &det_data;
  //  message_data.gps_data = &gps_data;
  message_data.cam_data = &fcam.camera_parameters;
  message_data.disp = &disp_settings;
    

  
  // Cross link the structures to image/guiding data
  guide_data.mpf_data = &mpf_data;
  guide_data.cam_data = &(fcam.camera_parameters);
  //  guide_data.det_data = &det_data;
  //guide_data.gps_data = &gps_data;
  guide_data.guide_settings= &guide_settings;
  //  guide_data.disp &disp_settings;
  
  // initialize the data structures
  init_guide_data(&guide_data);
  init_message_data(&message_data);
  init_mpf_data(&mpf_data);
  init_det_data(&det_data);
  //init_gps_data(&gps_data);
  init_guiding_settings(&guide_settings);
  init_guiding_settings(&next_guide_settings);

  // initalize the display parameters

  // Load previous guide settings
  //  LoadGuideSettings(&guide_settings);
  //  LoadGuideData(&guide_data);

  fcam.start();
  int err = fcam.cam_config();
  
  if (err != 0) {
    sprintf(str,"Camera did not succeed with error: %d\n",err);
      //    WriteToLog(str);
    exit(err);
  };
  
  // set the camera exposure time
  fcam.set_exposure(guide_settings.exptime);

  
  
  (void) pthread_create(&message_thread, NULL, message_function, (void *) &message_data);
  
  while(1){
    // if we need to update guide settings
    if(guide_data.update_guide_settings){
      if(guide_settings.guide_mode != next_guide_settings.guide_mode){
	pthread_mutex_lock(&guide_data_lock);
	guide_settings.guide_mode = next_guide_settings.guide_mode;
	guide_data.update_guide_settings = 0;
	pthread_mutex_unlock(&guide_data_lock);
	};
      
      // do we want to change the exposure time? 
      if(guide_settings.exptime != next_guide_settings.exptime){
	fcam.set_exposure(next_guide_settings.exptime);
	guide_settings.exptime = next_guide_settings.exptime;
	guide_data.update_guide_settings = 0;
      };
    };
    
    if(guide_data.update_guide_data){
      // this updates the whole stack of data...
      pthread_mutex_lock(&guide_data_lock);
      copy_guide_data(&next_guide_data, &guide_data);
      guide_data.update_guide_data = 0;
      pthread_mutex_unlock(&guide_data_lock);
    };


    if(message_data.quit){
      // we will be quitting now.
      message_data.self_quit = 1;
      continue;
    };

    // grab an exposure
    fcam.grab_single();
    // once exposure is done, spin off another thread.
    (void) pthread_create(&analysis_thread, NULL, image_function, (void *) &guide_data);
    pthread_detach(analysis_thread);
    // if we really want the thread to exit before the next grab call:
    // do this only if you start getting thread collisions, which
    // really should not happen for 30+ ms exposure times  
    // pthread_join(&image_thread);
  };

  //CloseDisplay();
  // stop the camera
  fcam.stop();
  //StopLog();
  
  CloseAOPort();

};
