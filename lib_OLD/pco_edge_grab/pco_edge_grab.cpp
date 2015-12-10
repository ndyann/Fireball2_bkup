//-----------------------------------------------------------------//
// Name        | pco_edge_grab.cpp           | Type: (*) source    //
//-------------------------------------------|       ( ) header    //
// Project     | pco.camera                  |       ( ) others    //
//-----------------------------------------------------------------//
// Platform    | Linux                                             //
//-----------------------------------------------------------------//
// Environment |                                                   //
//             |                                                   //
//-----------------------------------------------------------------//
// Purpose     | pco.camera - CameraLink Communication             //
//-----------------------------------------------------------------//
// Author      | MBL, PCO AG                                       //
//-----------------------------------------------------------------//
// Revision    | rev. 1.03                                         //
//-----------------------------------------------------------------//
// Notes       | main console program to show how to use           //
//             | class Cpco_cl_me4 and Cpco_cl_com to grab         //
//             | images from camera pco.edge Rolling Shutter       //
//             | with the Me4 grabber and save them to tif or b16  //
//             | files                                             //
//             |                                                   //
//-----------------------------------------------------------------//
// (c) 2010 - 2012 PCO AG                                          //
// Donaupark 11 D-93309  Kelheim / Germany                         //
// Phone: +49 (0)9441 / 2005-0   Fax: +49 (0)9441 / 2005-20        //
// Email: info@pco.de                                              //
//-----------------------------------------------------------------//

//-----------------------------------------------------------------//
// This program is free software; you can redistribute it and/or   //
// modify it under the terms of the GNU General Public License as  //
// published by the Free Software Foundation; either version 2 of  //
// the License, or (at your option) any later version.             //
//                                                                 //
// This program is distributed in the hope that it will be useful, //
// but WITHOUT ANY WARRANTY; without even the implied warranty of  //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the    //
// GNU General Public License for more details.                    //
//                                                                 //
// You should have received a copy of the GNU General Public       //
// License along with this program; if not, write to the           //
// Free Software Foundation, Inc., 59 Temple Place - Suite 330,    //
// Boston, MA 02111-1307, USA                                      //
//-----------------------------------------------------------------//

//-----------------------------------------------------------------//
// Revision History:                                               //
//-----------------------------------------------------------------//
// Rev.:     | Date:      | Changed:                               //
// --------- | ---------- | ---------------------------------------//
//  1.01     | 08.10.2011 | ported from Windows SDK                //
//           |            |                                        //
//-----------------------------------------------------------------//
//  1.02     | 14.01.2011 | more commands                          //
//           |            |                                        //
//-----------------------------------------------------------------//
//  1.03     | 24.01.2012 | use pcofile library                    //
//           |            | more commands                          //
//           |            |                                        //
//-----------------------------------------------------------------//
//  0.0x     | xx.xx.2012 |                                        //
//           |            |                                        //
//-----------------------------------------------------------------//


#include <iostream>

#include "VersionNo.h"

#include "pco_cl_include.h"
#include "Cpco_cl_com.h"
#include "Cpco_me4.h"
#include "file12.h"

int image_nr_from_timestamp(void *buf,int shift);
DWORD grab_single(CPco_me4 *grabber);
DWORD grab_single(CPco_me4 *grabber,char * filename);
DWORD grab_count(CPco_me4 *grabber,int ima_count);
DWORD grab_loop(CPco_me4 *grabber,int ima_count);

void get_number(char *number,int len);
void get_text(char *text,int len);
void get_hexnumber(int *num,int len);


CPco_Log log("pco_edge_grab.log");

int main(int argc, char* argv[])
{
  DWORD err;
  CPco_cl_com camera;
  CPco_me4_edge grabber;

  int board=0;
  char infostr[100];
  char number[20];

  int x;
  char c;
  int ima_count=100;
  int loop_count=1;
  int PicTimeOut=10000; //10 seconds
  WORD act_recstate;
  DWORD exp_time,delay_time,pixelrate;
  DWORD width,height,secs,nsecs;
  SC2_Camera_Description_Response description;
  PCO_SC2_CL_TRANSFER_PARAM clpar;
  int bufnum=20;
  double freq;
  SHORT ccdtemp,camtemp,pstemp;
  WORD camtype;
  DWORD serialnumber;
  WORD actlut,lutparam;

// log.set_logbits(0x0001FFFF);
  printf("Logging set to 0x%x\n",log.get_logbits());
  camera.log=&log;
  grabber.log=&log;

  printf("Try to open Camera\n");
  err=camera.Open_Cam(0);
  if(err!=PCO_NOERROR)
  {
   printf("error 0x%x in Open_Cam, close application",err);
   getchar();
   return -1;
  }

  err=camera.PCO_GetCameraType(&camtype,&serialnumber);

  if((err!=PCO_NOERROR)||(camtype!=CAMERATYPE_PCO_EDGE))
  {
   printf("Wrong camera for this application");
   camera.Close_Cam();
   getchar();
   return -1;
  }



  printf("Try to open Grabber\n");
  err=grabber.Open_Grabber(board);
  if(err!=PCO_NOERROR)
  {
   printf("error 0x%x in Open_Grabber, close application",err);
   camera.Close_Cam();
   getchar();
   return -1;
  }

  exp_time=5000;
  delay_time=1000;

  err=camera.PCO_GetCameraDescriptor(&description);
  if(err!=PCO_NOERROR)
   printf("PCO_GetCameraDescriptor() Error 0x%x\n",err);


  err=camera.PCO_GetInfo(1,infostr,sizeof(infostr));
  if(err!=PCO_NOERROR)
   printf("PCO_GetInfo() Error 0x%x\n",err);
  else
   printf("Camera Name is: %s serialnumber %d\n",infostr,serialnumber);

  err=camera.PCO_SetCameraToCurrentTime();
  if(err!=PCO_NOERROR)
   printf("PCO_SetCameraToCurrentTime() Error 0x%x\n",err);


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

  err=camera.PCO_GetTemperature(&ccdtemp,&camtemp,&pstemp);
  if(err!=PCO_NOERROR)
   printf("PCO_GetTemperature() Error 0x%x\n",err);
  else
  {
   printf("current temperatures\n");
   printf("Sensor:      %d\n",ccdtemp);
   printf("Camera:      %d\n",camtemp);
   printf("PowerSupply: %d\n",pstemp);
  }


  err=camera.PCO_GetPixelRate(&pixelrate);
  if(err!=PCO_NOERROR)
   printf("PCO_GetPixelrate() Error 0x%x\n",err);
  else
   printf("actual PixelRate: %d\n",pixelrate);
  printf("possible PixelRates:\n");
  for(x=0;x<4;x++)
  {
   if(description.dwPixelRateDESC[x]!=0)
   {
    printf("%d: %d\n",x,description.dwPixelRateDESC[x]);
   }
  }

  err=camera.PCO_GetLut(&actlut,&lutparam);
  if(err!=PCO_NOERROR)
   printf("PCO_GetLut() Error 0x%x\n",err);


//set RecordingState to STOP
  err=camera.PCO_SetRecordingState(0);
  if(err!=PCO_NOERROR)
   printf("PCO_SetRecordingState() Error 0x%x\n",err);

  err=camera.PCO_SetTimestampMode(2);
  if(err!=PCO_NOERROR)
   printf("PCO_SetTimestampMode() Error 0x%x\n",err);

//set camera timebase to ms
  err=camera.PCO_SetTimebase(1,1);
  if(err!=PCO_NOERROR)
   printf("PCO_SetTimebase() Error 0x%x\n",err);

  err=camera.PCO_SetDelayExposure(delay_time,exp_time);
  if(err!=PCO_NOERROR)
   printf("PCO_SetDelayExposure() Error 0x%x\n",err);


//prepare Camera for recording
  err=camera.PCO_ArmCamera();
  if(err!=PCO_NOERROR)
   printf("PCO_ArmCamera() Error 0x%x\n",err);

  err=camera.PCO_GetActualSize(&width,&height);
  if(err!=PCO_NOERROR)
   printf("PCO_GetActualSize() Error 0x%x\n",err);

  printf("Actual Resolution %d x %d\n",width,height);

//transfer dataformat must be changed depending on pixelrate and horizontal resolution
  err=camera.PCO_GetPixelRate(&pixelrate);
  if(err!=PCO_NOERROR)
   printf("PCO_GetPixelrate() Error 0x%x\n",err);

  if((width>1920)&&(pixelrate>=286000000))
  {
   clpar.DataFormat=SCCMOS_FORMAT_TOP_CENTER_BOTTOM_CENTER|PCO_CL_DATAFORMAT_5x12;
   printf("width>1920 %d && pixelrate >=286000000 %d Dataformat 0x%x\n",width,pixelrate,clpar.DataFormat);
  }
  else
  {
   clpar.DataFormat=SCCMOS_FORMAT_TOP_CENTER_BOTTOM_CENTER|PCO_CL_DATAFORMAT_5x16;
   printf("width<=1920 %d || pixelrate<286000000 %d Dataformat 0x%x\n",width,pixelrate,clpar.DataFormat);
  }

  err=camera.PCO_SetTransferParameter(&clpar,sizeof(clpar));
  if(err!=PCO_NOERROR)
   printf("PCO_TransferParameter() Error 0x%x\n",err);

  err=camera.PCO_ArmCamera();
  if(err!=PCO_NOERROR)
   printf("PCO_ArmCamera() Error 0x%x\n",err);

  err=grabber.Set_DataFormat(clpar.DataFormat);
  if(err!=PCO_NOERROR)
   printf("Set_DataFormat() Error 0x%x\n",err);

  err=grabber.Set_Grabber_Size(width,height);
  if(err!=PCO_NOERROR)
   printf("Set_Grabber_Size() Error 0x%x\n",err);


  err=grabber.Allocate_Framebuffer(20);
  if(err!=PCO_NOERROR)
   printf("Allocate_Framebuffer() Error 0x%x\n",err);

  err=camera.PCO_SetRecordingState(1);
  if(err!=PCO_NOERROR)
   printf("PCO_SetRecordingState() Error 0x%x\n",err);


  c=' ';
  while(c!='x')
  {
   int ch;
   c=' ';

   printf("\n");
   camera.PCO_GetRecordingState(&act_recstate);
   camera.PCO_GetDelayExposure(&delay_time,&exp_time);
   camera.PCO_GetCOCRuntime(&secs,&nsecs);
   freq=nsecs;
   freq/=1000000000;
   freq+=secs;
   freq=1/freq;
   printf("actual recording state %s actual freq: %.3lfHz %.2lfMB/sec\n",act_recstate ? "RUN" : "STOP",freq,(freq*width*height*2)/(1024*1024));
   printf("\n");
   printf("x to close camera and program\n");
   printf("l to set loop_count actual value =%d\n",loop_count);
   printf("c to set imagecount actual imagecount=%d\n",ima_count);
   printf("b to set number of allocated buffers actual =%d\n",bufnum);
   printf("t to set picture timeout actual timeout=%d\n",PicTimeOut);
   printf("e to set exposure time actual exposuretime=%dus\n",exp_time);
   printf("d to set delay time actual delaytime=%dus\n",delay_time);
   printf("f to set Dataformat 0x%x and LUT 0x%x\n",clpar.DataFormat,actlut);
   printf("p to set pixelrate actual pixelrate %d\n",pixelrate);
   printf("0 to set recording state to OFF\n");
   printf("1 to set recording state to ON\n");
   printf("2 Single Grab \n");
   printf("3 Start Grab in allocated buffers\n");
   printf("4 Start Grab loop (%d images)\n",ima_count);
   fflush(stdin);

   for( x = 0; (x < 2) &&  ((ch = getchar()) != EOF)
                        && (ch != '\n'); x++ )
    c=(char)ch;

   if(c=='c')
   {
    printf("enter ima_count ...<CR>: ");
    get_number(number,10);
    if(strlen(number))
     ima_count=atoi(number);
   }
   else if(c=='l')
   {
    printf("enter loop_count ...<CR>: ");
    get_number(number,10);
    if(strlen(number))
     loop_count=atoi(number);
   }
   else if(c=='b')
   {
    printf("enter number of buffers to allocate ...<CR>: ");
    get_number(number,5);
    if(strlen(number))
    {
     int i=atoi(number);
     grabber.Free_Framebuffer();
     if((err=grabber.Allocate_Framebuffer(i))!=PCO_NOERROR)
     {
      printf("Allocation failed use old number %d\n",bufnum);
      err=grabber.Allocate_Framebuffer(bufnum);
      printf("Any key <CR> to proceed");
      getchar();
     }
     else
      bufnum=i;
    }
   }
   else if(c=='t')
   {
    printf("enter picture timeout ...<CR>: ");
    get_number(number,10);
    if(strlen(number))
     PicTimeOut=atoi(number);
   }
   else if(c=='e')
   {
    printf("enter new exposure time in us ...<CR>: ");
    get_number(number,10);
    if(strlen(number))
    {
     exp_time=atoi(number);
     if(act_recstate==1)
      camera.PCO_SetRecordingState(0);
     err=camera.PCO_SetDelayExposure(delay_time,exp_time);
     err=camera.PCO_GetDelayExposure(&delay_time,&exp_time);
     err=camera.PCO_ArmCamera();
     if(err!=PCO_NOERROR)
      printf("PCO_ArmCamera() Error 0x%x\n",err);
     if(act_recstate==1)
      camera.PCO_SetRecordingState(1);
    }
   }
   else if(c=='d')
   {
    printf("enter new delay time in us ...<CR>: ");
    get_number(number,10);
    if(strlen(number))
    {
     delay_time=atoi(number);
     if(act_recstate==1)
      camera.PCO_SetRecordingState(0);
     err=camera.PCO_SetDelayExposure(delay_time,exp_time);
     err=camera.PCO_GetDelayExposure(&delay_time,&exp_time);
     err=camera.PCO_ArmCamera();
     if(err!=PCO_NOERROR)
      printf("PCO_ArmCamera() Error 0x%x\n",err);
     if(act_recstate==1)
      camera.PCO_SetRecordingState(1);
    }
   }
   else if(c=='p')
   {
    printf("enter new pixelrate in MHz ...<CR>: ");
    get_number(number,4);
    if(strlen(number))
    {
     unsigned int dataformat=clpar.DataFormat;
     DWORD p;
     int x;

     p=atoi(number)*1000*1000;
     for(x=0;x<4;x++)
     {
      if((p<description.dwPixelRateDESC[x]+1000*1000)&&
         (p>description.dwPixelRateDESC[x]-1000*1000))
      {
       p=description.dwPixelRateDESC[x];
       break;
      }
     }
     if(x>=4)
     {
      printf("pixelrate %d not supported",p);
      continue;
     }

     if(act_recstate==1)
      camera.PCO_SetRecordingState(0);

     err=camera.PCO_SetPixelRate(p);
     if(err!=PCO_NOERROR)
      printf("PCO_SetPixelRate() Error 0x%x\n",err);

     err=camera.PCO_ArmCamera();
     if(err!=PCO_NOERROR)
      printf("PCO_ArmCamera() Error 0x%x\n",err);

     camera.PCO_GetPixelRate(&pixelrate);

//this is a must do
     if((width>1920)&&(pixelrate>=286000000))
      clpar.DataFormat=(clpar.DataFormat&~PCO_CL_DATAFORMAT_MASK)|PCO_CL_DATAFORMAT_5x12;
//this is can do
//     else
//      clpar.DataFormat=(clpar.DataFormat&~PCO_CL_DATAFORMAT_MASK)|PCO_CL_DATAFORMAT_5x16;

     if(dataformat!=clpar.DataFormat)
     {
      err=camera.PCO_SetTransferParameter(&clpar,sizeof(clpar));
      if(err!=PCO_NOERROR)
       printf("PCO_TransferParameter() Error 0x%x\n",err);

      err=camera.PCO_ArmCamera();
      if(err!=PCO_NOERROR)
       printf("PCO_ArmCamera() Error 0x%x\n",err);

      err=grabber.Set_DataFormat(clpar.DataFormat);
      if(err!=PCO_NOERROR)
       printf("Set_DataFormat() Error 0x%x\n",err);

      err=grabber.Set_Grabber_Size(width,height);
      if(err!=PCO_NOERROR)
       printf("Set_Grabber_Size() Error 0x%x\n",err);
     }

     if(act_recstate==1)
      camera.PCO_SetRecordingState(1);
    }
   }
   else if(c=='f')
   {
    int dataformat,lut;

    dataformat=lut=-1;
    printf("enter new Dataformat in HEX ...<CR>: ");
    get_hexnumber(&dataformat,4);
    if((dataformat&SCCMOS_FORMAT_MASK)==0)
     dataformat|=SCCMOS_FORMAT_TOP_CENTER_BOTTOM_CENTER;
    if(   ((dataformat& PCO_CL_DATAFORMAT_MASK)!=PCO_CL_DATAFORMAT_5x16)
        &&((dataformat& PCO_CL_DATAFORMAT_MASK)!=PCO_CL_DATAFORMAT_5x12)
        &&((dataformat& PCO_CL_DATAFORMAT_MASK)!=PCO_CL_DATAFORMAT_5x12L))
    {
     printf("DataFormat 0x%x not supported" ,dataformat);
     continue;
    }

    printf("installed LUT's:\n");
    for(int x=0;x<camera.num_lut;x++)
     printf("%d: '%20s' Id 0x%x %02d->%02d Format 0x%x\n",x+1
       ,camera.cam_lut[x].Description,camera.cam_lut[x].wIdentifier
       ,camera.cam_lut[x].bInputWidth,camera.cam_lut[x].bOutputWidth
       ,camera.cam_lut[x].wFormat);
    printf("enter new LUT ID in HEX ...<CR>: ");
    get_hexnumber(&lut,4);

    if((dataformat>0)&&(lut>=0))
    {
     if(act_recstate==1)
      camera.PCO_SetRecordingState(0);

     if((width>1920)&&(pixelrate>=286000000))
      clpar.DataFormat=(dataformat&~PCO_CL_DATAFORMAT_MASK)|PCO_CL_DATAFORMAT_5x12;
     else
      clpar.DataFormat=dataformat;

     err=camera.PCO_SetTransferParameter(&clpar,sizeof(clpar));
     if(err!=PCO_NOERROR)
      printf("PCO_TransferParameter() Error 0x%x\n",err);

     actlut=lut;
     err=camera.PCO_SetLut(actlut,0);
     if(err!=PCO_NOERROR)
      printf("PCO_TransferParameter() Error 0x%x\n",err);


     err=camera.PCO_ArmCamera();
     if(err!=PCO_NOERROR)
      printf("PCO_ArmCamera() Error 0x%x\n",err);

     err=grabber.Set_DataFormat(clpar.DataFormat);
     if(err!=PCO_NOERROR)
      printf("Set_DataFormat() Error 0x%x\n",err);

     err=grabber.Set_Grabber_Size(width,height);
     if(err!=PCO_NOERROR)
      printf("Set_Grabber_Size() Error 0x%x\n",err);

     if(act_recstate==1)
      camera.PCO_SetRecordingState(1);
    }
   }
   else if(c=='0')
   {
    err=camera.PCO_SetRecordingState(0);
    if(err==PCO_NOERROR)
     printf("\nrecoding state is set to STOP\n");
   }
   else if(c=='1')
   {
    err=camera.PCO_SetRecordingState(1);
    if(err==PCO_NOERROR)
     printf("\nrecoding state is set to RUN\n");
   }
   else if(c=='2')
   {
    if(act_recstate==0)
     printf("\nStart Camera before grabbing\n");
    else
    {
     char filename[300];
     printf("enter filename ...<CR>: \n");
     printf("if filename has suffix .b16 save image in b16 format\n");
     printf("if filename has suffix .tif save image in tif format\n");
     printf("if nothing is entered, no file will be saved\n");
     get_text(filename,300);
     if(strlen(filename))
      grab_single(&grabber,filename);
     else
      grab_single(&grabber);
    }
    printf("\n");
   }
   else if(c=='3')
   {
    if(act_recstate==0)
     printf("\nStart Camera before grabbing\n");
    else
     grab_count(&grabber,bufnum);
    printf("\n");
   }
   else if(c=='4')
   {
    if(act_recstate==0)
     printf("\nStart Camera before grabbing\n");
    else
     grab_loop(&grabber,ima_count);
    printf("\n");
   }
  }

  err=grabber.Free_Framebuffer();

  grabber.Close_Grabber();
  camera.Close_Cam();

  return 0;
}


DWORD grab_single(CPco_me4 *grabber)
{
 int err;
 int picnum;
 unsigned int w,h;
 WORD *adr;
 WORD *linebuf;


 picnum=1;
 err=grabber->Start_Aquire(1);
 if(err!=PCO_NOERROR)
  printf("\ngrab_single Start_Aquire error 0x%x\n",err);

 if(err==PCO_NOERROR)
 {
  err=grabber->Wait_For_Image(&picnum,10);
  if(err!=PCO_NOERROR)
   printf("\ngrab_single Wait_For_Image error 0x%x\n",err);
 }

 if(err==PCO_NOERROR)
 {
  err=grabber->Check_DMA_Length(picnum);
  if(err!=PCO_NOERROR)
   printf("\ngrab_single Check_DMA_Length error 0x%x\n",err);
 }

 if(err==PCO_NOERROR)
 {
  err=grabber->Get_Framebuffer_adr(picnum,(void**)&adr);
  if(err!=PCO_NOERROR)
   printf("\ngrab_single Get_Framebuffer_adr error 0x%x\n",err);
 }

 err=grabber->Stop_Aquire();
 if(err!=PCO_NOERROR)
  printf("\ngrab_single Stop_Aquire error 0x%x\n",err);

 if(err==PCO_NOERROR)
 {
  grabber->Get_actual_size(&w,&h,NULL);
  linebuf=(WORD*)malloc(w*sizeof(WORD));
  grabber->Get_Image_Line(linebuf,adr,1,w,h);
  picnum=image_nr_from_timestamp(linebuf,0);
  printf("\ngrab_single done successful, timestamp image_nr: %d\n",picnum);
  free(linebuf);
 }

 return err;
}

DWORD grab_single(CPco_me4 *grabber,char* filename)
{
 int err;
 int picnum;
 unsigned int w,h,l;
 WORD *adr;
 WORD *picbuf;

 picnum=1;
 err=grabber->Start_Aquire(1);
 if(err!=PCO_NOERROR)
  printf("\ngrab_single Start_Aquire error 0x%x\n",err);

 if(err==PCO_NOERROR)
 {
  err=grabber->Wait_For_Image(&picnum,10);
  if(err!=PCO_NOERROR)
   printf("\ngrab_single Wait_For_Image error 0x%x\n",err);
 }

 if(err==PCO_NOERROR)
 {
  err=grabber->Check_DMA_Length(picnum);
  if(err!=PCO_NOERROR)
   printf("\ngrab_single Check_DMA_Length error 0x%x\n",err);
 }

 if(err==PCO_NOERROR)
 {
  err=grabber->Get_Framebuffer_adr(picnum,(void**)&adr);
  if(err!=PCO_NOERROR)
   printf("\ngrab_single Get_Framebuffer_adr error 0x%x\n",err);
 }

 err=grabber->Stop_Aquire();
 if(err!=PCO_NOERROR)
  printf("\ngrab_single Stop_Aquire error 0x%x\n",err);

 if(err==PCO_NOERROR)
 {
  char *txt;
  int min,max,v;

  grabber->Get_actual_size(&w,&h,NULL);
  picbuf=(WORD*)malloc(w*h*sizeof(WORD));
  grabber->Extract_Image(picbuf,adr,w,h);

  picnum=image_nr_from_timestamp(picbuf,0);
  printf("grab_single done successful, timestamp image_nr: %d\n",picnum);
  max=0;
  min=0xFFFF;
  adr=(WORD*)picbuf;
  l=w*20; //skip first lines with timestamp
  for(;l<w*h;l++)
  {
   v=*(adr+l);
   if(v<min)
    min=v;
   if(v>max)
    max=v;
  }
  printf("grab_single pixels min_value: %d max_value %d\n",min,max);


  do
  {
   txt=strchr(filename,'.');
  }
  while(strlen(txt)>4);

  if((txt)&&(strstr(txt,"b16")))
  {
   store_b16(filename,w,h,0,picbuf);
   printf("b16 image saved to %s\n",filename);
  }
  else if((txt)&&(strstr(txt,"tif")))
  {
   store_tif(filename,w,h,0,picbuf);
   printf("tif image saved to %s\n",filename);
  }

  free(picbuf);
 }

 return err;
}


DWORD grab_count(CPco_me4 *grabber,int ima_count)
{
 int err;
 int firstnum,lastnum;
 unsigned int width,height;
 WORD *adr;
 WORD *linebuf;
 double time,freq;

 grabber->Get_actual_size(&width,&height,NULL);
 linebuf=(WORD*)malloc(width*sizeof(WORD));

 firstnum=1;
 lastnum=ima_count;

 err=grabber->Start_Aquire(ima_count);
 if(err!=PCO_NOERROR)
  printf("\ngrab_count Start_Aquire error 0x%x\n",err);

 if(err==PCO_NOERROR)
 {
  err=grabber->Wait_For_Image(&firstnum,10);
  log.start_time_mess();
  if(err!=PCO_NOERROR)
   printf("\ngrab_count Error waiting for first image number %d\n",firstnum);
 }

 if(err==PCO_NOERROR)
 {
  err=grabber->Get_Framebuffer_adr(firstnum,(void**)&adr);
  if(err!=PCO_NOERROR)
   printf("\ngrab_single Get_Framebuffer_adr error 0x%x\n",err);
 }

 if(err==PCO_NOERROR)
 {
  grabber->Get_Image_Line(linebuf,adr,1,width,height);
  firstnum=image_nr_from_timestamp(linebuf,0);
  printf("wait until %d images are grabbed waittime %dms",ima_count,ima_count*1000);
  fflush(stdout);
  err=grabber->Wait_For_Image(&lastnum,1*ima_count);
  if(err!=PCO_NOERROR)
   printf("\ngrab_count Error waiting for last image number %d\n",lastnum);
 }

 time=log.stop_time_mess();

 for(int x=1;x<=ima_count;x++)
 {
  err=grabber->Check_DMA_Length(x);
  if(err!=PCO_NOERROR)
   printf("\ngrab_count Check_DMA_Length error 0x%x at image number %d\n",err,x);
 }


 err=grabber->Stop_Aquire();
 if(err!=PCO_NOERROR)
  printf("\ngrab_count Stop_Aquire error 0x%x\n",err);

 err=grabber->Get_Framebuffer_adr(lastnum,(void**)&adr);
 if(err!=PCO_NOERROR)
  printf("\ngrab_count Get_Framebuffer_adr error 0x%x\n",err);

 if(err==PCO_NOERROR)
 {
  grabber->Get_Image_Line(linebuf,adr,1,width,height);
  lastnum=image_nr_from_timestamp(linebuf,0);

  printf("\ngrab_count done successful");

  freq=(ima_count-1)*1000;
  freq/=time;
  printf("\n%05d images grabbed time %dms freq: %.2fHz %.2fMB/sec",ima_count,(int)time,freq,(freq*width*height*2/(1024*1024)));

  freq=(lastnum-firstnum)*1000;
  freq/=time;
  printf("\nImagenumbers: first %d last %d count %d freq: %.2fHz",firstnum,lastnum,lastnum-firstnum+1,freq);
 }

 free(linebuf);
 return err;
}

DWORD grab_loop(CPco_me4 *grabber,int ima_count)
{
 int picnum,err;
 unsigned int width,height;
 void *adr;
 WORD *linebuf;
 int firstnum,lastnum,currentnum;
 int numoff;
 double time,freq;

 time=freq=1;

 grabber->Get_actual_size(&width,&height,NULL);
 linebuf=(WORD*)malloc(width*sizeof(WORD));

 err=PCO_NOERROR;

 numoff=0;
 picnum=firstnum=1;
 log.start_time_mess();
 grabber->Start_Aquire(ima_count);
 if(err!=PCO_NOERROR)
 {
  printf("\ngrab_loop Start_Aquire error 0x%x",err);
  free(linebuf);
  return err;
 }

 do
 {
  err=grabber->Wait_For_Image(&picnum,10);
  if(err!=PCO_NOERROR)
   printf("\ngrab_loop Error while waiting for image number %d",picnum);

  if(picnum==1)
   log.start_time_mess();

  if(err==PCO_NOERROR)
  {
   err=grabber->Check_DMA_Length(picnum);
   if(err!=PCO_NOERROR)
    printf("\ngrab_loop Check_DMA_Length error 0x%x",err);
  }

  if(err!=PCO_NOERROR)
  {
   printf("\ngrab_loop Error break loop at image number %d",picnum);
   break;
  }

  grabber->Get_Framebuffer_adr(picnum,&adr);
  grabber->Get_Image_Line(linebuf,adr,1,width,height);
  currentnum=image_nr_from_timestamp(linebuf,0);
  if(picnum==1)
   firstnum=currentnum;

  printf("picnum %d currentnum %d %d\r",picnum,currentnum,currentnum-firstnum+1);

  if((currentnum-firstnum-numoff+1)!=picnum)
  {
   numoff++;
   printf("\ngrab_loop Error number: camera %d grabber %d offset now %d\n",currentnum-firstnum+1,picnum,numoff);
  }
  picnum++;
  fflush(stdout);
 }while(picnum<ima_count);

 if(err==PCO_NOERROR)
 {
  picnum=ima_count;
  err=grabber->Wait_For_Image(&picnum,10);
  if(err!=PCO_NOERROR)
   printf("\ngrab_loop Error while waiting for image number %d",picnum);
  time=log.stop_time_mess();

  if(err==PCO_NOERROR)
  {
   err=grabber->Check_DMA_Length(picnum);
   if(err!=PCO_NOERROR)
    printf("\ngrab_loop Check_DMA_Length error 0x%x",err);
  }
 }

 if(grabber->Stop_Aquire()!=PCO_NOERROR)
  printf("\ngrab_loop Stop_Aquire error \n");

 printf("\n");

 if(err==PCO_NOERROR)
 {
  printf("Grab %d images done (picnum %d)\n",ima_count,picnum);
  grabber->Get_Framebuffer_adr(picnum,&adr);
  grabber->Get_Image_Line(linebuf,adr,1,width,height);
  lastnum=image_nr_from_timestamp(linebuf,0);
  freq=(picnum-1)*1000;
  freq/=time;
  printf("%05d images grabbed time %dms freq: %.2fHz %.2fMB/sec real %.2fMB/sec\n",picnum,(int)time,freq,(freq*width*height*2)/(1024*1024),((freq*width*height*2*12)/16)/(1024*1024));

  freq=(lastnum-firstnum)*1000;
  freq/=time;
  printf("Imagenumbers: first %d last %d count %d freq: %.2fHz\n",firstnum,lastnum,lastnum-firstnum+1,freq);
 }
 fflush(stdout);

 free(linebuf);
 return PCO_NOERROR;
}


int image_nr_from_timestamp(void *buf,int shift)
{
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


void get_number(char *number,int len)
{
   int ret_val;
   int x=0;

   while(((ret_val=getchar())!=10)&&(x<len-1))
   {
    if(isdigit(ret_val))
     number[x++]=ret_val;
   }
   number[x]=0;
}

void get_text(char *text,int len)
{
   int ret_val;
   int x=0;

   while(((ret_val=getchar())!=10)&&(x<len-1))
   {
    if(isprint(ret_val))
     text[x++]=ret_val;
   }
   text[x]=0;
}


void get_hexnumber(int *num,int len)
{
  int ret_val;
  int cmd=0;
  while(((ret_val=getchar())!=10)&&(len > 0))
  {
   if(isxdigit(ret_val))
   {
    if(ret_val<0x3A)
     cmd=(ret_val-0x30)+cmd*0x10;
    else if(ret_val<0x47)
     cmd=(ret_val-0x41+0x0A)+cmd*0x10;
    else
     cmd=(ret_val-0x61+0x0A)+cmd*0x10;
    len--;
   }
  }
  *num=cmd;
}

