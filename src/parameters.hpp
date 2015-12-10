//
//  MINT
//	Main header
//
//  Created by Lucas Sousa de Oliveira on 9/5/12.
//  Property of NASA - Goddard Space Flight Center
//

#ifndef rubble_mint_parameters
#define rubble_mint_parameters
/* Don't change this */
#define ENABLE  1
#define DISABLE 0

/* CHANGE ONLY BELOW THIS LINE */

/* The path for the program */
#ifndef HOMEPATH
#define HOMEPATH "/home/salo/FB/Fireball2/mint_camera_only"

#endif

/* Enable/Disable specific devices */
#define LOGGING		ENABLE		// Enable the logging functions
#define CAMERA		ENABLE		// Enable capturing frames from PCO.Edge camera
#define FITS		ENABLE		// Enable saving file as FITS
#define DEBUG		DISABLE		// Enable waiting on error
#define LOG2SCREEN	DISABLE		// Enable printing logs to stdout

/* Global parameters */
#define MAX_RETRY	10		// attempts. Max number of tries to open camera and grab frame.

/* Camera parameters */
#define CAM_GRAB_N_PICS		1	// frames (-1 = infinite)
#define CAM_WAIT_FRAME		10	// milliseconds. Time to wait for frame before raising timeout error.
#define CAM_EXPOSURE		10	// milliseconds 
#define CAM_SATURATION 		65535 	//max pixel value (16 bits)


/* Status parameters */
#define STATUS_DELAY		10 		//milliseconds
#define STATUS_EXPOSURE		100		//milliseconds
#define STATUS_BUFFER		20 		//buffer size (pictures)

/* Lost in Space parameters */
#define FIND_EXPOSURE		200 	//milliseconds
#define FIND_WAIT_FRAME		10 		//milliseconds
#define FIND_INVERT			ENABLE  //invert image (lab)
#define FIND_FITS			ENABLE  //Save image used for Astrometry.net
//#define FIND_ASTROMETRY		"/home/salo/0.Jose/astrometry/bin/solve-field --scale-low 1 --no-fits2fits --no-verify --no-plots " 	//Full command to run astrometry on machine
#define FIND_ASTROMETRY		"/home/salo/FB/astrometry/bin/solve-field --scale-low 1 --no-fits2fits --no-verify --no-plots "

/* Guiding parameters */
#define GUIDE_CAM_WAIT_FRAME	100 		//milliseconds. Time waits to get frame before raising error
#define GUIDE_LONG_EXPOSURE		200			//milliseconds. Exposure used at the beginning of guidance for star acquisition
#define GUIDE_INVERT			ENABLE 		//invert image (lab)
#define GUIDE_FITS_INITIAL		ENABLE 		//save initial frames in fits
#define GUIDE_STAR_AUTO			ENABLE 		//auto select 2 stars for guidance
#define GUIDE_CROP				ENABLE 		//crop fov to avoid picking star close to border
#define GUIDE_CROP_MARGIN		100 		//margin to crop fov in pixels
#define GUIDE_WINDOW_SIZE		100 		//size of window around guiding star in pixels
#define GUIDE_DIST_MIN			250 		//min dist in pixels between primary and secondary star (for rotation estimate)
#define GUIDE_COUNT_MAX			20 			//number of guiding cycles for continuous
#define GUIDE_COUNT_ROTATION	2  			//subcycles for rotation estimate (30 nominal)
#define GUIDE_STACK_SECONDARY	ENABLE 		//allow to stack image for secondary to increase signal


#endif
