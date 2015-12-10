#include "fireball_camera.h"
//#include "Cpco_cl_com.h"
//#include "Cpco_me4.h"
//#include "pco_cl_include.h"
//#include "../siso_include/clser.h"

//#include "VersionNo.h"      // Version definition
//#include "file12.h"         // Functions regarding image storage


// Constructor: opens camera and grabber and gets them ready


CPco_Log pco_log("pco_edge_grab.log");



void fireball_camera::fireball_camera_cam(){
  //  start_camera(camera, grabber);
};


fireball_camera::fireball_camera(){

  // Creates an object of type CPco_cl_com for the camera (PCO lib),
  // tries to open it MAX_OPEN times, and prints error if fails

  err=1;       // To check function returns (0==OK)
  uint32_t att=1;       // To keep track of open attempts
  uint32_t MAX_OPEN=10; // Maximum opening attempts we want to allow
  uint32_t width, height, pixelrate;
  //*************

  picnum = 1;
  timeout = 10;
   
  /* Tries to open camera MAX_RETRY times. If MAX_RETRY attempts
     are made of a FATAL error is risen, the function is shut. */
  for (att = 1; att <= MAX_RETRY && err != FATAL; att++)   {
    printf("Attempt #%3d of opening the camera.\n",att);

    //Debug - JJG
    sleep(1);    
    printf("Attempt!\n\n");
    fflush(stdout);        
    
      //    camera.log  = &pco_log;
      //    grabber.log = &pco_log;
    
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

    uint16_t binx, biny;
    err = PCO_GetBinning(&camera, &binx, &biny);
    printf("the binning is: %d by %d\n",binx, biny);
    binx = 2;
    biny = 2;
    err = PCO_SetBinning(&camera, binx, biny);
    err = PCO_SetROI(&camera,1,1,2560/2,2160/2);
    printf("error = 0x%x\n",err);
    usleep(500000);
    printf("Yup. Here.\n");
    err = PCO_GetBinning(&camera, &binx, &biny);
    printf("the binning is: %d by %d\n",binx, biny);
    err = camera.PCO_SetPixelRate(286000000);

    

      


	
    err = camera.PCO_ArmCamera();
        if(err == PCO_NOERROR)  {
	  
        }   else    {
            printf("Could not arm camera. Msg: 0x%x\n",err);
        }
        
	err = camera.PCO_GetActualSize(&width,&height);
        if(err == PCO_NOERROR)	{
            printf("Image size is %ux%u.\n",width,height);
        }   else    {
            printf("Could not get image size. Msg: 0x%x\n",err);
        }
        


        err = camera.PCO_GetPixelRate(&pixelrate);
        if(err == PCO_NOERROR)	{
            printf("Pixel rate is %u.\n",pixelrate);
	}   else    {
            printf("Could not get pixel rate from camera. Msg: 0x%x\n",err);
        }
    
	
        if((width>1920)&&(pixelrate>=286000000))	{
	  clpar.DataFormat=SCCMOS_FORMAT_TOP_CENTER_BOTTOM_CENTER|PCO_CL_DATAFORMAT_5x12;
	  printf("width>1920 %d && pixelrate >=286000000 %d Dataformat 0x%x.\n",width,pixelrate,clpar.DataFormat);
        }
        else                                        {
	  clpar.DataFormat=SCCMOS_FORMAT_TOP_CENTER_BOTTOM_CENTER|PCO_CL_DATAFORMAT_5x16;
	  printf("width<=1920 %d || pixelrate <286000000 %d Dataformat 0x%x(%x).\n",width,pixelrate,clpar.DataFormat,PCO_CL_DATAFORMAT_5x16);
        }
        
        err = camera.PCO_SetTransferParameter(&clpar,sizeof(clpar));
        if(err == PCO_NOERROR){
        
        }   else    {
            printf("Could not set transfer parameter. Msg: 0x%x\n",err);
        }
        
        err = camera.PCO_ArmCamera();
        if(err == PCO_NOERROR){
        }   else    {
	  printf("Could not arm camera. Msg: 0x%x\n",err);
        }
	
        err = grabber.Set_DataFormat(clpar.DataFormat);
        if(err == PCO_NOERROR){
	}
        else                    {
	  printf("Could not set data format. Msg: 0x%x\n",err);
        }
        
	printf("Setting image size expected by the grabber to %dx%d\n",width,height);
        err = grabber.Set_Grabber_Size(width,height);
        if(err == PCO_NOERROR)   {
	}
        else                    {
            printf("Could not set image size on grabber. Msg: 0x%x\n",err);
        }
        
		/* Attempts to allocate a frame buffer for 20 frames. If not successful,
		   tries the same for n-1 frames while n>0. */
        unsigned int i = 0;
        err = WARNING;
        for(i = 20; i > 0 && err != OK; i--)	{ //OK should mean that the framebuffer was allocated
	  if((err = grabber.Allocate_Framebuffer(i)) == PCO_NOERROR){
	  }
            else				{
				if (i > 1){
				  printf("Could not allocate frame buffer for %d frames. Trying again for %d frame buffers. Msg: 0x%x",i,i-1,err);
				} else {
				  printf("Could not allocate frame buffer at all! Msg: 0x%x\n",err);
				  return;
				}
            }
        }
		
        err = camera.PCO_SetRecordingState(0);
        if(err == PCO_NOERROR){
	}
        else                    {
            printf("Could not set recording state. Msg; 0x%x",err);
        }
        
        if (err == OK)
            break;
    }
    
	/* If it reaches the end of the function with after attempting MAX_RETRY times,
	 it means it wasn't successful. */
    if (att == 10) {
	printf("Reached maximum number of attempts to connect to the camera.\n");
	return;
    }


    return;


  //**************

//  camera.log = &pco_log;
 // grabber.log = &pco_log;

//  while(err!=0 && att<=MAX_OPEN){
//   err = camera.Open_Cam(0);
//   ++att;
// }
  // If after MAX_OPEN attempts it cannot open, print message
//  if (err!=0) { 
//   printf("Failed to open camera after %d attempts \n", MAX_OPEN);
//   exit(-1);
// };


//  printf("%d\n",PCO_NOERROR);

//  uint32_t serialnumber;
//  uint16_t camtype;
//  camera.PCO_GetCameraType(&camtype, &serialnumber);
//  printf("cam %d, sn %d\n",camtype, serialnumber);

  // Creates an object of type CPco_me4_edge for grabber (PCO lib)

//  err=1;
// att=1;
// while(err!=0 && att<=MAX_OPEN){
///   err = grabber.Open_Grabber(0);
//  printf("grabber error: %x\n",err);
//  fflush(stdout);
//  ++att;
//}
//if (err!=0) {
//  printf("Failed to open grabber after %d attempts \n", MAX_OPEN);
//  exit(-1);
//};
//picnum = 1;
//timeout = 10;
};

// Destructor: closes camera and grabber

fireball_camera::~fireball_camera(){
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
int fireball_camera::set_params(){
  uint32_t delay, exposure;
  //  stop();
  //do stuff
  camera.PCO_SetDelayExposure(0,33);
  camera.PCO_GetDelayExposure(&delay, &exposure);
  printf("%d %d\n\n",delay, exposure);
  fflush(stdout);
  //  start();
  return 0;
};

// Start and stop acquisition
int fireball_camera::start(){
  uint32_t height, width, pixelrate;
  // Arms camera and prepares grabber with set parameters,
  // starts an infinite loop grabbing images, does not store any

  // Arms camera to check image size and pixel rate
  err = camera.PCO_ArmCamera();
  if(err!=0){
    printf("Cannot arm camera\n");
    exit(-2);
  };

  // Gets actual size of images acquired by camera
  err = camera.PCO_GetActualSize(&width,&height);
  if(err!=0){ 
    printf("Cannot get actual image size\n");
    exit(-2);
  };
  camera_parameters.height = height;
  camera_parameters.width = width;
  // Gets actual pixel rate from camera
  
  err = camera.PCO_GetPixelRate(&pixelrate);
  if(err!=0){
    printf("Cannot get pixel rate\n");
    exit(-2);
  };
  // Determines correct transfer data format
  if((width>1920)&&(pixelrate>=286000000)){
    clpar.DataFormat = SCCMOS_FORMAT_TOP_CENTER_BOTTOM_CENTER|PCO_CL_DATAFORMAT_5x12;
  }
  else{
    clpar.DataFormat = SCCMOS_FORMAT_TOP_CENTER_BOTTOM_CENTER|PCO_CL_DATAFORMAT_5x16;
  }
  // Sets correct transfer data format
  err = camera.PCO_SetTransferParameter(&clpar,sizeof(clpar));
  if(err!=0){
    printf("Cannot set transfer parameters\n");
    exit(-3);
  };
  // Arms the camera to start image acquisition
  err = camera.PCO_ArmCamera();
  if(err!=0){
    printf("Cannot arm camera\n");
    exit(-3);
  };


  // Sets transfer data format to grabber
  err = grabber.Set_DataFormat(clpar.DataFormat);
  if(err!=0){
    printf("Cannot set data format on grabber\n");
    exit(-4);
  };
  // Sets grabber size
  err = grabber.Set_Grabber_Size(width,height);
  if(err!=0){
    printf("Cannot set grabber size\n");
    exit(-4);
  };
  // Allocates frame buffer
  //  err = grabber.Allocate_Framebuffer(20);
  //  if(err!=0){
  //    printf("Cannot allocate frame buffer\n");
  //    exit(-5);
  //  };

  camera_parameters.pixelrate = pixelrate;

  err = camera.PCO_SetRecordingState(1);

  /*
  err = grabber.Start_Aquire(picnum);
  if(err!=0){
    printf("Could not start acquiring image \n");
    exit(-6);
  };
  */

  printf("xx %dx%d\n",width,height);
  //  allocate memory to image buffer if using 12 bit packing
  camera_parameters.picbuf = (uint16_t*)malloc(width*height*sizeof(uint16_t));

  return 0;
};


int fireball_camera::grab_single(){
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
  

  // Storing the image in memory
  err = grabber.Get_actual_size(&camera_parameters.width,&camera_parameters.height,NULL);
  // will need to keep this if using 12bit stuff
  //  printf("%d\n",camera_parameters.picbuf[10000]);
  grabber.Extract_Image(camera_parameters.picbuf, adr,camera_parameters.width,camera_parameters.height);
  //  printf("%d\n",camera_parameters.picbuf[10000]);

  char fname[50];
  sprintf(fname,"arrayx.tif");
    

  //  store_tif(&fname[0],camera_parameters.width, camera_parameters.height, 0, camera_parameters.picbuf);

  //camera_parameters.picbuf = adr;  

  // Freeing the memory (should be moved out of this function)
  //free(picbuf);
  return 0;
};


int fireball_camera::stop(){
  // if using 12 bit packing.
  
  //  int err = grabber.Stop_Aquire();
  //  if(err!=0){
  //    printf("Could not stop acquiring single frame\n");
  //    exit(-10);
  //  };
  free(camera_parameters.picbuf);
  err = camera.PCO_SetRecordingState(0);
  return 0;
};

// Other functions

int fireball_camera::temperature(){
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
