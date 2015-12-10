#include "fireball_camera2.h"

// Constructor: opens camera and grabber and gets them ready

CPco_Log pco_log("pco_edge_grab.log");

fireball_camera2::fireball_camera2(){
  unsigned int err=1;   // To check function returns (0==OK)
  uint32_t att=1;       // To keep track of open attempts
  //uint32_t MAX_OPEN=10; // Maximum opening attempts we want to allow
  //uint32_t width, height, pixelrate;
  //*************

  picnum = 1;
  timeout = 10;
   
  /* Tries to open camera MAX_RETRY times. If MAX_RETRY attempts
     are made of a FATAL error is risen, the function is shut. */
  // MAX_RETRY is defined in the .h file
  for (att = 1; att <= MAX_RETRY && err != FATAL; att++)   {
    printf("Attempt #%3d of opening the camera.\n",att);
    
    //Debug - JJG
    sleep(1);    
    //printf("Attempt!\n\n");
    fflush(stdout);        
    
      //    camera.log  = &pco_log;
      //    grabber.log = &pco_log;
    
    //printf("err before Open_Cam: %d\n",err);
    err = camera.Open_Cam(0);
    //printf("err after Open_Cam: %d\n",err);
    if(err == PCO_NOERROR)  {
    }   else    {                  
      printf("Could not open camera. Msg: 0x%x\n",err);
      fflush(stdout);
      continue;
    }
    //printf("err before SetRecordingState: %d\n",err);
    err = camera.PCO_SetRecordingState(0);
    //printf("err after SetRecordingState: %d\n",err);
    //printf("err before Open_Grabber: %d\n",err);
    err = grabber.Open_Grabber(0);
    //printf("err after Open_Grabber: %d\n",err);
    if(err == PCO_NOERROR){
    }   else    {
      printf("Could not open grabber. Msg: 0x%x\n",err);
      fflush(stdout);
      camera.Close_Cam();
      continue;
    }

    /*
        */
	//printf("err before check lo leave for loop: %d\n",err);
        if (err == OK)
            break;
       // */  ////////
    }
    printf("Attempts needed: %d\n",att);
    
	/* If it reaches the end of the function with after attempting MAX_RETRY times,
	 it means it wasn't successful. */
    if (att == 10) {
	printf("Reached maximum number of attempts to connect to the camera.\n");
	return;
    }


    return;
};

// Destructor: closes camera and grabber

fireball_camera2::~fireball_camera2(){
  err = 1;   // To check function returns (0==OK)
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




// Set parameters
//int fireball_camera::set_params(camera_parameters_t *campar){
int fireball_camera2::set_params(){
  uint32_t delay, exposure;
  //  stop();
  //do stuff
  camera.PCO_SetDelayExposure(2,1000);
  camera.PCO_GetDelayExposure(&delay, &exposure);
  printf("%d %d\n\n",delay, exposure);
  fflush(stdout);
  //  start();
  return 0;
};

// Start and stop acquisition
int fireball_camera2::start(){
  uint32_t att=1;
  err=1; 
  uint32_t height, width, pixelrate;
  //Tries to start the camera a set of times, if it cannot it raises an error
  for (att = 1; att <= MAX_RETRY && err != FATAL; att++){
    //Arms camera
    err = camera.PCO_ArmCamera();
    if(err == PCO_NOERROR){
    } 
    else {
      printf("Could not arm camera. Msg: 0x%x\n",err);
    }
    //Sets the transfer parameters, needing actual size and pixel rate
    err = camera.PCO_GetActualSize(&width,&height);
    if (err == PCO_NOERROR) {
      printf("Image size is %ux%u.\n",width,height);
    } 
    else {
      printf("Could not get image size. Msg: 0x%x\n",err);
    }
    err = camera.PCO_GetPixelRate(&pixelrate);
    if(err == PCO_NOERROR)  {
      printf("Pixel rate is %u.\n",pixelrate);
    } 
    else {
      printf("Could not get pixel rate from camera. Msg: 0x%x\n",err);
    }
    if((width>1920)&&(pixelrate>=286000000)) {
      clpar.DataFormat=SCCMOS_FORMAT_TOP_CENTER_BOTTOM_CENTER|PCO_CL_DATAFORMAT_5x12;
      printf("width>1920 %d && pixelrate >=286000000 %d Dataformat 0x%x.\n",width,pixelrate,clpar.DataFormat);
    } 
    else {
      clpar.DataFormat=SCCMOS_FORMAT_TOP_CENTER_BOTTOM_CENTER|PCO_CL_DATAFORMAT_5x16;
      printf("width<=1920 %d || pixelrate <286000000 %d Dataformat 0x%x(%x).\n",width,pixelrate,clpar.DataFormat,PCO_CL_DATAFORMAT_5x16);
    }
    err = camera.PCO_SetTransferParameter(&clpar,sizeof(clpar));
    if(err == PCO_NOERROR) {
    } 
    else  {
      printf("Could not set transfer parameter. Msg: 0x%x\n",err);
    }
    // Arms camera again, now with the correct transfer parameters
    err = camera.PCO_ArmCamera();
    if (err == PCO_NOERROR) {
    }   
    else {
      printf("Could not arm camera. Msg: 0x%x\n",err);
    }

    // Sets data format and size for the grabber
    err = grabber.Set_DataFormat(clpar.DataFormat);
    if (err == PCO_NOERROR) {
    }
    else {
      printf("Could not set data format. Msg: 0x%x\n",err);
    }    
    printf("Setting image size expected by the grabber to %dx%d\n",width,height);
    err = grabber.Set_Grabber_Size(width,height);
    if (err == PCO_NOERROR) {
    }
    else {
      printf("Could not set image size on grabber. Msg: 0x%x\n",err);
    }
    //Attemps to allocate a frame buffer for 20 frames. If not succesful,
    //tries with less frames until 1
    unsigned int i = 0;
    err = WARNING;      //Defined in header file
    for (i = 20; i > 0 && err != OK; i--) { //OK should mean that the framebuffer was allocated
      if ((err = grabber.Allocate_Framebuffer(i)) == PCO_NOERROR){
      }
      else  {
        if (i > 1) {
          printf("Could not allocate frame buffer for %d frames. Trying again for %d frame buffers. Msg: 0x%x",i,i-1,err);
        } 
        else {
          printf("Could not allocate frame buffer at all! Msg: 0x%x\n",err);
          return -1;
        }
      }
    }
    //Sets the recording state of the camera to "OFF"
    err = camera.PCO_SetRecordingState(0); // REMOVE?
    if(err == PCO_NOERROR){
    }
    else {
      printf("Could not set recording state. Msg; 0x%x",err);
    }
    // If everything is OK, it leaves the loop    
    if (err == OK)
      break;
  }   // End of the for look for MAX_TRY
  /* If it reaches the end of the function with after attempting MAX_RETRY times,
  it means it wasn't successful. */
  if (att == 10) {
    printf("Reached maximum number of attempts to connect to the camera.\n");
  return -1;
  }

  camera_parameters.pixelrate = pixelrate; //MOVE UPWARDS?
  err = camera.PCO_SetRecordingState(1);
  printf("xx %dx%d\n",width,height);
  //  allocate memory to image buffer if using 12 bit packing
  camera_parameters.picbuf = (uint16_t*)malloc(width*height*sizeof(uint16_t));
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
  err = grabber.Check_DMA_Length(picnum);
  if(err!=0){
    printf("Check DMA lenght error\n");
    exit(-8);
  };
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

  // Storing the image in memory
  err = grabber.Get_actual_size(&camera_parameters.width,&camera_parameters.height,NULL);
  // will need to keep this if using 12bit stuff
  printf("%d\n",camera_parameters.picbuf[10000]);
  //  grabber.Extract_Image(camera_parameters.picbuf, adr,camera_parameters.width,camera_parameters.height);
  printf("%d\n",camera_parameters.picbuf[10000]);

  // Storing the image in a fits file
  char fname[50];
  sprintf(fname,"arrayz.tif");
  //store_tif(&fname[0],camera_parameters.width, camera_parameters.height, 0, camera_parameters.picbuf);

  camera_parameters.picbuf = adr;  

  // Freeing the memory (should be moved out of this function)
  //free(picbuf);
  return 0;
};


int fireball_camera2::stop(){
  // if using 12 bit packing.
  free(camera_parameters.picbuf);
  err = camera.PCO_SetRecordingState(0);
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
