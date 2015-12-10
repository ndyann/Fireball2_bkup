// Code to apply Astrometry.net on image taken by PCO.Edge

#include "zfind.hpp"

using namespace std;

int main (int argc, const char *argv[]) {
  cout << "################################################################################\n";
  cout << "Entering lost-in-space function program...\n";
  cout << "################################################################################\n";
	#if CAMERA
  //Creates the camera and grabber objects to call their methods
	CPco_cl_com camera;
	CPco_me4_edge grabber;
	//*******************************************************************************
	//1. Start camera and grab single long exposure frame
  //*******************************************************************************
	cout << "--------------------------------------------------------------------------------\n";
  cout << "Capturing field to solve for pointing:\n";
  cout << "--------------------------------------------------------------------------------\n";
  //Starts the camera
	cout << "Starting the camera...\n";
  log(__FUNCTION__,OK,"Connect to camera");
  start_camera(camera,grabber);
  //Sets the exposure to long value to identify many stars
	int exposure = FIND_EXPOSURE;  
	cout << "Exposure set to " << exposure << " ms\n";
  camera.PCO_SetDelayExposure(CAM_WAIT_FRAME, exposure);
  cout << "Wait time set to " << FIND_WAIT_FRAME << "ms\n";
	//Grabs a frame
	unsigned int c_ok = NOTHING;			//Variable to store status with camera
  uint16_t* p_img = NULL;                   //To store the raw image from the camera
	c_ok = -1;
  c_ok = grab_single(&grabber, &p_img);
  if(c_ok == OK){
    log(__FUNCTION__, OK, "Grabbed new frame successfully");
  }
  else{
    log(__FUNCTION__, OK, "Error: Could not grab new frame");
  }
  (c_ok==OK)? cout << "Frame grabbed\n" : cout << "Could not grab new frame\n";
  //If the image used is a negative (e.g. at the lab) it inverts it
  int NegativeIm = FIND_INVERT;				//If 1 the image used is a negative and needs inversion
  const uint16_t CamSat = CAM_SATURATION;   //Maximum pixel value (saturation)
  unsigned int numrows, numcols;
  c_ok=grabber.Get_actual_size(&numcols,&numrows,NULL);
  const int CamRows = numrows;			//Camera rows in 1x1 mode 2160
  const int CamCols = numcols;			//Camera columns in 1x1 mode 2560
  if(NegativeIm == 1){
    cout << "--------------------------------------------------------------------------------\n";
    cout << "Inverting image (lab setting):\n";
    cout << "--------------------------------------------------------------------------------\n";
    ImageInvert(p_img, CamRows*CamCols, CamSat);
    log(__FUNCTION__,OK,"Image inverted");
    cout << "Inverted image of " << CamRows << " rows & " << CamCols << " columns\n";
  }
	//****************************************************************************************************
	//2. Save the frame as a FITS file
  //****************************************************************************************************
  #if FIND_FITS
  cout << "--------------------------------------------------------------------------------\n";
  cout << "Saving image as FITS:\n";
  cout << "--------------------------------------------------------------------------------\n";
  //Prepares the FITS file name
  struct tm timeinfo;					//To build FITS name
  char time1[500],name[500],aux[500];		//To build FITS name
  log(__FUNCTION__,OK,"Forming filename using system time information");
  time_t rawtime;
  time(&rawtime);
  timeinfo = *localtime(&rawtime);
  strftime(time1, 500, (HOMEPATH"/imgs/%Y-%m-%d_%H-%M-%S_%%06d.fits"), &timeinfo);
  sprintf(name,time1,clock()%CLOCKS_PER_SEC);
  log(__FUNCTION__,OK,"Filename formed. Result: "+string(name));
  //Saves the image
  if(c_ok == OK) {
    uint16_t att;   //Number of attempts to save the image
    log(__FUNCTION__, OK, "Attempting to save image");
    for (c_ok = NOTHING, att = 0; c_ok != OK && ++att < MAX_RETRY; c_ok = save_ptr2fits(name,CamCols,CamRows,p_img));
     if(c_ok != OK){
      sprintf(aux,"Could not save image '%s' in %d tries",name,MAX_RETRY);
      log(__FUNCTION__,ERROR,aux);
      }
     }
     else{
     	log(__FUNCTION__, OK, "Error: Could not grab new frame");
     }
  #endif
  (c_ok==OK)? cout << "Saved image: " << name << endl : cout << "Could not save image\n";
	//****************************************************************************************************
	//3. Close Camera and free resources
  //****************************************************************************************************
  cout << "--------------------------------------------------------------------------------\n";
  cout << "Closing camera and freeing resources:\n";
  cout << "--------------------------------------------------------------------------------\n";
	//Free buffer
  if(p_img != NULL){
    log(__FUNCTION__, OK, "Freeing image buffer (Unverifiable)");
    free(p_img);
  }
  else{
    fprintf(stderr, "Could not free image buffer\n");
    fflush(stderr);
    log(__FUNCTION__, OK, "Error: Could not free image buffer: Image buffer was unallocated");
  }
  //Close the camera and grabber
  log(__FUNCTION__,OK,"Close camera connection and free resources");
  if(close_camera(camera,grabber) == OK)
    log(__FUNCTION__,OK,"Closed camera");
  else
    log(__FUNCTION__,ERROR,"COULD NOT close camera");
  #endif  //MOVE endif TO WHERE IT MAKES SENSE
	//**************************************************************************************************
	//4. Call Astromety.net from the command line to solve for the field
	//**************************************************************************************************
  cout << "--------------------------------------------------------------------------------\n";
  cout << "Calling Astrometry.net to solve field:\n";
  cout << "--------------------------------------------------------------------------------\n";
  // Generates the command that would need to be input in the terminal to run astrometry.net
  string astrometry_command = FIND_ASTROMETRY;
  string astrometry_full = astrometry_command + name;
  const char * command = astrometry_full.c_str();
  int ret = system(command);
  (ret==0)? cout << "Astrometry.net executed correctly\n" : cout << "Astrometry.net did not excecute correctly\n";
 
  //**************************************************************************************************
  //5. Parse output into file / entry of other program
  //**************************************************************************************************

  cout << "################################################################################\n";
  cout << "Exiting MGS lost-in-space\n";
  cout << "################################################################################\n";

	return 0;
}


//*********************************************************************************************************
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

