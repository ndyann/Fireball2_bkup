#include "fireball_camera2.h"

CPco_Log pco_log("pco_edge_grab.log");

// Constructor: opens camera and grabber and gets them ready
fireball_camera2::fireball_camera2(){

  // Creates an object of type CPco_cl_com for the camera (PCO lib),
  // tries to open it MAX_RETRY times (defined in header), and prints error if fails
  err=1;                // To check function returns (0==OK)
  uint32_t att=1;       // To keep track of open attempts

  picnum = 1;
  timeout = 10;
   
  /* Tries to open camera MAX_RETRY times. If MAX_RETRY attempts
     are made of a FATAL error is risen, the function is shut. */
  for (att = 1; att <= MAX_RETRY && err != FATAL; att++)   {
    printf("Attempt #%3d of opening the camera.\n",att);

    //Debug - JJG
    sleep(1);    
    fflush(stdout);        

    //In case we want logs
    //camera.log  = &pco_log;
    //grabber.log = &pco_log;
    
    err = camera.Open_Cam(0);
    if(err == PCO_NOERROR)  {
    }   else    {                  
      printf("Could not open camera. Msg: 0x%x\n",err);
      fflush(stdout);
      continue;
    }
    
    err = camera.PCO_SetRecordingState(0);
    //    camera.PCO_ResetSettingsToDefault();
    //    camera.PCO_SetPixelRate(95300000);
    err = camera.PCO_ArmCamera();

    err = grabber.Open_Grabber(0);
    if(err == PCO_NOERROR){
    }   else    {
      printf("Could not open grabber. Msg: 0x%x\n",err);
      fflush(stdout);
      camera.Close_Cam();
      continue;
    }

    //If everything is OK here, set some internal parameters and exit
    if (err == OK){
      camera_open = 1;
      camera_acquiring = 0;

      break;
    }
  } //End of for loop
  //Check whether we arrived to MAX_RETRY without success
  if (att == 10) {
    printf("Reached maximum number of attempts to connect to the camera.\n");
    return;
  }
  return;
};

// Destructor: closes camera and grabber
fireball_camera2::~fireball_camera2(){
  uint32_t err = 1;   // To check function returns (0==OK)

  // Test what happens when setting recording state to 0, or resetting camera prior to closing

  // Closes grabber
  err = grabber.Close_Grabber();
  if (err!=0){
    printf("Could not close grabber \n");
    exit(-1);
  };
  // Closes camera
  err = camera.Close_Cam();
  if(err!=0){
    printf("Could not close camera \n");
    exit(-1);
  };
  return;
};




// Setting the camera parameters
int fireball_camera2::set_params(){
  uint32_t delay, exposure;
  uint16_t binx, biny;
  //Sets recording state to OFF
  //err = camera.PCO_SetRecordingState(0);
  //Sets delay & exposure times and prints it on screen
  camera.PCO_SetDelayExposure(0,33);
  camera.PCO_GetDelayExposure(&delay, &exposure);
  printf("Delay = %d ms, exposure =  %d ms\n",delay, exposure);
  fflush(stdout);
  //Gets initial, binning, sets it to new and prints
  err = PCO_GetBinning(&camera, &binx, &biny);
  printf("the binning is: %d by %d\n",binx,biny);
  binx = 2;
  biny = 2;
  err = PCO_SetBinning(&camera, binx, biny);
  //After setting binning, it is important to reset ROI to avoid error
  err = PCO_SetROI(&camera,1,1,2560/2,2160/2);
  usleep(500000);
  err = PCO_GetBinning(&camera, &binx, &biny);
  printf("the binning is: %d by %d\n",binx,biny);
  //Finally, except in the binning 1x1, we can afford to use higher pixel rate
  err = camera.PCO_SetPixelRate(286000000);

  return 0;
};

// These 2 functions should substitute set_params()

int fireball_camera2::cam_config(){
  uint32_t delay, exposure, pixelrate;
  uint16_t binx, biny;
  //Need to add check on binning, pixelrate, delay and exposure
  //Set initial delay and exposure times (can be changed later)
  camera.PCO_SetDelayExposure(DELAY,EXPOSURE);
  //Set binning
  err = PCO_SetBinning(&camera, BIN_X, BIN_Y);
  err = PCO_SetROI(&camera, 1, 1, FULL_WIDTH/BIN_X, FULL_HEIGHT/BIN_Y);
  //Set pixel rate
  err = camera.PCO_SetPixelRate(PIXELRATE);
  //Print new settings
  camera.PCO_GetDelayExposure(&delay, &exposure);
  err = PCO_GetBinning(&camera, &binx, &biny);
  //err = PCO_GetPixelRate(&pixelrate);
  printf("Camera configured:\nDelay = %d ms, Exposure = %d ms\nBinning %d x %d\nPixel rate %d\n",delay,exposure,binx,biny,PIXELRATE);
  //Update structure from camera parameters
  ////camera_parameters.act_recstate
  ////camera_parameters.loopcount
  camera_parameters.width = FULL_WIDTH/BIN_X;
  camera_parameters.height = FULL_HEIGHT/BIN_Y;
  printf("Image width = %d pixels, image height = %d pixels\n",camera_parameters.width, camera_parameters.height);
  camera_parameters.exp = exposure;
  camera_parameters.delay = delay;
  ////camera_parameters.timeout
  camera_parameters.pixelrate = pixelrate;
  ////camera_parameters.picbuf

  return 0;
}

int fireball_camera2::set_exposure(uint32_t exposure){
  //Maybe we want to move these params to the headfile in #defines?
  uint32_t delay = DELAY;
  uint32_t exp_min = 1;  // In reality it can be 0.5 ms but let's keep it in integers in ms for simplicity
  uint32_t exp_max = 2000;
  uint32_t old_delay, old_exp;
  if (exposure >= exp_min  && exposure <= exp_max){
    err = camera.PCO_GetDelayExposure(&old_delay, &old_exp);
    printf("Previous exposure = %d ms\n", old_exp);
    err = camera.PCO_SetDelayExposure(delay,exposure);
    camera_parameters.exp = exposure;
    err = camera.PCO_GetDelayExposure(&delay,&exposure);
    printf("Exposure set to %d ms\n",exposure);
    //Update camera parameters
    camera_parameters.exp = exposure;
    camera_parameters.delay = delay;
  }
  else {
    printf("Exposure needs to be an integer between [%d, %d] ms\n",exp_min,exp_max);
  }

  return 0;
}


// Start and stop acquisition
int fireball_camera2::start(){
  uint32_t att;
  uint32_t height, width, pixelrate;
  // Arms camera and prepares grabber with set parameters,
  // we do a loop as with the constructor to allow it to try several times
  err = 0;
  for (att=1; att<=MAX_RETRY && err!= FATAL; att++){
    // To set the transfer parameters, first needs to get actual size and pixel rate
    err = camera.PCO_ArmCamera();
    if(err == PCO_NOERROR)  {   
    }   
    else {
    printf("Could not arm camera. Msg: 0x%x\n",err);
    }
    err = camera.PCO_GetActualSize(&width,&height);
    if(err == PCO_NOERROR)  {
      printf("Image size is %ux%u.\n",width,height);
    }   
    else  {
      printf("Could not get image size. Msg: 0x%x\n",err);
    }
    err = camera.PCO_GetPixelRate(&pixelrate);
    if(err == PCO_NOERROR)  {
      printf("Pixel rate is %u.\n",pixelrate);
    }   
    else  {
      printf("Could not get pixel rate from camera. Msg: 0x%x\n",err);
    }
    // Sets transfer parameters
    if((width>1920)&&(pixelrate>=286000000))  {
      clpar.DataFormat=SCCMOS_FORMAT_TOP_CENTER_BOTTOM_CENTER|PCO_CL_DATAFORMAT_5x12;
      printf("width>1920 %d && pixelrate >=286000000 %d Dataformat 0x%x.\n",width,pixelrate,clpar.DataFormat);
    }
    else  {
      clpar.DataFormat=SCCMOS_FORMAT_TOP_CENTER_BOTTOM_CENTER|PCO_CL_DATAFORMAT_5x16;
      printf("width<=1920 %d || pixelrate <286000000 %d Dataformat 0x%x(%x).\n",width,pixelrate,clpar.DataFormat,PCO_CL_DATAFORMAT_5x16);
    } 
    err = camera.PCO_SetTransferParameter(&clpar,sizeof(clpar));
    if(err == PCO_NOERROR){  
    }   
    else  {
      printf("Could not set transfer parameter. Msg: 0x%x\n",err);
    }
    // Arms camera
    err = camera.PCO_ArmCamera();
    if(err == PCO_NOERROR){
    }   
    else  {
      printf("Could not arm camera. Msg: 0x%x\n",err);
    }
    // Sets grabber
    err = grabber.Set_DataFormat(clpar.DataFormat);
    if(err == PCO_NOERROR){
    }
    else  {
      printf("Could not set data format. Msg: 0x%x\n",err);
    }
    printf("Setting image size expected by the grabber to %dx%d\n",width,height);
    err = grabber.Set_Grabber_Size(width,height);
    if(err == PCO_NOERROR) {
    }
    else {
      printf("Could not set image size on grabber. Msg: 0x%x\n",err);
    }
    // Allocates an image buffer, tries initially with 20 images, goes down after
    unsigned int i = 0;
    err = WARNING;
    for(i = 20; i > 0 && err != OK; i--)  { //OK should mean that the framebuffer was allocated
      if((err = grabber.Allocate_Framebuffer(i)) == PCO_NOERROR){
      }
      else  {
        if (i > 1){
          printf("Could not allocate frame buffer for %d frames. Trying again for %d frame buffers. Msg: 0x%x",i,i-1,err);
        } 
        else {
          printf("Could not allocate frame buffer at all! Msg: 0x%x\n",err);
          return -1;  //Returns an error value
        }
      }
    }
    // Sets the recording state to OFF before we update some parameters
    err = camera.PCO_SetRecordingState(0);
    if(err == PCO_NOERROR){
    }
    else {
      printf("Could not set recording state. Msg; 0x%x",err);
    }
    // If everything is OK, exist the loop (no need to keep trying)
    if (err == OK)
      break;
  }
  // After the for loop, check if we reached MAS_RETRY without success
  if (att == 10) {
    printf("Reached maximum number of attempts to connect to the camera.\n");
    return -1;  //Returns an error value
  }
  // Updates pixelrate
  camera_parameters.pixelrate = pixelrate;
  // After updatiing parameters we set the Recording state to ON to be ready to record
  err = camera.PCO_SetRecordingState(1);
  // Quick check on dimensions of image
  printf("xx %dx%d\n",width,height);
  // Allocate memory to image buffer if using 12 bit packing CHECK THIS PART!!!
  camera_parameters.picbuf = (uint16_t*)malloc(width*height*sizeof(uint16_t));
  // Everything went fine
  return 0;
}

int fireball_camera2::stop(){
  // if using 12 bit packing.
  
  //  int err = grabber.Stop_Aquire();
  //  if(err!=0){
  //    printf("Could not stop acquiring single frame\n");
  //    exit(-10);
  //  };
  // Free the memory allocated on start()
  free(camera_parameters.picbuf);
  err = camera.PCO_SetRecordingState(0);
  return 0;
};


int fireball_camera2::grab_single(){
  // Getting the image from the grabber
  err = grabber.Start_Aquire(picnum);
  if(err!=0){
    printf("Could not start acquiring image \n");
    exit(-6);
  };
  err = grabber.Wait_For_Image(&picnum,timeout);
  if(err!=0){
    printf("No image after %d ms waiting\n", timeout);
    exit(-7);
  };
  // This slows down the function, but if we start getting errors, may need to uncomment it
  //  err = grabber.Check_DMA_Length(picnum);
  //  if(err!=0){
  //    printf("Check DMA lenght error\n");
  //    exit(-8);
  //  };
  err = grabber.Get_Framebuffer_adr(picnum,(void**)&adr);
  if(err!=0){
    printf("Could not get frame buffer address\n");
    exit(-9);
  };
  err = grabber.Stop_Aquire();
  if(err!=0){
    printf("Could not stop acquiring single frame\n");
    exit(-10);
  };

  // Copying the image in memory
  // Using this function slows down the guider (copies the image), but yields good image
  // If we need to access directly the image through the pointer given by the camera, we need to figure out the format (12 bit)
  err = grabber.Get_actual_size(&camera_parameters.width,&camera_parameters.height,NULL);
  grabber.Extract_Image(camera_parameters.picbuf, adr,camera_parameters.width,camera_parameters.height);
  // If we decide to point directly at the pointer, ue the following below
  //camera_parameters.picbuf = adr; 
  //  printf("%d\n",camera_parameters.picbuf[10000]);

  // Only if we want to store the image in a tif file
  //char fname[50];
  //sprintf(fname,"arrayx.tif");
  //store_tif(&fname[0],camera_parameters.width, camera_parameters.height, 0, camera_parameters.picbuf);

  return 0;
};


// Other functions

int fireball_camera2::temperature(){
  err=1;     // To check function returns (0==OK)
  short ccdtemp, camtemp, pstemp;
  err = camera.PCO_GetTemperature(&ccdtemp,&camtemp,&pstemp);
  if (err!=0){
    printf("Could not get camera temperature");
  }
  else {
    printf("Camera temperatures: \n");
    printf("Sensor:       %d\n",ccdtemp);
    printf("Camera:       %d\n",camtemp);
    printf("Power supply: %d\n",pstemp);
  };
  return 0;
};



uint32_t PCO_GetBinning(CPco_cl_com *cam, uint16_t *binningx, uint16_t *binningy){
  SC2_Simple_Telegram com;
  SC2_Binning_Response resp;
  uint32_t err;
  
  com.wCode = GET_BINNING;
  com.wSize = sizeof(com);
  
  err = (*cam).Control_Command(&com,sizeof(com),&resp,sizeof(resp));
  if(err==PCO_NOERROR){
    *binningx = resp.wBinningx;
    *binningy = resp.wBinningy;
  };
  return err;
};

uint32_t  PCO_SetBinning(CPco_cl_com *cam, uint16_t binningx, uint16_t binningy){
  SC2_Set_Binning com;
  SC2_Binning_Response resp;
  uint32_t err;

  com.wCode = SET_BINNING;
  com.wSize = sizeof(com);
  com.wBinningx = binningx;
  com.wBinningy = binningy;

  err = (*cam).Control_Command(&com,sizeof(com),&resp,sizeof(resp));
  
  return err;
};


uint32_t PCO_SetROI(CPco_cl_com *cam, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1){
  SC2_Set_ROI com;
  SC2_ROI_Response resp;
  uint32_t err;
  com.wCode = SET_ROI;
  com.wSize = sizeof(com);
  com.wROI_x0 = x0;
  com.wROI_y0 = y0;
  com.wROI_x1 = x1;
  com.wROI_y1 = y1;

  err = (*cam).Control_Command(&com,sizeof(com),&resp,sizeof(resp));

  return err;
};
