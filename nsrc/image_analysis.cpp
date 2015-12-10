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



void track(uint16_t *buf, int width, int height, double *tx, double *ty, uint16_t ***boxes, int *bhs, char *starmask){
  //Copy windows from image in buffer
  //--------
  //First, I need to know the indexes that define the boxes
  //Loop by window, sorry, could not find a better way
  int box, row;
  int buf_index;
  int box_row, box_col;
  int x_max, y_max,  pix_max;
  double x_cen, y_cen, f_cen, background;
  static unsigned int cbhs = 2;  //Half size of box used for centroiding
  //int 
  
  for (box=0; box < MAXSTARS; box++){ //CHANGE FROM 2 TO MAXSTARS!!
    printf("Box = %d, valid = %d\n",box,starmask[box]);
    if (starmask[box]==1){ //Only work with the stars we want
      box_row = 0;
      x_max = 0;
      y_max = 0;
      pix_max = -1;
      for (row =int(ty[box])-(*bhs); row <= int(ty[box])+(*bhs); row++){
        //box_index = 0 + (*bhs) * box_row + (*bhs) * (*bhs) * box; 
        buf_index = int(tx[box])-(*bhs) + row * width;
        //printf("row = %d, buf_index = %d, pixel = %d",row,buf_index,*(buf+buf_index+13));
        memcpy(boxes[box][box_row],buf+buf_index,(2*(*bhs)+1)*sizeof(uint16_t));
        //printf(", pixel = %d\n", *(boxes[box][box_row]+13));
	//Getx the highest pixel value and indexes for the row
	pix_max = std::max_element(boxes[box][box_row],boxes[box][box_row]+(2*(*bhs)+1));
	x_max = std::distance(boxes[box][box_row],std::max_element(boxes[box][box_row],boxes[box][box_row]+(2*(*bhs)+1)));
	y_max = row;
	printf("box %d, x_max = %d, x_max = %f\n", box, x_max, x_max);
	//Compares with the previous highest value if higher updates value and row indes	
	if (boxes[box][box_row][x_max] > pix_max){
	  pix_max = boxes[box][box_row][x_max];
	  y_max = box_row;
	}
        //printf("Column_index_max = %d, row_index_max = %d, pix_max = %d\n",x_max,y_max,pix_max);
	box_row++;
	//printf("%d",box_row);
      }
    //Compute the centroid and average background
    //I use the pixels in cenbox to compute centroid and the rest for the
    //background
    x_cen = 0.0;
    y_cen = 0.0;
    f_cen = 0.0;
    background = 0.0;
    //First i compute the total flux in the centroid box, and the sky background per pixel
    for(box_row = 0; box_row < (2*(*bhs)+1); box_row++){
      //printf("%d\n",box_row);
      for(box_col = 0; box_col < (2*(*bhs)+1); box_col++){
        //printf("Row = %d, Col = %d",box_row,box_col);
        if(std::abs(*bhs - box_row)<= cbhs && std::abs(*bhs - box_col)<=cbhs){
	  //printf("Centroid pixel\n");
	  //x_cen = x_cen + box_col * boxes[box][box_row][box_col];
	  //y_cen = y_cen + box_row * boxes[box][box_row][box_col];
	  f_cen = f_cen + boxes[box][box_row][box_col];
	  //printf("x_cen = %f, y_cen = %f, f_cen = %f\n",x_cen,y_cen,f_cen);
        }
	else {
	  //printf("Background pixel\n");
	  background = background + boxes[box][box_row][box_col];
	}
      }
    }
    //printf("Total back = %f\n",background);
    //Divides total pixel counts by number of background pixels
    background = background / ((2*(*bhs)+1)*(2*(*bhs)+1) - (2*cbhs+1)*(2*cbhs+1));
    //Removes background from flux in centroid box
    //printf("Pixel background = %f\n", background);
    //printf("Raw_flux = %f\n", f_cen);
    f_cen = f_cen - background * (2*cbhs+1)*(2*cbhs+1);
    //printf("Normalized flux = %f\n", f_cen);
    //f_cen = f_cen / ((2*cbhs+1)*(2*cbhs+1));
    //printf("Pixel flux source = %f\n",f_cen);
    //Now we can compute the x_cen and y_cen
    printf("x_max = %d, y_max = %d\n", x_max, y_max);
    for (box_row = (y_max - cbhs); box_row <= (y_max + cbhs); box_row++){
      for (box_col = (x_max - cbhs); box_col <= (x_max + cbhs); box_col++){

        //(boxes[box][box_row][box_col] - background)
        printf("box_row = %d, box_col = %d\n",box_row, box_col);
	//printf("%f, %f\n", double(box_row),double(box_col));
	//printf("x_cen_old = %f",x_cen);
	//printf("%d\n", boxes[box][box_row][box_col]);
	//printf("%f\n", background);
	//printf("%f\n", boxes[box][box_row][box_col] - background);
	//printf("%f\n", box_row * (boxes[box][box_row][box_col] - background));
	//printf("%f\n", double(box_row) * (boxes[box][box_row][box[col] - background));
	
	
	//printf("pixel_old = %d, background = %f, box_col = %d, new_pixel = %f\n", boxes[box][box_row][box_col], background, box_col, box_col * (boxes[box][box_row][box_col] - background));
	//x_cen = x_cen + box_col * (boxes[box][box_row][box_col] - background);
        //printf("box_col = %d, pixel = x_cen = %f\n",x_cen);
	//y_cen = y_cen + box_row * double(boxes[box][box_row][box_col]);
	//f_cen = f_cen + double(boxes[box][box_row][box_col]);
	//printf("%d, %d, %d, %f, %f, %f\n",box_row,box_col,boxes[box][box_row][box_col],x_cen,y_cen,f_cen);
      }
    }
    //*/
    //Centroid needs to be in full frame coordinates
    x_cen = x_cen / f_cen + int(tx[box])-(*bhs);
    y_cen = y_cen / f_cen + int(ty[box])-(*bhs);
    printf("Centroid: (%f, %f, %f) in fullframe coord\n",x_cen, y_cen, f_cen);
    } //if loop (if the mask tells us to use that star

  }
/*
  for (box=0; box < MAXSTARS; box++){
    //Loop for the rows of the full image that cover the window
    for (row = std::max(int(y[box])-boxhs,0); row <= std::min(int(y[box])+boxhs,height); row++){
      memcpy(box[0],buf[x[box]-boxhs+row*width], (2*boxhs+1)*sizeof(uint16_t));
    }
  }
*/
  //Compute centroids in each window (no fblob)
  //---------------
};




