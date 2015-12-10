

//I will move this to a header file once it works
#include <iostream>				//I want to output to the console
#include <chrono>					//For timing performance

#include "Cpco_cl_com.h"		//I need the camera class from PCO
#include "Cpco_me4.h"			//I need the grabber class from PCO
#include "VersionNo.h"			//Includes version of the drivers used
#include "file12.h"				//To save the image from the camera

#ifndef HOMEPATH
#define HOMEPATH "/home/salo/0.Jose/2.mintguider/0.Versions/2015.07.15/mint_camera_only"
#endif

int img_nr_from_tmstamp (void *buf,int shift);
//End of includes

using namespace std;				//I want to output to the console
using namespace std::chrono;	//For timing performance

int main(){
	cout << "------------------------------------------------------------------\n";
	cout << "Program to measure performance of image grabbing with PCO.Edge 5.5\n";
	cout << "------------------------------------------------------------------\n";
	
	//I will use a variable err to check that everything works
	DWORD err=0;
	//Create and open camera object
	CPco_cl_com camera;
	cout << "Opening camera...";
	err=camera.Open_Cam(0);
	(err==0)? cout << "opened\n" : cout << "Open_Cam() Error\n";
	//Create and open grabber object
	CPco_me4_edge grabber;
	cout << "Opening grabber...";
	err=grabber.Open_Grabber(0);
	(err==0)? cout << "opened\n" : cout << "Open_Grabber() Error\n";
	//Set data format depending on sensor width and pixelrate
	cout << "Setting data format...\n";
	PCO_SC2_CL_TRANSFER_PARAM clpar;			//To store the transfer parameters
	DWORD width, height;
	err=camera.PCO_GetActualSize(&width,&height);
	if (err!=0) cout << "PCO_GetActualSize() Error\n";
	DWORD pixelrate;
	err=camera.PCO_GetPixelRate(&pixelrate);
	if (err!=0) cout << "PCO_GetPixelRate() Error\n";

	if((width>1920)&&(pixelrate>=286000000)){
   	clpar.DataFormat=SCCMOS_FORMAT_TOP_CENTER_BOTTOM_CENTER|PCO_CL_DATAFORMAT_5x12;
	}
   else{
      clpar.DataFormat=SCCMOS_FORMAT_TOP_CENTER_BOTTOM_CENTER|PCO_CL_DATAFORMAT_5x16;
   }
	cout << "Width = " << width << " pixels, pixel rate = " << pixelrate << " pixels/s, data format = " << clpar.DataFormat << endl;
	err=camera.PCO_SetTransferParameter(&clpar,sizeof(clpar));
	(err==0)? cout << "... transfer parameters set\n" : cout << "PCO_SetTransferParameter() Error\n";
	//Arm camera
	cout << "Arming camera...";
	err=camera.PCO_ArmCamera();
	(err==0)? cout << "armed\n" : cout << "PCO_ArmCamera() Error";
	//Set data format and prepare buffer for grabber
	cout << "Setting data format for grabber...";
	err=grabber.Set_DataFormat(clpar.DataFormat);
	(err==0)? cout << "set\n" : cout << "Set_DataFormat() Error\n";
	cout << "Setting grabber size...";
	err=grabber.Set_Grabber_Size(width,height);
	(err==0)? cout << "set\n" : cout << "Set_Grabber_Size() Error\n";
	cout << "Allocating Framebuffer...";
	int bufnum=20;
	err=grabber.Allocate_Framebuffer(bufnum);
	(err==0)? cout << "allocated to " << bufnum << " pictures\n" : cout << "Allocate_Framebuffer() Error\n";
	//Setting camera mode for recording
	cout << "Setting camera mode for recording...";
	err=camera.PCO_SetRecordingState(1);
	(err==0)? cout << "set\n" : cout << "PCO_SetRecordingState() Error\n";
	cout << "Setting delay and exposure time...\n";
	DWORD delay, exposure;
	delay = 1;
//*************************************************
//EXPOSURE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	exposure = 100;
	err=camera.PCO_SetDelayExposure(delay,exposure);
	err=camera.PCO_GetDelayExposure(&delay,&exposure);
	(err==0)? cout << "Camera wait time = " << delay << " ms, exposure = " << exposure << " ms\n" : cout << "PCO_GetDelayExposure() Error\n";
//GET CAMERA PARAMETERS PRIOR TO PERFORMANCE MEASUREMENT
	cout << "-----------------\n";
	cout << "Camera parameters\n";
	cout << "-----------------\n";
	//Name
	char infocamname[100];
	err=camera.PCO_GetInfo(1,infocamname,sizeof(infocamname));
	(err==0)? cout << "Camera name: " << infocamname << endl : cout << "Problem getting the camera name, PCO_GetInfo()\n";
	//Camera identifier
	WORD infocamtype;
  	DWORD infoserialnumber;
	err=camera.PCO_GetCameraType(&infocamtype,&infoserialnumber);
	(err==0)? cout << "Camera type: " << infocamtype << ", serial number: " << infoserialnumber << endl : cout << "Problem getting camera type\n";
	//LUT (Look Up Table) for color correction, we're not using it, make sure it is zero since we're using a mono camera
	WORD infoidentifier, infoparameter;
	err=camera.PCO_GetLut(&infoidentifier, &infoparameter);
	(err==0)? cout << "LUT identifier: " << infoidentifier << ", LUT parameter: " << infoparameter << endl : cout << "PCO_GetLut() Error\n";
	//Recording state	
	WORD inforecstate;	
	err=camera.PCO_GetRecordingState(&inforecstate);
	(err==0)? cout << "Recording state: " << inforecstate << endl : cout << "Problem getting the camera recording state, PCO_GetRecordingState()\n";
	//Transfer parameters
	PCO_SC2_CL_TRANSFER_PARAM infoclpar;
	err=camera.PCO_GetTransferParameter(&infoclpar,sizeof(infoclpar));
	if(err!=0)
		printf("PCO_GetTransferParameter() Error 0x%x\n",err);
  	else{
		//printf("Camera transfer parameters:\n");
		//printf("---------------------------\n");
   		printf("Baudrate      : %d\n",infoclpar.baudrate);
   		printf("Clockfrequency: %d\n",infoclpar.ClockFrequency);
   		printf("Dataformat    : 0x%x\n",infoclpar.DataFormat);
   		printf("Transmit      : 0x%x\n",infoclpar.Transmit);
  	}
	//Timestamp mode
	WORD infomode;
	err=camera.PCO_GetTimestampMode(&infomode);
	(err==0)? cout << "Timestamp mode: " << infomode << endl : cout << "Cannot get timestamp mode PCO_GetTimestampMode() Error\n";
	//BitAlignment
	WORD infoalign;
	err=camera.PCO_GetBitAlignment(&infoalign);
	(err==0)? cout << "Bit alignment: " << infoalign << endl : cout << "Cannot get bit alignment PCO_GetBitAlignment() Error\n";
	//Correction mode
	//WORD infocorr;
	//err=camera.PCO_GetCorrectionMode(&infocorr);
	//(err==0)? cout << "Correction mode: " << infocorr << endl : cout << "Cannot get correction mode PCO_GetCorrectionMode() Error\n";
	//Binning
	WORD infobinx, infobiny;
	err=camera.PCO_GetBinning(&infobinx,&infobiny);
	(err==0)? cout << "Binning mode: " << infobinx << " x " << infobiny << endl : cout << "Cannot get binning mode PCO_GetBinning() Error\n";
	//Size
	DWORD infowidth, infoheight;
	err=camera.PCO_GetActualSize(&infowidth,&infoheight);
	(err==0)? cout << "Actual size: " << infowidth << " x " << infoheight << endl : cout << "Cannot get actual size PCO_GetActualSize() Error\n";
	//Pixelrate
	DWORD infopixrate;
	err=camera.PCO_GetPixelRate(&infopixrate);
	(err==0)? cout << "Pixel rate: " << infopixrate << endl : cout << "Cannot get pixel rate PCO_GetPixelRate() Error\n";
	//Delay & Exposure
	DWORD infodelay, infoexp;
	err=camera.PCO_GetDelayExposure(&infodelay,&infoexp);
	(err==0)? cout << "Delay: " << infodelay << ", exposure: " << infoexp << endl : cout << "Cannot get delay & exposure PCO_GetDelayExposure Error\n";
	//Time base
	WORD infotdelay, infotexp;
	err=camera.PCO_GetTimebase(&infotdelay,&infotexp);
	(err==0)? cout << "Delay timebase: " << infotdelay << ", exposure timebase: " << infotexp << endl : cout << "PCO_GetTimebase() Error\n";
	//COC Runtime
	DWORD infos, infons;
	err=camera.PCO_GetCOCRuntime(&infos,&infons);
	(err==0)? cout << "s: " << infos << ", ns: " << infons << endl : cout << "PCO_GetCOCRuntime() Error\n";
	//Temperature
	SHORT infoccdtemp,infocamtemp,infopstemp;
	err=camera.PCO_GetTemperature(&infoccdtemp,&infocamtemp,&infopstemp);
  	if(err!=PCO_NOERROR)
   		printf("PCO_GetTemperature() Error 0x%x\n",err);
  	else{
   		printf("Current temperatures (C):\n");
   		if(infoccdtemp == (SHORT)(-32768))
    			printf("no sensor temperature\n");
   		else
   			cout << "CCD temperature = " << double(infoccdtemp)/10.0 << ", camera temperature = " << infocamtemp << ", power supply temperature = " << 	infopstemp << endl;
  	}
	cout << "-----------------\n";
/*
Get_Transfer_Parameters(void *buffer,int len);
 DWORD PCO_GetCameraDescriptor(SC2_Camera_Description_Response *description);
*/
	//Grab images
	cout << endl;
	//Start aquisition, wait for image to be retrieved
	int picnum=1;
	//Measuring time taken to take and read a certain number of frames
	int nframes = 100;
	cout << "Measuring time to take " << nframes << " frames...\n";
	high_resolution_clock::time_point t1 = high_resolution_clock::now();
	//Variables to be used within the loop declared here for scope reasons
	WORD *adr;
	unsigned int act_width, act_height;
	WORD *picbuf = NULL;
	//I first get nframes without saving them to measure performance
	for (int i=0; i<nframes; i++){
		//cout << "Setting grabber to aquire image...";
		err=grabber.Start_Aquire(1);
		//(err==0)? cout << "set\n" : cout << "StartAquire() Error\n";
		//cout << "Grabber waiting for image...";
		err=grabber.Wait_For_Image(&picnum,delay);
		//(err==0)? cout << "image retrieved\n" : cout << "Wait_For_Image() Error. No image after waiting for " << delay << " ms\n";
		//cout << "Checking DMA Length...";
		//Check DMA and get address from buffer
		err=grabber.Check_DMA_Length(picnum);\
		//(err==0)? cout << "checked\n" : cout << "Check_DMA_Length() Error\n";
		//cout << "Getting address from framebuffer...";
		err=grabber.Get_Framebuffer_adr(picnum,(void**)&adr);
		//(err==0)? cout << "got\n" : cout << "Get_Framebuffer_adr() Error\n";
		//Stop aquisition
		//cout << "Stopping aquisition...";
		err=grabber.Stop_Aquire();
		//(err==0)? cout << "stopped\n" : cout << "Stop_Aquire() Error\n";
		//Get image size, allocate memory and transfer the image there
		//cout << "Getting image size...";
		grabber.Get_actual_size(&act_width,&act_height,NULL);
		//cout << "got frame of size " << act_width << " x " << act_height << " pixels\n";
		//cout << "Allocating memory for frame...";
		picbuf = (WORD*)malloc(act_width*act_height*sizeof(WORD));
		//cout << "allocated\n";
		//cout << "Extracting the frame...";
		grabber.Extract_Image(picbuf,adr,act_width,act_height);
		//cout << "extracted\n";
		//cout << "Freeing picbuf not to leak memory...";
		free(picbuf);
		//cout << "free\n";
		//cout << endl;
	}
	//Measuring time
	high_resolution_clock::time_point t2 = high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count();
	cout << "Total time for frame grabbing: " << duration << "ms" << ". Time per frame: " << duration/nframes << " ms/frame" << endl;
	cout << endl;
//************************************************************************

	//Now I essentially take another frame with the same settings to explore the image later	
	cout << "Setting grabber to aquire image...";
	err=grabber.Start_Aquire(1);
	(err==0)? cout << "set\n" : cout << "StartAquire() Error\n";
	cout << "Grabber waiting for image...";
	err=grabber.Wait_For_Image(&picnum,delay);
	(err==0)? cout << "image retrieved\n" : cout << "Wait_For_Image() Error. No image after waiting for " << delay << " ms\n";
	cout << "Checking DMA Length...";
	//Check DMA and get address from buffer
	err=grabber.Check_DMA_Length(picnum);\
	(err==0)? cout << "checked\n" : cout << "Check_DMA_Length() Error\n";
	cout << "Getting address from framebuffer...";
	err=grabber.Get_Framebuffer_adr(picnum,(void**)&adr);
	(err==0)? cout << "got\n" : cout << "Get_Framebuffer_adr() Error\n";
	//Stop aquisition
	cout << "Stopping aquisition...";
	err=grabber.Stop_Aquire();
	(err==0)? cout << "stopped\n" : cout << "Stop_Aquire() Error\n";
	//Get image size, allocate memory and transfer the image there
	cout << "Getting image size...";
	grabber.Get_actual_size(&act_width,&act_height,NULL);
	cout << "got frame of size " << act_width << " x " << act_height << " pixels\n";
	cout << "Allocating memory for frame...";
	picbuf = (WORD*)malloc(act_width*act_height*sizeof(WORD));
	cout << "allocated\n";
	cout << "Extracting the frame...";
	grabber.Extract_Image(picbuf,adr,act_width,act_height);
	cout << "extracted\n";
	picnum = img_nr_from_tmstamp(picbuf,0);
	cout << "Timestamp from frame: " << picnum << endl;
/*
	//Now I have the image in picbuf, let's do a quick trial before saving
	cout << "Some pixel values: " << *picbuf << ", " << *picbuf++ << endl;
	//char *txt;
  	int min,max,v;
	unsigned int l;
	WORD *adrs;
  	max=0;
  	min=0xFFFF;
  	adrs=(WORD*)picbuf;
  	l=act_width*20; //skip first lines with timestamp
  	for(;l<act_width*act_height;l++){
   	v=*(adrs+l);
   	if(v<min)
    		min=v;
   	if(v>max)
    		max=v;
  	}
  	printf("grab_single pixels min_value: %d max_value %d\n",min,max);
*/
	//Store image as fits
	cout << "Storing image...";
	char *filename = "/home/salo/0.Jose/2.mintguider/0.Versions/2015.07.15/mint_camera_only/imgs/test.fits";
	err=store_fits(filename,act_width,act_height,0,picbuf);
	(err==0)? cout << "stored as " << filename << endl : cout << "store_fits() Error\n";
	//Free picbuf now that I have used its contents to save the image
	free(picbuf);
	//Free frame buffer and close grabber
	cout << "Freeing frame buffer...";
	err=grabber.Free_Framebuffer();
	(err==0)? cout << "freed\n" : cout << "Free_Framebuffer() Error\n";
	cout << "Closing grabber...";
	err=grabber.Close_Grabber();
	(err==0)? cout << "closed\n" : cout << "Closing grabber() Error\n";
	//Set camera status to not recording and close
	cout << "Setting camera state to not recording...";
	err=camera.PCO_SetRecordingState(0);
	(err==0)? cout << "set\n" : cout << "PCO_SetRecordingState() Error";
	cout << "Closing camera...";
	err=camera.Close_Cam();
	(err==0)? cout << "closed\n" : cout << "Close_Cam() Error\n";
	
	cout << "Leaving zperformance program...\n";
	cout << "--------------------------------\n";

	return 0;
}

int img_nr_from_tmstamp (void *buf,int shift){
	unsigned short *b;
	int y;
	int image_nr=0;
	b=(unsigned short *)(buf);
	y=100*100*100;
	for(;y>0;y/=100)
	{
		*b>>=shift;
		image_nr+= (((*b&0x00F0)>>4)*10 + (*b&0x000F))*y;
		b++;
	}
	return image_nr;
}
