#include "Cpco_cl_com.h"                                                                 
#include "Cpco_me4.h"                                                                        
#include "pco_cl_include.h" 
#include "../siso_include/clser.h"

CPco_Log pco_log("pco_edge_grab.log");

CPco_cl_com camera;
CPco_me4_edge grabber;
PCO_SC2_CL_TRANSFER_PARAM clpar;

int main(){
  int err;
  uint32_t ret, width, height;
  uint16_t ret2;
  char infostr[100];
  
  
  err = camera.Open_Cam(0);
  printf("%d: 0x%x\n",__LINE__,err);

  err=camera.PCO_GetCameraType(&ret2,&ret);
  printf("Cam: %d, SN: %d\n",ret2, ret);


  err = grabber.Open_Grabber(0);
  printf("%d: 0x%x\n",__LINE__,err);

  err=camera.PCO_GetInfo(1,infostr,sizeof(infostr));
  printf("Camera Name is: %s\n",infostr);

  err=camera.PCO_GetTransferParameter(&clpar,sizeof(clpar));
  if(err!=PCO_NOERROR)
    printf("PCO_GetTransferParameter() Error 0x%x\n",err);
  else
    {
      printf("Baudrate      : %d\n",clpar.baudrate);
      printf("Clockfrequency: %d\n",clpar.ClockFrequency);
      printf("Dataformat    : 0x%x\n",clpar.DataFormat);
      printf("Transmit       : 0x%x\n",clpar.Transmit);
    }


  err=camera.PCO_GetPixelRate(&ret);
  printf("%d: %d:  0x%x\n",__LINE__,ret, err);

  err=camera.PCO_SetRecordingState(0);

  err=camera.PCO_GetActualSize(&width,&height);
  printf("%d: %d x %d\n",__LINE__,width,height);

  clpar.DataFormat=SCCMOS_FORMAT_TOP_CENTER_BOTTOM_CENTER|PCO_CL_DATAFORMAT_5x16;
  //  clpar.DataFormat=SCCMOS_FORMAT_TOP_CENTER_BOTTOM_CENTER|PCO_CL_DATAFORMAT_5x12;
  err = camera.PCO_SetTransferParameter(&clpar,sizeof(clpar));
  printf("%d: 0x%x\n",__LINE__,err);


  int x;
  SC2_Camera_Description_Response description;
  uint32_t pixelrate=0;
  err=camera.PCO_GetCameraDescriptor(&description);
  printf("%d: 0x%x\n",__LINE__,err);
  for(x=0;x<4;x++)
    {
      if(description.dwPixelRateDESC[x]!=0)
	{
	  printf("%d: %d\n",x,description.dwPixelRateDESC[x]);
	  if(pixelrate<description.dwPixelRateDESC[x])
	    pixelrate=description.dwPixelRateDESC[x];
	}
    }


  err=camera.PCO_SetPixelRate(description.dwPixelRateDESC[x]);
  printf("%d: 0x%x\n",__LINE__,err);
  if(err!=0) {
    sleep(1);
    grabber.Close_Grabber();
    sleep(1);
    camera.Close_Cam();
  };

  camera.PCO_SetDelayExposure(0,33);
  printf("%d: 0x%x\n",__LINE__,err);


  err=camera.PCO_SetRecordingState(0);
  printf("%d: 0x%x\n",__LINE__,err);


  
  err=camera.PCO_SetRecordingState(0);
  printf("%d: 0x%x\n",__LINE__,err);




  err = camera.Close_Cam();
  printf("%d: 0x%x\n",__LINE__,err);

  return 0;
};