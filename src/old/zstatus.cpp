/* Short program to check MGS status, in particular, camera
*/

// The includes needed to use the functions
#include "zstatus.hpp"
// To avoid repeating std::cout...
using namespace std;

int main(int argc, const char * argv[]){
	cout << "################################################################################\n";
	cout << "Checking MGS status...\n";
	cout << "################################################################################\n";
//Open camera
	//Create camera object
	CPco_cl_com camera;
	//Open the camera
	DWORD err=0;
	cout << "Opening camera... ";
	err=camera.Open_Cam(0);
	(err==0)? cout << "Camera opened!\n" : cout << "Could not open camera\n";
//Get camera info
	//Get the camera type and serial number
  	WORD camtype;
  	DWORD serialnumber;
	err=camera.PCO_GetCameraType(&camtype,&serialnumber);
	(err==0)? cout << "Camera type: " << camtype << ", serial number: " << serialnumber << endl : cout << "Problem getting camera type\n";
	//Get camera description
	SC2_Camera_Description_Response description;
	err=camera.PCO_GetCameraDescriptor(&description);
	//Get camera name
	char infostr[100];
	err=camera.PCO_GetInfo(1,infostr,sizeof(infostr));
	(err==0)? cout << "Camera name: " << infostr << endl : cout << "Problem getting the camera name\n";
	if(err!=0)
		printf("PCO_GetCameraDescriptor() Error 0x%x\n",err);
	else{
		cout << "--------------------------------------------------------------------------------\n";
		cout << "Sensor parameter options available for camera:\n";
		cout << "--------------------------------------------------------------------------------\n";
		cout << "Pixel rate options: " << description.dwPixelRateDESC[0] << ", " << description.dwPixelRateDESC[1] << ", " << description.dwPixelRateDESC[2] << ", " << description.dwPixelRateDESC[3] << endl;
		cout << "Maximum horizontal binning is " << description.wMaxBinHorzDESC << ", in steps of " << description.wBinHorzSteppingDESC << endl;
		cout << "Maximum vertical binning is " << description.wMaxBinVertDESC << ", in steps of " << description.wBinVertSteppingDESC << endl;
		cout << "Minimum exposure time is " << description.dwMinExposureDESC << " ns, Maximum exposure time is " << description.dwMaxExposureDESC << " ms" << endl;
	}
	//Get camera transfer parameters
	PCO_SC2_CL_TRANSFER_PARAM clpar;
	err=camera.PCO_GetTransferParameter(&clpar,sizeof(clpar));
	if(err!=0)
		printf("PCO_GetTransferParameter() Error 0x%x\n",err);
  	else{
		cout << "--------------------------------------------------------------------------------\n";
		printf("Camera transfer parameters:\n");
		cout << "--------------------------------------------------------------------------------\n";
   		printf("Baudrate      : %d\n",clpar.baudrate);
   		printf("Clockfrequency: %d\n",clpar.ClockFrequency);
   		printf("Dataformat    : 0x%x\n",clpar.DataFormat);
   		printf("Transmit      : 0x%x\n",clpar.Transmit);
  	}
//Readind sensor parameters that cannot be easily changed in Linux
	cout << "--------------------------------------------------------------------------------\n";
	cout << "Sensor parameters that need to be changed in CamWare (Windows)\n";
	cout << "--------------------------------------------------------------------------------\n";
	//Get binning
	cout << "Getting binning...";
	WORD binningx, binningy;
	err=camera.PCO_GetBinning(&binningx,&binningy);
	(err==0)? cout << "... binning is " << binningx << " x " << binningy << endl : cout << "PCO_GetBinning() Error\n";
	//Get image size
	DWORD width;
	DWORD height;
	err=camera.PCO_GetActualSize(&width,&height);
	(err==0)? cout << "Sensor width = " << width << " pixels, sensor height = " << height << " pixels\n" : cout << "PCO_GetActualSize() Error\n";
	//Get pixelrate
	cout << "Getting pixel rate...";
	DWORD pixelrate;
	err=camera.PCO_GetPixelRate(&pixelrate);
	(err==0)? cout << "... pixel rate = " << pixelrate << " pixels/s\n" : cout << "PCO_GetPixelRate() Error\n";


//Setting parameters
	cout << "--------------------------------------------------------------------------------\n";
	cout << "Preparing the camera for exposure\n";
	cout << "--------------------------------------------------------------------------------\n";
	//Set camera to current time
	cout << "Setting camera to current time...";
	err=camera.PCO_SetCameraToCurrentTime();
	(err==0)? cout << "camera set to current time!\n" : cout << "PCO_SetCameraToCurrentTime() Error\n";
	//Set camera recording status to not recording
	cout << "Setting camera status to no recording...";
	err=camera.PCO_SetRecordingState(0);
	(err==0)? cout << "set!\n" : cout << "PCO_SetRecordingState() Error\n";
	//Set timestamp mode
	cout << "Setting timestamp mode...";
	err=camera.PCO_SetTimestampMode(2);
	(err==0)? cout << "set!\n" : cout << "PCO_SetTimestampMode() Error\n";
	//Set timebase WHY???????????
	cout << "Setting timebase...";
	err=camera.PCO_SetTimebase(1,1);
	(err==0)? cout << "set!\n" : cout << "PCO_SetTimebase() Error\n";
	//Set delay & Exposure
	cout << "Setting delay and exposure times...\n";
	DWORD delay, exposure;
	delay = STATUS_DELAY;
	exposure = STATUS_EXPOSURE;
	err=camera.PCO_SetDelayExposure(delay,exposure);
	err=camera.PCO_GetDelayExposure(&delay,&exposure);
	(err==0)? cout << "Delay = " << delay << " ms, exposure = " << exposure << " ms\n" : cout << "PCO_GetDelayExposure() Error\n";
	//Set bit alignment
	cout << "Setting bit alignment...";
	err=camera.PCO_SetBitAlignment(BIT_ALIGNMENT_LSB);
	(err==0)? cout << "set!\n" : cout << "PCO_SetBitAlignment() Error\n";
	//Set transmission parameters
	cout << "Setting additional transmission parameters...\n";
	if(clpar.Transmit!=1)
  	{
   		printf("Set TransferParameter Transmit to 1 = continuous\n");
   		clpar.Transmit=1;
  	}
	if(clpar.ClockFrequency!=80000000)
	{
   		printf("Set TransferParameter Cameralink ClockFrequency to 80MHz\n");
   		clpar.ClockFrequency=80000000;
  	}
	err=camera.PCO_SetTransferParameter(&clpar,sizeof(clpar));
	if(err!=PCO_NOERROR)
   		printf("PCO_SetTransferParameter() Error 0x%x\n",err);
  	err=camera.PCO_GetTransferParameter(&clpar,sizeof(clpar));
  	if(err!=PCO_NOERROR)
   		printf("PCO_GetTransferParameter() Error 0x%x\n",err);
  	else
  	{
   		printf("Transmit      : 0x%x\n",clpar.Transmit);
   		printf("Clockfrequency: %d\n",clpar.ClockFrequency);
  	}
	cout << "Transmission parameters set\n";
	//Arm camera
	cout << "Arming camera...";
	err=camera.PCO_ArmCamera();
	(err==0)? cout << " camera armed!\n" : cout << "PCO_ArmCamera() Error\n";
	cout << "--------------------------------------------------------------------------------\n";
	cout << "Preparing the grabber for exposure\n";
	cout << "--------------------------------------------------------------------------------\n";
	//Create grabber object
	CPco_me4_edge grabber;
	//Open the grabber
	cout << "Opening grabber... ";
	err=grabber.Open_Grabber(0);
	(err==0)? cout << "Grabber opened!\n" : cout << "Could not open grabber\n";
	//Set data format
	cout << "Setting data format...";
	err=grabber.Set_DataFormat(clpar.DataFormat);
	(err==0)? cout << "set!\n" : cout << "Set_DataFormat() Error\n";
	//Get actual image size
	cout << "Verifying actual image size size...";
	err=camera.PCO_GetActualSize(&width, &height);
	(err==0)? cout << "... width = " << width << " pixels, height = " << height << " pixels\n" : cout << "PCO_GetActualSize() Error\n";
	//Get Delay exposure
	cout << "Verifying actual delay & exposure times...";
	DWORD act_delay, act_exp;
	err=camera.PCO_GetDelayExposure(&act_delay,&act_exp);
	(err==0)? cout << "... delay = " << act_delay << " ms, exp = " << act_exp << " ms\n" : cout << "PCO_GetDelayExposure() Error\n";
	//Set the grabber size
	cout << "Setting grabber size...";
	err=grabber.Set_Grabber_Size(width,height);
	(err==0)? cout << "set!\n" : cout << "Set_Grabber_Size() Error\n";
	//Allocate frame buffer
	cout << "Allocating frame buffer...";
	int bufnum = STATUS_BUFFER;
	err=grabber.Allocate_Framebuffer(bufnum);
	(err==0)? cout << "allocated buffer for " << bufnum << " frames!\n" : cout << "Allocate_Framebuffer() Error\n";
	//Set recording state
	cout << "Setting recording state...";
	err=camera.PCO_SetRecordingState(1);
	(err==0)? cout << "set!\n" : cout << "PCO_SetRecordingState() Error\n";
	//Get Recording state again
	cout << "Getting recording state...";
	WORD act_recstate;
	err=camera.PCO_GetRecordingState(&act_recstate);
	(err==0)? cout << "it is " << act_recstate << endl : cout << "PCO_GetRecordingState() Error\n";
	//Get COCRuntime
	cout << "Getting COCRuntime...";
	DWORD secs, nsecs;
	err=camera.PCO_GetCOCRuntime(&secs,&nsecs);
	(err==0)? cout << "... secs = " << secs << ", nsecs = " << nsecs << endl : cout << "PCO_GetCOCRuntime() Error\n";

//ACTUAL IMAGE GRABBING
	cout << "--------------------------------------------------------------------------------\n";
	cout << "Acquiring frame as test\n";
	cout << "--------------------------------------------------------------------------------\n";
	int picnum = 1;
	err=grabber.Start_Aquire(picnum);
	cout << "Start Acquiring... ";
	(err==0)? cout << "... acquisition started!\n" : cout << "Start_Aquire() Error\n";
	cout << "Waiting for image... ";
	err=grabber.Wait_For_Image(&picnum,10);
	(err==0)? cout << "... image arrived!\n" : cout << "Wait_For_Image() Error\n";
	cout << "Checking DMA length... ";
	err=grabber.Check_DMA_Length(picnum);
	(err==0)? cout << "... DMA length checked!\n" : cout << "Check_DMA_Length() Error\n";
	WORD *adr;
	cout << "Getting buffer address... ";
	err=grabber.Get_Framebuffer_adr(picnum,(void**)&adr);
	(err==0)? cout << "... buffer address is " << adr << endl : cout << "Get_Framebuffer_adr() Error\n";
	cout << "Stop acquiring... ";
	err=grabber.Stop_Aquire();
	(err==0)? cout << "... acquisition stopped!\n" : cout << "Stop_Aquire() Error\n";
	unsigned int w,h;
	cout << "Getting actual image size... ";
	err=grabber.Get_actual_size(&w,&h,NULL);
	(err==0)? cout << "... width = " << w << ", height = " << h << endl : cout << "Get_actual_size() Error\n";
	WORD *picbuf;
	cout << "Allocating memory for image... ";
	picbuf=(WORD*)malloc(w*h*sizeof(WORD));
	(picbuf==NULL)? cout << "Problem allocating picbuf\n" : cout << "... memory allocated for image\n";
	//cout << *picbuf << endl;
	cout << "Extracting image to buffer... ";
	grabber.Extract_Image(picbuf,adr,w,h);
	(err==0)? cout << "... image extracted!\n" : cout << "Extract_Image() Error\n";
	//cout << *	picbuf << endl;
	//picnum=img_nr_from_tmstamp(picbuf,0);  
	//cout << picnum << endl;
/*
	//Save image
	unsigned int c_ok = NOTHING;
	char name[86] = "/home/salo/0.Jose/2.mintguider/0.Versions/2015.07.15/mint_camera_only/imgs/image.fits";
	cout << "Saving image at " << name << endl;
	cout << c_ok << endl;
	//cout << typeid(c_ok).name() << ", " << typeid(picbuf).name() << endl;
	//c_ok = save_ptr2fits(name,2560,2160,picbuf);
	cout << "Image saved at " << name << endl;
	cout << c_ok << endl;
*/
	//Do not forget to free the allocated memory to avoid leaks!!
	free(picbuf);

//INFO ON CAMERA TEMPERATURE
	cout << "--------------------------------------------------------------------------------\n";
	cout << "Information on camera temperature\n";
	cout << "--------------------------------------------------------------------------------\n";
	//Get camera temperature
	SHORT ccdtemp,camtemp,pstemp;
	err=camera.PCO_GetTemperature(&ccdtemp,&camtemp,&pstemp);
  	if(err!=PCO_NOERROR)
   		printf("PCO_GetTemperature() Error 0x%x\n",err);
  	else{
   		printf("Current temperatures (C):\n");
   		if(ccdtemp == (SHORT)(-32768))
    			printf("no sensor temperature\n");
   		else
   			cout << "CCD temperature = " << double(ccdtemp)/10.0 << ", camera temperature = " << camtemp << ", power supply temperature = " << pstemp << endl;
  	}
//CLOSING SYSTEM
	cout << "--------------------------------------------------------------------------------\n";
	cout << "Closing system\n";
	cout << "--------------------------------------------------------------------------------\n";
	//Close grabber
	cout << "Closing grabber... ";
	err=grabber.Close_Grabber();
	(err==0)? cout << "... grabber closed!\n" : cout << "Could not close grabber\n";
	//Close camera
	cout << "Closing camera... ";
	err=camera.Close_Cam();
	(err==0)? cout << "... camera closed!\n" : cout << "Could not close camera\n";

	cout << "################################################################################\n";
	cout << "Exiting MGS status check\n";
	cout << "################################################################################\n";

	return 0;
}


