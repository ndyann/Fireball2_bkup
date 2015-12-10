#include "image_analysis.h"
//Needs to update to get all this information from a structure guide_data_t
void acq_func(uint16_t *buf, int width, int height, double *x, double *y, double *flux, char *valid, int *nstars){
  // This function extracts x, y, flux from MAXSTARS (8) brightest stars from an image in the buffer pointed by *buf
  int i;
  int stars_found;
  bloblist *pBlobs;
  static unsigned int bits = 16;	     // Image depth
  static uint16_t cam_sat = 65535;   // Pixel saturation value, is 2**bits
  static double plate_scale = 1.0;   // For pixel coordinates
  static unsigned int coarse_grid = 50;  //coarse grid pixsize
  static unsigned int cen_box = 50;       //centroid box
  static unsigned int ap_box = 10;        //aperture box (for flux)
  static double sigma_thres = 5.0;        //sigma threshold for blob identification
  static int dist_tol = 50;               //adjacent source distance tolerance in pixels

  // Setting the frameblob for extracting blobs
  printf("Setting blob structure...\n");
  frameblob fblob(buf, width, height, bits, plate_scale);
  fblob.clearblobs();
  fblob.set_satval(cam_sat);
  fblob.set_grid(coarse_grid);
  fblob.set_cenbox(cen_box);
  fblob.set_apbox(ap_box);
  fblob.set_threshold(sigma_thres);
  fblob.set_disttol(dist_tol*dist_tol);
  printf("... structure set\n");

  // Extracting the blobs
  printf("Extracting blobls...\n");
  fblob.calc_mapstat();
  fblob.calc_searchgrid();
  fblob.calc_centroid();
  fblob.calc_flux();
  fblob.fix_multiple();
  fblob.sortblobs();
  printf("... blobs extracted\n");
  stars_found = fblob.Numblobs();
  
  // Print out how many sources have been identified
  printf("Identified %d blobs\n", stars_found);
  // Initialize the valid tag to 0 (not valid)
  for (i = 0; i < MAXSTARS; i++){
    valid[i]=0;
  }
  // Extracting the blobs to a
  pBlobs = fblob.getblobs();
  *nstars = std::min(MAXSTARS, stars_found);
  printf("Star positions\n");
  // Update the pointers with the information on the fblob
  for (i = 0; i < *nstars; i++){
    x[i] = pBlobs->getx();
    y[i] = pBlobs->gety();
    flux[i] = double(pBlobs->getflux());
    valid[i] = 1;
    pBlobs = pBlobs->getnextblob();
    //Prints (comment out later)
    printf("x = %f, y = %f, flux = %f, valid = %d\n",x[i],y[i],flux[i],valid[i]);
  }

};



void track(uint16_t *buf, int width, int height, double *tx, double *ty, char *valid,  uint16_t ***boxes, int *bhs, char *starmask, double *x, double *y){
  // indexes to move along arrays and pointers
  int buf_index, row, box, box_row, box_col;
  // brightest pixel in array
  int x_max, y_max, pix_max;
  // centroid, background noise, signal-to-noise
  double x_cen, y_cen, f_cen, background, s_n;
  // parameters that won't change between function calls
  static unsigned int cbhs = 2;  // half size of box used for centroiding
  static double sigma = 1.5;     // threshols in s/n to change tag to non valid
  
  for (box=0; box < MAXSTARS; box++){
    //printf("Box = %d, valid = %d\n",box,starmask[box]);
    if (starmask[box]==1){ //Only work with the stars we want
      box_row = 0;
      x_max = 0;
      y_max = 0;
      pix_max = -1;
      // going row by row for ech box to copy it and get coordinates for centroiding
      for (row =int(ty[box])-(*bhs); row <= int(ty[box])+(*bhs); row++){
        // copies a section of bytes from the mem block with the full image to the array 
        buf_index = int(tx[box])-(*bhs) + row * width;
        memcpy(boxes[box][box_row],buf+buf_index,(2*(*bhs)+1)*sizeof(uint16_t));
	// gets the highest pixel of the row, compares it with the previous highest and updates
	if (*std::max_element(boxes[box][box_row],boxes[box][box_row]+(2*(*bhs)+1)) > pix_max){
	  //Update highest pixel
	  pix_max = *std::max_element(boxes[box][box_row],boxes[box][box_row]+(2*(*bhs)+1));
	  x_max = std::distance(boxes[box][box_row],std::max_element(boxes[box][box_row],boxes[box][box_row]+(2*(*bhs)+1)));
	  y_max = box_row;
	}
	else {
	  //Do nothing
	}
	//printf("box = %d, x_max = %d, y_max = %d, pixel_max = %d\n", box, x_max, y_max, pix_max);
	box_row++;	
      }
    // looping through the pixels in the box to compute centroid (smaller box around pix_max
    // and background for signal/noise calculation
    x_cen = 0.0;
    y_cen = 0.0;
    f_cen = 0.0;
    background = 0.0;
    // computes background (per pixel) and background-subtracted flux in cenbox
    for(box_row = 0; box_row < (2*(*bhs)+1); box_row++){
      for(box_col = 0; box_col < (2*(*bhs)+1); box_col++){
        // pixels within cenbox contribute to centroid and signal calculation, rest contribute to noise/background
        if(std::abs(*bhs - box_row)<= cbhs && std::abs(*bhs - box_col)<=cbhs){
	  f_cen = f_cen + boxes[box][box_row][box_col];
          x_cen = x_cen + box_col * (boxes[box][box_row][box_col]);
	  y_cen = y_cen + box_row * (boxes[box][box_row][box_col]);
        }
	else {
	  background = background + boxes[box][box_row][box_col];
	}
      }
    }
    // normalizes centroid coordinates and transforms them in full chip coordinates
    x_cen = x_cen / f_cen;
    y_cen = y_cen / f_cen;
    x_cen = x_cen + tx[box] - (*bhs);
    y_cen = y_cen + ty[box] - (*bhs);
    // calculation of signal-to-noise
    background = background / ((2*(*bhs)+1)*(2*(*bhs)+1) - (2*cbhs+1)*(2*cbhs+1));
    // first definition, using the brightest pixel vs. background
    s_n = pix_max / background;
    // second definition, using the total flux in cenbox
    //background = background * (2*cbhs+1)*(2*cbhs+1);
    //s_n = f_cen / background;
 //printf("xmax = %d, ymax = %d, x_cen = %f, y_cen = %f, s/n = %f\n", x_max, y_max, x_cen, y_cen, s_n);
    // if the signal-to-noise is too low, change valid[box] to 0 (non valid)
    // first way to account for stars leaving the box, clouds... we can add conditions based
    // on distance to border of box
    if (s_n < sigma){
      valid[box] = 0;
    }
    // update pointers x, y
    x[box] = x_cen;
    y[box] = y_cen;
    //printf("Valid? %d\n", valid[box]);
    } // end of "if" loop telling  us to use that star
    printf("star: %d, x_cen: %f, y_cen: %f, s/n: %f, valid? %d\n",box,x_cen,y_cen,s_n,valid[box]);

  }

};




