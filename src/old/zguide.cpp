/* This program uses 2 stars to provide error signal
to the guiding computer*/

#include "zguide.hpp"

int main (int argc, const char * argv[]){
   printf("Entering the z_guide program\n");
   cout << "################################################################################\n";
   cout << "Entering guiding program...\n";
   cout << "################################################################################\n";
   //Creates a camera and grabber objects
   #if CAMERA
   CPco_cl_com camera;
   CPco_me4_edge grabber;
   #endif
   //Variable values that are required during the process, put here to avoid scope issues:
   //We need to check the result from calling functions
   unsigned int c_ok = -1;
   //We need a place to save the images grabbed by the camera
   uint16_t* p_img = NULL; 
   //We may need some image parameters to invert the images
   int NegativeIm = GUIDE_INVERT;           //If 1 the image used is a negative and needs inversion
   const int CamRows = 2160;			//Camera rows in 1x1 mode
   const int CamCols = 2560;			//Camera columns in 1x1 mode
   const uint16_t CamSat = 65535;		//Maximum pixel value (saturation)

   if (GUIDE_STAR_AUTO == 1){
/* In automatic mode the program does the following:
	1. Makes a long exposure frame
	2. Extracts stars from it, chooses the 2 brightests, shows in console their pixel location in the frame
	3. Sets windows around them and sets the camera for short exposure mode
	4. Enters in a loop, for each loop it gets a frame, slices the windows, uses the first window to compute E and CE errors
*/

   //This implementation gives me errors, need to verify later
   //Sets the SIGINT treatment. ctrl-c terminates program and closes connections
   //if (signal(SIGINT, terminate)==SIG_IGN)
   //  signal(SIGINT, SIG_IGN);

   //*************************************************************************************************************
   //1 Takes first long exposure image to identify potential sources in the frame. Saves it in TIFF if necessary
   //*************************************************************************************************************
   cout << "--------------------------------------------------------------------------------\n";
   cout << "Identifying stars to guide on:\n";
   cout << "--------------------------------------------------------------------------------\n";
   #if CAMERA
   log(__FUNCTION__,OK,"Connect to camera");
   cout << "Starting camera... ";
   c_ok = start_camera(camera,grabber);
   (c_ok==0)? cout << "camera started!\n" : cout << " Error start_camera()\n";
   //Sets the exposure to long value to first identify many stars
   cout << "Setting exposure parameters... ";
   c_ok = camera.PCO_SetDelayExposure(GUIDE_CAM_WAIT_FRAME, GUIDE_LONG_EXPOSURE);
   (c_ok==0)? cout << "wait time set to " << GUIDE_CAM_WAIT_FRAME << " ms, exposure set to " << GUIDE_LONG_EXPOSURE << " ms\n" : cout << "Error PCO_SetDelayExposure()\n";
   //Grabs a frame and stores it
   cout << "Grabbing field frame... ";
   c_ok = grab_single(&grabber, &p_img);
   (c_ok==0)? cout << "frame grabbed!\n" : cout << "Error grab_single()\n";
   if(c_ok == OK){
      log(__FUNCTION__, OK, "Grabbed new frame successfully");
   }
   else{
      log(__FUNCTION__, OK, "Error: Could not grab new frame");
   }
   //Inverts image for lab
   if(NegativeIm == 1){
      cout << "Inverting image (lab)... ";
      ImageInvert(p_img, CamRows*CamCols, CamSat);
      cout << "image inverted!\n";
      log(__FUNCTION__,OK,"Image inverted");
   }
   #endif
   //Saves frame in FITS file if needed
   #if GUIDE_FITS_INITIAL
   SaveFITS(p_img, CamCols, CamRows);
   #endif

   //**************************************************************************
   //2. Extracts the sources, picks 2 brightest to track and prepares window structure
   //**************************************************************************
   //We may want to crop the image to avoid picking stars near the border of the fov
   int FrameMargin = GUIDE_CROP * GUIDE_CROP_MARGIN;
   int FrameX = CamCols - 2 * FrameMargin;
   int FrameY = CamRows - 2 * FrameMargin;
   uint16_t *frame = new uint16_t[FrameX*FrameY];
   ImageWindow(p_img, frame, CamRows, CamCols, FrameMargin);
   (GUIDE_CROP==1)? cout << "Image cropped by " << FrameMargin << " pixels to avoid fov borders\n" : cout << "Image not cropped, careful with fov borders\n";
   //Saves the cropped image in a FITS file
   #if GUIDE_FITS_INITIAL
   SaveFITS(frame, FrameX, FrameY);
   #endif
/*
   //DEBUG: just a check to see whether there are saturated pixels in the frame (erase later)
   uint16_t maxframe = 0;
   for (int i=0; i<FrameX*FrameY; i++){
      maxframe = max(maxframe, frame[i]);
   }
   cout << "Max value in frame = " << maxframe << ", and saturation value is " << CamSat << endl;
*/
   //Source extraction from the (cropped) image
	cout << "Extracting sources from the (cropped) image... \n";
   frameblob fblob(frame, FrameX, FrameY, 16, 1.0);    // creates a frameblob structure to store the blobs in the whole frame
   fblob.clearblobs();   //not necessary but better to clear blobs in case there are some from previous calls
   //Sets search values IMPORTANT to see which ones to port to parameters.txt file
   cout << "Setting parameters for source extraction...";
   fblob.set_satval(CamSat);    //pixel saturation level
   fblob.set_grid(100);         //size of grid, determines how many sources will be found (max 1 source per grid)
   fblob.set_cenbox(80);        //size of box to compute centroid. It has to be large enough to hold brightest star
   fblob.set_apbox(40);         //size of aperture box to compute source flux. It has to be large enough to hold brightest star
   fblob.set_threshold(5.0);    //threshold for detection (5 sigma now)
   fblob.set_disttol(50*50);    //maximum allowable distance betwee sources, now at 50 pixels
	cout << "set!\n";
   //Extracts sources
   fblob.calc_mapstat();    		// calc map mean + count # saturated pixels each column
   fblob.calc_searchgrid();    	// find cells in search grid with source
   fblob.calc_centroid();    	   // after search/flux, calculate centroids
   fblob.calc_flux();    		   // sum flux in apertures, check if pointlike
   fblob.fix_multiple();    		// check for multiple sources within disttol and fix
   fblob.sortblobs();		      // sorts blobs from highest to lowest flux
   int n_stars = fblob.Numblobs();

	 // Frees the memory of the frame once the sources are extracted to avoid leaks
	 delete [] frame;
/*
   //DEBUG: how to check content of bloblist at each step
 	cout<<"Let's check what we've found:\n";
   bloblist *pblob5;
   pblob5 = fblob.getblobs();
   cout << "After reordering" << endl;
   cout << "numblobs after removing multiple sources = " << fblob.Numblobs() << endl;
   cout << "Blob: " << pblob5->getx() << "   " << pblob5->gety() << "   " << pblob5->getflux() << endl;
   pblob5 = pblob5->getnextblob();
   cout << "Blob: " << pblob5->getx() << "   " << pblob5->gety() << "   " << pblob5->getflux() << endl;
   pblob5 = pblob5->getnextblob();
   cout << "Blob: " << pblob5->getx() << "   " << pblob5->gety() << "   " << pblob5->getflux() << endl;
*/
   cout << n_stars << " sources identified in (cropped) fov\n";
   cout << "--------------------------------------------------------------------------------\n";
   cout << "Setting up guidance windows:\n";
   cout << "--------------------------------------------------------------------------------\n";
   //window structure creation**********************************************************************************
	cout << "Creating main window for elevation and cross-elevation correction...";
   static bloblist *pFrameBlob;
   pFrameBlob = fblob.getblobs();   //list of stars with x, y centroid coordinates and ordered by flux
   //Picks the brightest star to measure E and CE error, since we clipped the frame when we extracted the sources
   //the margin was larger than half a window, we do not need to check that the star is too close to the fov border
	double first_old_x = pFrameBlob->getx();	   //Centroid x coordinate in clipped frame pixel coordinates
	double first_old_y = pFrameBlob->gety();     //Centroid y coordinate in clipped frame pixel coordinates
	//To avoid cropping the full images (time), we center the window on full chip coordinates
	first_old_x = first_old_x + FrameMargin;     //Centroid x coordinate in full frame pixel coordinates
	first_old_y = first_old_y + FrameMargin;     //Centroid y coordinate in full frame pixel coordinates
	cout << "with center in (" << first_old_x << ", " << first_old_y << ") pixels\n";
	//Computes the lower left corner of the main window to slice the images in guiding loop
	const int windowSize = GUIDE_WINDOW_SIZE;
	int first_x_ref = int(first_old_x) - windowSize / 2;
	int first_y_ref = int(first_old_y) - windowSize / 2;
   //Builds secondary window for field rotation
   cout << "Creating secondary window for field rotation correction... ";
   double second_old_x = 0.0;
   double second_old_y = 0.0;
   double rot_dist = 0.0;
   int index_star = 1;
   while (rot_dist<GUIDE_DIST_MIN && index_star < n_stars){
      pFrameBlob = pFrameBlob->getnextblob();
      second_old_x = pFrameBlob->getx() + FrameMargin;
      second_old_y = pFrameBlob->gety() + FrameMargin;
      rot_dist = sqrt(pow((first_old_x - second_old_x),2) + pow((first_old_y - second_old_y),2));
      index_star = index_star + 1;
   }
   (index_star < n_stars)? cout << "with center in (" << second_old_x << ", " << second_old_y << ") pixels\n" : cout << "Error: run out of stars for secondary\n";
   cout << "The distance between primary and secondary star is " << rot_dist << " pixels\n"; 
   int second_x_ref = int(second_old_x) - windowSize / 2;
   int second_y_ref = int(second_old_y) - windowSize / 2;
   double angle_old = atan2(first_old_y - second_old_y, first_old_x - second_old_x);
	//*********************************************************************************************************
	//3. Guiding loop
   //********************************************************************************************************* 
   cout << "--------------------------------------------------------------------------------\n";
   cout << "Entering guiding loop:\n";
   cout << "--------------------------------------------------------------------------------\n";
	//The loop can go for a number of iterations
	int countmax = GUIDE_COUNT_MAX;
   //We will measure the new centroid of the guiding  and secondary stars
	double first_new_x = 0.0;
	double first_new_y = 0.0;
   double second_new_x = 0.0;
   double second_new_y = 0.0;
   double angle_new = 0.0;
   //And use it to estimate the error in x and y in pixels and in angle in radians
	double error_x = 0.0;
	double error_y = 0.0;
   double error_angle = 0.0;
   //We will centroid just the guiding  and secondary windoww
	uint16_t *first_win = new uint16_t[windowSize*windowSize];
   uint16_t *second_win = new uint16_t[windowSize*windowSize];
   uint16_t *second_win_cum = new uint16_t[windowSize*windowSize];
   uint16_t maxframe = 0;
   //Ensures there stacked window for the secondary star is zero
   ZeroWindow(second_win_cum,windowSize*windowSize);
   //Start the main loop
	for (int count = 1; count < countmax; count++){
	  //Grabs a frame
	  //cout << "Grabbing new frame...\n";
	  c_ok = -1;
      c_ok = grab_single(&grabber, &p_img);
      if(c_ok == OK){
         log(__FUNCTION__, OK, "Grabbed new frame successfully");
     	}
      else{
         log(__FUNCTION__, OK, "Error: Could not grab new frame");
      }
      //Inverts the image if it is a negative
      if(NegativeIm == 1){
         ImageInvert(p_img, CamRows*CamCols, CamSat);
         log(__FUNCTION__,OK,"Image inverted");
      }
		//cout << "... new frame grabbed\n";
	 	//Slices the image to get just the main guidance window
		ImageWindow(p_img, first_win, CamRows, CamCols, windowSize, windowSize, first_x_ref, first_y_ref);
	 	//Gets the centroid of guiding star in window coordinates
      Centroid(first_win, windowSize, CamSat, first_new_x, first_new_y);
		//New centroid coordinates in image coordinates (pixels)
		first_new_x = first_new_x + first_x_ref; 
		first_new_y = first_new_y + first_y_ref;
	 	//x,y error in pixels
	 	error_x = first_new_x - first_old_x;
	 	error_y = first_new_y - first_old_y;
      //CONVERT x,y error in pixels in ELEVATION / CROSS ELEVATION in arcsec
	 	//Output in console
	 	cout << "Count = " << count << ", error_x = " << error_x << " pixels, error_y = " << error_y << " pixels\n";
	   //Update pixel coordinates of main guiding star centroid
		first_old_x = first_new_x;
		first_old_y = first_new_y;

      //Slices the secondary window
      ImageWindow(p_img, second_win, CamRows, CamCols, windowSize, windowSize, second_x_ref, second_y_ref);
      //Stack if the secondary window is not saturated and stacking is enabled
      maxframe = 0;
      for (int i=0; i<windowSize*windowSize; i++){
         maxframe = max(maxframe, second_win_cum[i]);
      }
      if (GUIDE_STACK_SECONDARY == 1 && maxframe < CAM_SATURATION){
         for (int i=0; i<windowSize*windowSize; i++){
            second_win_cum[i] = second_win_cum[i] + second_win[i];
         }
      }
      else{
         for (int i=0; i<windowSize*windowSize; i++){
            second_win_cum[i] = second_win[i];
         }
      }
      // If it is time for rotation, compute rotation, output it, and zero cumulated secondary window
      if(count%GUIDE_COUNT_ROTATION==0){
         //First get the new centroid
         Centroid(second_win_cum, windowSize, CamSat, second_new_x, second_new_y);
         //New centroid coordinates in image coordinates (pixels)
         second_new_x = second_new_x + second_x_ref; 
         second_new_y = second_new_y + second_y_ref;
         //Computes new angle and error in angle
         angle_new = atan2(first_new_y - second_new_y, first_new_x - second_new_x);
         error_angle = angle_new - angle_old;
         cout << "error_angle = " << error_angle << " rad\n";
         //Update angle
         angle_old = angle_new;
         //Need to convert error in angle!

         //And let's not forget to zero the cumulated window
         ZeroWindow(second_win_cum,windowSize*windowSize);
      }
	 }
	 //Free guiding window after usage to prevent leaks
	 delete [] first_win;
    delete [] second_win;
    delete [] second_win_cum;
   }
   else{
      log(__FUNCTION__, OK, "Selected manual star guiding choice, not implemented yet");
      cout << "Selected manual star guiding choice, not implemented yet\n";
   }

   //*********************************************************************************************************
   //4. Closing
   //********************************************************************************************************* 
   cout << "--------------------------------------------------------------------------------\n";
   cout << "Closing and freeing ressources:\n";
   cout << "--------------------------------------------------------------------------------\n";
   //Freeing dynamically allocated variables
   #if CAMERA
   if(p_img != NULL){
      log(__FUNCTION__, OK, "Freeing image buffer (Unverifiable)");
      free(p_img);
   }
   else{
      fprintf(stderr, "Could not free image buffer\n");
      fflush(stderr);
      log(__FUNCTION__, OK, "Error: Could not free image buffer: Image buffer was unallocated");
   }
   //Closing the camera and grabber
   log(__FUNCTION__,OK,"Close camera connection and free resources");
   if(close_camera(camera,grabber) == OK)
      log(__FUNCTION__,OK,"Closed camera");
   else
      log(__FUNCTION__,ERROR,"COULD NOT close camera");
   #endif
   cout << "################################################################################\n";
   cout << "Exiting guiding program\n";
   cout << "################################################################################\n";
   
   return 0;
}





//*************************************************************************************************************************************************************
//*************************************************************************************************************************************************************
void ImageInvert(uint16_t *pImage, int ImSize, uint16_t MaxValue){
/* 
In the lab, I used a negative sky photograph for testing, this function
just inverts any image, to turn it back to a positive (dark sky, white sources).
- pImage is a pointer to the image, which PCOEdge5.5 stores as uint16_t
- Size is the number of pixels in the array pointed by pImage
- MaxValue is the saturation value of the image, for PCOEdge5.5 it is 65535
*/

  for (int element = 0; element < ImSize; element++){
    pImage[element] = MaxValue - pImage[element];
  }

}
//**************************************************************************************************************************************************************
//**************************************************************************************************************************************************************
void ImageWindow(uint16_t *pImage, uint16_t *pWindow, int nRows, int nCols, double x, double y, int sizeX, int sizeY, int origin_x, int origin_y){
/*
This function takes an image containedd in pImage, with nRows and nCols, and crops it to a square window
of side size centered in x and y in the original Image.
- nRows, nCols and size are pixels
- pImage is the pointer to the entry image
- pWindow is the pointer to the exit image
- x, y can be double (e.g. centroid coordinates)
*/
   if ((sizeY>nRows)||(sizeX>nCols)){
      cout << "Error: window size exceeds original image. Image left untouched" << endl;
      pWindow = pImage;
   }
   else {
      int x1, x2, y1, y2;
      //calculates the column range of the window
      x1 = round(max(0.0, x - sizeX/2.0));
      x2 = x1 + sizeX - 1;
      if (x2>=nCols){
         x2 = nCols - 1;
         x1 = x2 - sizeX + 1;
         if (x1<=0){
            x1 = 0;
         }
      }
      origin_x = x1;
      //calculates the row range of the window
      y1 = round(max(0.0, y - sizeY/2.0));
      y2 = y1 + sizeY - 1;
      if (y2>=nCols){
         y2 = nCols - 1;
         y1 = y2 - sizeY + 1;
         if (y1<=0){
            y1 = 0;
         }
      }
      origin_y = y1;
      //cout << "winref_x = " << origin_x << ", winref_y = " << origin_y << endl;
      //slices pImage to get pWindow
      int element = 0;
      for(int row = y1; row < y2 + 1 ; row++){
         for(int column = x1; column < x2 + 1; column++){
            pWindow[element] = pImage[row * nCols + column];
            element = element + 1;
         }
      }
   }
}
//*******************************************************************************************************************************************

//*******************************************************************************************************************************************
void ImageWindow(uint16_t *pImage, uint16_t *pWindow, int nRows, int nCols, int sizeX, int sizeY, int origin_x, int origin_y){
	/* This function takes as inputs 2 pointers, one to an image of size nRows x nCols and another to a memory space of size sizeX x sizeY
	it also takes the coordinates of the lower left corner of the window we want to create (origin_x, origin_y). It then stores the content
	of the window to the memory allocated and pointed by the pointer to the window
	*/
	//First resizes the window size to fit the image if necessary
	sizeX = min(sizeX, nCols - origin_x);
	sizeY = min(sizeY, nRows - origin_y);
	int element = 0;
	for (int row = origin_y; row < origin_y + sizeY; row++){
		for (int column = origin_x; column < origin_x + sizeX; column++){
			pWindow[element] = pImage[row * nCols + column];
			element = element + 1;
		}
	}

}

//******************************************************************************************************************************************
void ImageWindow(uint16_t *pImage, uint16_t *pWindow, int nRows, int nCols, int margin){
	/* This function takes as inputs 2 pointers, one to an image of size nRows x nCols and another to a memory space of size 
	(nRows - 2 * margin) x (nCols - 2 * margin) and clips the former into the later
	*/
	int element = 0;
	for (int row = margin; row < nRows - margin; row++){
		for (int column = margin; column < nCols - margin; column++){
			pWindow[element] = pImage[row * nCols + column];
			element = element + 1;
		}
	}
}
//******************************************************************************************************************************************
void ZeroWindow(uint16_t *pWindow, int nSize){
   /* This function zeros the elements of a window */
   for (int element = 0; element < nSize; element++){
      pWindow[element] = 0;
   }
}
//*******************************************************************************************************************************************
void SaveFITS(uint16_t * p_img, const int& CamCols, const int& CamRows){
   /* This function uses the function save_ptr2fits() to save an image to fits file
   so it will need fitswrapper.hpp, it also uses logs functions
   */
   //Prepares the FITS file name and logs (may want to remove the log later)
   unsigned int c_ok = 0;
   struct tm timeinfo;              
   char time1[500],name[500],aux[500]; 
   time_t rawtime;   
   time(&rawtime);
   log(__FUNCTION__,OK,"Forming filename using system time information");
   timeinfo = *localtime(&rawtime);
   strftime(time1, 500, (HOMEPATH"/imgs/%Y-%m-%d_%H-%M-%S_%%06d.fits"), &timeinfo);
   sprintf(name,time1,clock()%CLOCKS_PER_SEC);
   log(__FUNCTION__,OK,"Filename formed. Result: "+string(name));
   cout << "Saving " << name << " ... ";
   //Saves the image in a FITS file
   uint16_t att;   //Number of attempts to save the image
   log(__FUNCTION__, OK, "Attempting to save image");
   for (c_ok = NOTHING, att = 0; c_ok != OK && ++att < MAX_RETRY; c_ok = save_ptr2fits(name,CamCols,CamRows,p_img));
   if(c_ok != OK){
      sprintf(aux,"Could not save image '%s' in %d tries",name,MAX_RETRY);
      log(__FUNCTION__,ERROR,aux);
   }
   (c_ok==0)? cout << " saved!\n" : cout << "Error: could not save FITS\n";
}
//*******************************************************************************************************************************************
void Centroid(uint16_t * win, const int & windowSize, const uint16_t & CamSat, double & x_centroid, double & y_centroid){

   //Creates a frameblob object from the window (an image matrix), assuming 16 bit images and 1.0 platescale
   frameblob win_blob(win, windowSize, windowSize, 16, 1.0); 
   //Clears the object from potential blobs (not necessary)
   win_blob.clearblobs();
   //Sets parameters to find blobs in the image
   win_blob.set_satval(CamSat);    //pixel saturation level
   win_blob.set_grid(100);         //size of grid, determines how many sources will be found (max 1 source per grid)
   win_blob.set_cenbox(80);        //size of box to compute centroid. It has to be large enough to hold brightest star
   win_blob.set_apbox(40);         //size of aperture box to compute source flux. It has to be large enough to hold brightest star
   win_blob.set_threshold(5.0);    //threshold for detection (5 sigma now)
   win_blob.set_disttol(50*50);    //maximum allowable distance betwee sources, now at 50 pixels
   //Find blobs and stores in a list their pixel centroid coordinates and flux. List ordered by flux
   win_blob.calc_mapstat();         // calc map mean + count # saturated pixels each column
   win_blob.calc_searchgrid();      // find cells in search grid with source
   win_blob.calc_centroid();        // after search/flux, calculate centroids
   win_blob.calc_flux();            // sum flux in apertures, check if pointlike
   win_blob.fix_multiple();         // check for multiple sources within disttol and fix
   win_blob.sortblobs();            // sorts blobs from highest to lowest flux
   //Extracts the first (brightest) blob and gets its centroid coordinates, only if it gets blobls
   if(win_blob.Numblobs()>0){
      static bloblist *pWinBlob;
      pWinBlob = win_blob.getblobs();     //list of stars with x, y centroid coordinates and ordered by flux
      x_centroid = pWinBlob->getx();      //Centroid x coordinate in clipped frame pixel coordinates
      y_centroid = pWinBlob->gety();
   }
   else{
      cout << "Error Centroid(): No blobs found, centroid left unchanged!\n";
   }
}









