//-----------------------------------------------------------------//
// Name        | Cpco_cl_com.cpp             | Type: (*) source    //
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
// Notes       | In this file are all functions and definitions,   //
//             | for commiunication with CameraLink grabbers       //
//             |                                                   //
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
// Free Software Foundation, Inc., 59 Temple Place- Suite 330,     //
// Boston, MA 02111-1307, USA                                      //
//-----------------------------------------------------------------//

//-----------------------------------------------------------------//
// Revision History:                                               //
//-----------------------------------------------------------------//
// Rev.:     | Date:      | Changed:                               //
// --------- | ---------- | ---------------------------------------//
//  1.01     | 08.10.2010 | ported from Windows SDK                //
//           |            |                                        //
// --------- | ---------- | ---------------------------------------//
//  1.02     | 14.01.2011 | new functions:                         //
//           |            | PCO_GetTemperature                     //
//           |            | PCO_SetLut                             //
//           |            | PCO_ResetSettingsToDefault             //
//           |            | PCO_SetTriggerMode                     //
//           |            | PCO_ForceTrigger                       //
//           |            |                                        //
// --------- | ---------- | ---------------------------------------//
//  1.03     | 24.01.2012 | new functions:                         //
//           |            | PCO_GetCameraType                      //
//           |            | without log class                      //
//           |            |                                        //
//-----------------------------------------------------------------//
//  0.0x     | xx.xx.2009 |                                        //
//           |            |                                        //
//-----------------------------------------------------------------//


#include "Cpco_cl_com.h"


////////////////////////////////////////////////////////////////////////////////////////////
//
//  Defines and globals
//
////////////////////////////////////////////////////////////////////////////////////////////



const char crlf[3]={0x0d,0x0a,0x00};
DWORD connected=0;

////////////////////////////////////////////////////////////////////////////////////////////
//
//  LOCAL FUNCTIONS
//
////////////////////////////////////////////////////////////////////////////////////////////


#if !defined _byteswap_ulong
inline unsigned long _byteswap_ulong(unsigned long val)
{
 int x;
 unsigned char b[4];

 for(x=0;x<4;x++)
  b[3-x]=*((unsigned char*)&val+x);

 return (unsigned long)(*(unsigned long*)b);
}
#endif

static void Sleep(int time) //time in ms
{
  int ret_val;
  fd_set rfds;
  struct timeval tv;

  FD_ZERO(&rfds);
  FD_SET(0,&rfds);


  tv.tv_sec=time/1000;
  tv.tv_usec=(time%1000)*1000;

  ret_val=select(0,NULL,NULL,NULL,&tv);
  if(ret_val<0)
  {
   fprintf(stderr,"error in select\n");
  }
}

CPco_cl_com::CPco_cl_com()
{
  log=NULL;
  hdriver = NULL;

  initmode=0;
  boardnr=-1;
  connected=0;
//  hComMutex=NULL;
}

CPco_cl_com::CPco_cl_com(int num)
{
  log=NULL;
  hdriver = NULL;

  initmode=0;
  boardnr=-1;
  connected=0;

//  hComMutex=NULL;
}


CPco_cl_com::~CPco_cl_com()
{
  if((hdriver)||(serialRef))
   Close_Cam();
}


void CPco_cl_com::writelog(DWORD lev,PCO_HANDLE hdriver,const char *str,...)
{
 if(log)
 {
  char *txt;
  txt=new char[1000];
  va_list arg;
  va_start(arg,str);
  vsprintf(txt,str,arg);
  log->writelog(lev,hdriver,txt);
  va_end(arg);
  delete[] txt;
 }
}

//return size with checksum
DWORD CPco_cl_com::build_checksum(unsigned char *buf,int *size)
{
  unsigned char cks;
  unsigned short *b;
  int x,bsize;

  b=(unsigned short *)buf;//size of packet is second WORD
  b++;
  bsize=*b-1;
  if(bsize>*size)
   return PCO_ERROR_DRIVER_CHECKSUMERROR | PCO_ERROR_DRIVER_CAMERALINK;
  cks=0;
  for(x=0;x<bsize;x++)
   cks+=buf[x];

  buf[x]=cks;
  *size=x+1;
  return PCO_NOERROR;
}

//return size with checksum
DWORD CPco_cl_com::test_checksum(unsigned char *buf,int *size)
{
  unsigned char cks;
  unsigned short *b;
  int x,bsize;
  cks=0;
  b=(unsigned short *)buf; //size of packet is second WORD
  b++;
  bsize=(int)*b;
  bsize--;
  if(bsize>*size)
  {
   writelog(ERROR_M,(PCO_HANDLE)1,"test_checksum size error");
   return PCO_ERROR_DRIVER_CHECKSUMERROR | PCO_ERROR_DRIVER_CAMERALINK;
  }

  for(x=0;x<bsize;x++)
   cks+=buf[x];

  if(buf[x]!=cks)
  {
   if(log)
    log->writelog(ERROR_M,(PCO_HANDLE)1,"test_checksum size error");
   return PCO_ERROR_DRIVER_CHECKSUMERROR | PCO_ERROR_DRIVER_CAMERALINK;
  }

  *size=x+1;
  return PCO_NOERROR;
}


DWORD CPco_cl_com::scan_camera()
{
  SC2_Simple_Telegram com;
  SC2_Camera_Type_Response resp;
  int baudrate;
  int b;
  DWORD err=PCO_NOERROR;
  DWORD buf[10]={1,2,3,4,5,6,7,8,9,0};

  b=0;
  baudrate=baudrates[b];

  writelog(INIT_M,hdriver,"scan_camera: baudrate scan with GET_CAMERA_TYPE");

  com.wCode=GET_CAMERA_TYPE;
  com.wSize=sizeof(SC2_Simple_Telegram);

  Sleep(150);
  clGetSupportedBaudRates(serialRef,buf);

  err=Control_Command(&com,sizeof(com),&resp,sizeof(SC2_Camera_Type_Response));
  if(err!=PCO_NOERROR)
  {
   writelog(INIT_M,hdriver,"scan_camera: try again baudrate %d",baudrate);
   err=Control_Command(&com,sizeof(com),&resp,sizeof(SC2_Camera_Type_Response));
  }

  while((err!=PCO_NOERROR)&&(baudrate<115200))
  {
   b++;
   baudrate=baudrates[b];

   writelog(INIT_M,hdriver,"scan_camera: test baudrate %d",baudrate);

   err=clSetBaudRate(serialRef,1<<b);
   Sleep(150);

   err=Control_Command(&com,sizeof(com),&resp,sizeof(SC2_Camera_Type_Response));
  }

  if(err==PCO_NOERROR)
   transferpar.baudrate=baudrate;

  writelog(INIT_M,hdriver,"scan_camera: baudrate set to %d",transferpar.baudrate);

  return err;
}

DWORD CPco_cl_com::set_baudrate(int baudrate)
{
 // return PCO_ERROR_DRIVER_NOFUNCTION | PCO_ERROR_DRIVER_CAMERALINK;
 DWORD err;
 int b;

 writelog(INTERNAL_1_M,hdriver,"set_baudrate: new baudrate %d",baudrate);

 SC2_Simple_Telegram com;
 SC2_Get_CL_Baudrate_Response resp;

 com.wCode=GET_CL_BAUDRATE;
 com.wSize=sizeof(com);
 err=Control_Command(&com,sizeof(com),&resp,sizeof(SC2_Get_CL_Baudrate_Response));

 if(err!=PCO_NOERROR)
 {
  writelog(ERROR_M,hdriver,"set_baudrate: GET_CL_BAUDRATE failed with 0x%x",err);
  return err;
 }

 switch(baudrate)
 {
 case   9600:
  b=CL_BAUDRATE_9600;
  break;

 case  19200:
  b=CL_BAUDRATE_19200;
  break;

 case  38400:
  b=CL_BAUDRATE_38400;
  break;

 case  57600:
  b=CL_BAUDRATE_57600;
  break;

 case 115200:
  b=CL_BAUDRATE_115200;
  break;

 default:
  writelog(ERROR_M,hdriver,"set_baudrate: baudrate %d not supported",baudrate);
  return PCO_ERROR_WRONGVALUE | PCO_ERROR_DRIVER | PCO_ERROR_DRIVER_CAMERALINK;;
 }

 SC2_Set_CL_Baudrate setbaud;

 setbaud.wCode=SET_CL_BAUDRATE;
 setbaud.wSize=sizeof(SC2_Set_CL_Baudrate);
 setbaud.dwBaudrate=baudrate;

 writelog(INTERNAL_1_M,hdriver,"set_baudrate: set camera baudrate %d",setbaud.dwBaudrate);

 err=Control_Command(&setbaud,sizeof(SC2_Set_CL_Baudrate),&resp,sizeof(SC2_Get_CL_Baudrate_Response));
 if(err!=PCO_NOERROR)
 {
  writelog(ERROR_M,hdriver,"set_baudrate: SET_CL_BAUDRATE failed with 0x%x",err);
  return err;
 }

 Sleep(200);
 writelog(INTERNAL_1_M,hdriver,"set_baudrate: set grabber baudrate %d",b);
 clSetBaudRate(serialRef,b);


 com.wCode=GET_CL_BAUDRATE;
 com.wSize=sizeof(com);
 err=Control_Command(&com,sizeof(com),&resp,sizeof(SC2_Get_CL_Baudrate_Response));

 //switch back to default baudrate of 9600
 if(err!=PCO_NOERROR)
 {
  writelog(ERROR_M,hdriver,"set_baudrate: GET_CL_BAUDRATE failed with 0x%x",err);
  b=1;
  b=1;
  clSetBaudRate(serialRef,b);
  transferpar.baudrate=9600;
 }

 transferpar.baudrate=baudrate;

 writelog(INTERNAL_1_M,hdriver,"set_baudrate: done transferpar.baudrate %d err 0x%x",transferpar.baudrate,err);
 return err;
}


DWORD CPco_cl_com::get_firmwarerev()
{
  SC2_Simple_Telegram info;
  SC2_Firmware_Versions_Response resp;
  DWORD err;


  tab_timeout.command=400;
  info.wCode=GET_FIRMWARE_VERSIONS;
  info.wSize=0x0005;

  err=Control_Command(&info,sizeof(SC2_Simple_Telegram),
                      &resp,sizeof(SC2_Firmware_Versions_Response));

  if(err==PCO_NOERROR)
  {
   int x,y;
   y=0;
   for(x=0;x<resp.DeviceNum;x++)
   {
    if(!strncmp(resp.Device[x].szName,"Main uP",7))
    {
     writelog(INFO_M,hdriver,"camera mainproz= 0x%02x%02x",resp.Device[x].bMajorRev,resp.Device[x].bMinorRev);
     camerarev|= ((resp.Device[x].bMajorRev<<8)|resp.Device[x].bMinorRev);
     y|=0x01;
    }
    if(!strncmp(resp.Device[x].szName,"Main FPGA",7))
    {
     writelog(INFO_M,hdriver,"camera mainfpga= 0x%02x%02x",resp.Device[x].bMajorRev,resp.Device[x].bMinorRev);
     camerarev|=(((resp.Device[x].bMajorRev<<8)|resp.Device[x].bMinorRev)<<16);
     y|=0x02;
    }
    if(y==0x03)
     break;
   }
  }
  else
   writelog(ERROR_M,hdriver,"get_firmwarerev: err 0x%x",err);

  tab_timeout.command=PCO_SC2_COMMAND_TIMEOUT;
  return err;
}


DWORD CPco_cl_com::get_actual_cl_config()
{
  SC2_Simple_Telegram com;
  SC2_Get_CL_Configuration_Response resp;
  SC2_Get_Interface_Output_Format com_get_if;
  SC2_Get_Interface_Output_Format_Response resp_if;
  DWORD err=PCO_NOERROR;

  com.wCode=GET_CL_CONFIGURATION;
  com.wSize=sizeof(SC2_Simple_Telegram);
  err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

  if(err!=PCO_NOERROR)
   writelog(ERROR_M,hdriver,"get_actual_cl_config: GET_CL_CONFIGURATION failed with 0x%x",err);
  else
  {
   transferpar.ClockFrequency=resp.dwClockFrequency;
   transferpar.CCline=resp.bCCline;
   transferpar.Transmit=resp.bTransmit;
   transferpar.DataFormat=resp.bDataFormat;
  }

//  if((description.wSensorTypeDESC==SENSOR_CIS2051_V1_FI_BW)
//     ||(description.wSensorTypeDESC==SENSOR_CIS2051_V1_BI_BW))
  {
   com_get_if.wCode=GET_INTERFACE_OUTPUT_FORMAT;
   com_get_if.wSize=sizeof(com_get_if);
   com_get_if.wInterface=INTERFACE_CL_SCCMOS;
   err=Control_Command(&com_get_if,sizeof(com_get_if),
                       &resp_if,sizeof(resp_if));
   if(err!=PCO_NOERROR)
    resp_if.wFormat=0;
   else
    transferpar.DataFormat|=resp_if.wFormat;
   writelog(INIT_M,hdriver,"get_actual_cl_config: GET_INTERFACE_OUTPUT_FORMAT err 0x%x format 0x%x",err,resp_if.wFormat);
  }
  return PCO_NOERROR;
}

DWORD CPco_cl_com::set_cl_config(PCO_SC2_CL_TRANSFER_PARAM cl_par)
{
  SC2_Set_CL_Configuration cl_com;
  SC2_Get_CL_Configuration_Response cl_resp;
  DWORD err=PCO_NOERROR;

  cl_com.wCode=SET_CL_CONFIGURATION;
  cl_com.wSize=sizeof(cl_com);
  cl_com.dwClockFrequency=cl_par.ClockFrequency;
  cl_com.bTransmit=cl_par.Transmit&0xFF;
  cl_com.bCCline=cl_par.CCline&0xFF;
  cl_com.bDataFormat=cl_par.DataFormat&0xFF;
  writelog(INIT_M,hdriver,"set_cl_config: Set_CL_Configuration freq:%dMHz data %08x line %04x transmit %02x",cl_com.dwClockFrequency/1000000,cl_com.bDataFormat,cl_com.bCCline,cl_com.bTransmit);

//send set CL_config
  err=Control_Command(&cl_com,sizeof(cl_com),
                      &cl_resp,sizeof(cl_resp));
  if(err!=PCO_NOERROR)
  {
   writelog(ERROR_M,hdriver,"set_cl_config: SET_CL_CONFIGURATION failed with 0x%x",err);
   return err;
  }

  if((description.wSensorTypeDESC==SENSOR_CIS2051_V1_FI_BW)
     ||(description.wSensorTypeDESC==SENSOR_CIS2051_V1_BI_BW))
  {
   SC2_Set_Interface_Output_Format com_set_if;
   SC2_Set_Interface_Output_Format_Response resp_if;

   com_set_if.wCode=SET_INTERFACE_OUTPUT_FORMAT;
   com_set_if.wSize=sizeof(com_set_if);
   com_set_if.wFormat=cl_par.DataFormat&SCCMOS_FORMAT_MASK;
   com_set_if.wInterface=INTERFACE_CL_SCCMOS;
   err=Control_Command(&com_set_if,sizeof(com_set_if),
                       &resp_if,sizeof(resp_if));
   if(err!=PCO_NOERROR)
    writelog(ERROR_M,hdriver,"set_cl_config: SCCMOS SET_INTERFACE_OUTPUT_FORMAT failed with 0x%x",err);
  }
  return err;
}


DWORD CPco_cl_com::get_lut_info()
{
  SC2_Get_Lookuptable_Info_Response resp;
  SC2_Simple_Telegram com;
  DWORD err=PCO_NOERROR;
  int x;

  num_lut=0;
  for(x=0;x<10;x++)
   memset(&cam_lut[x],0,sizeof(SC2_LUT_DESC));

  com.wCode=GET_LOOKUPTABLE_INFO;
  com.wSize=sizeof(SC2_Simple_Telegram);
  err=Control_Command(&com,sizeof(com),
                      &resp,sizeof(SC2_Get_Lookuptable_Info_Response));

  if(err!=PCO_NOERROR)
  {
   writelog(ERROR_M,hdriver,"get_lut_info: GET_LOOKUPTABLE_INFO failed with 0x%x",err);
   if((err&~PCO_ERROR_DEVICE_MASK)==PCO_ERROR_FIRMWARE_NOT_SUPPORTED)
   {
    err=PCO_NOERROR;
    writelog(ERROR_M,hdriver,"get_lut_info: PCO_ERROR_FIRMWARE_NOT_SUPPORTED return 0x%x",err);
   }
  }
  else
  {
   num_lut=resp.wLutNumber;
   writelog(INIT_M,hdriver,"get_lut_info: camera has %d luts installed",num_lut);

   for(x=0;x<num_lut;x++)
    memcpy(&cam_lut[x],&resp.LutDesc[x],sizeof(SC2_LUT_DESC));
  }
  return err;
}

////////////////////////////////////////////////////////////////////////////////////////////
//
// OPEN/CLOSE FUNCTIONS
//
//
////////////////////////////////////////////////////////////////////////////////////////////


DWORD CPco_cl_com::Open_Cam(DWORD num)
{
  return Open_Cam_Ext(num,NULL);
}

DWORD CPco_cl_com::Open_Cam_Ext(DWORD num,SC2_OpenStruct *open)
{
  int err;

  initmode=num & ~0xFF;
  num=num&0xFF;

  if(num>PCO_SC2_DRIVER_ENTRY_SIZE)
  {
   writelog(ERROR_M,(PCO_HANDLE)1,"Open_Cam_Ext: No more entries left return NODRIVER");
   return PCO_ERROR_DRIVER_NODRIVER | PCO_ERROR_DRIVER_CAMERALINK;
  }

  if(connected&(1<<num))
  {
   writelog(ERROR_M,(PCO_HANDLE)1,"Open_Cam_Ext: camera is already connected");
   return PCO_ERROR_DRIVER_NODRIVER | PCO_ERROR_DRIVER_CAMERALINK;
  }

  hdriver=(void*)(0x100+num);
  camerarev=0;

  tab_timeout.command=PCO_SC2_COMMAND_TIMEOUT;
  tab_timeout.image=PCO_SC2_IMAGE_TIMEOUT_L;
  tab_timeout.transfer=PCO_SC2_COMMAND_TIMEOUT;

  transferpar.baudrate=9600;
  transferpar.DataFormat=PCO_CL_DATAFORMAT_5x12;

  unsigned int num_port=0;
  {
   char manufacturerName[500];
   unsigned int bufferSize;
   unsigned int version;
   char type[500];
   unsigned int buffersize;

   bufferSize=sizeof(manufacturerName);
   buffersize=sizeof(type);

//   if(clGetNumSerialPorts)
   {
    clGetNumSerialPorts(&num_port);
    if(num>=num_port)
    {
     hdriver=NULL;
     writelog(ERROR_M,(PCO_HANDLE)1,"Open_Cam_Ext: board %d reqested, only %d ports found return NODRIVER",num,num_port);
	    return PCO_ERROR_DRIVER_NODRIVER | PCO_ERROR_DRIVER_CAMERALINK;
    }
   }
//   if(clGetManufacturerInfo)
   {
    clGetManufacturerInfo(manufacturerName,&bufferSize,&version);
    writelog(INIT_M,(PCO_HANDLE)1,"Open_Cam_Ext: ManufacturerName %s",manufacturerName);
   }

//   if(clGetSerialPortIdentifier)
   {
    for(unsigned int i=0;i<num_port;i++)
    {
     clGetSerialPortIdentifier(i,type,&buffersize);
     writelog(INIT_M,(PCO_HANDLE)1,"Open_Cam_Ext: PortIdentifier %d: %s",i,type);
    }
   }
  }

  err=clSerialInit(num,(void**)&serialRef);
  if(err<0)
  {
   writelog(ERROR_M,(PCO_HANDLE)1,"Open_Cam_Ext: Cannot open serial Cameralink Device %d %d",num,err);
   hdriver=NULL;
   serialRef=NULL;
   return PCO_ERROR_DRIVER_NODRIVER | PCO_ERROR_DRIVER_CAMERALINK;
  }

  boardnr=num;

//check if camera is connected, error should be timeout
//get camera descriptor to get maximal size of ccd
//scan for other baudrates than default 9600baud
  SC2_Simple_Telegram com;
  err=PCO_NOERROR;

  err=scan_camera();
  if(err!=PCO_NOERROR)
  {
   writelog(ERROR_M,hdriver,"Open_Cam_Ext: Control command failed with 0x%x, no camera connected",err);

   clSerialClose(serialRef);

   boardnr=-1;
   hdriver=NULL;
   serialRef=NULL;

   return PCO_ERROR_DRIVER_NOTINIT  | PCO_ERROR_DRIVER_CAMERALINK;
  }

  com.wCode=GET_CAMERA_DESCRIPTION;
  com.wSize=sizeof(SC2_Simple_Telegram);
  err=Control_Command(&com,sizeof(com),
                      &description,sizeof(SC2_Camera_Description_Response));

  if(err!=PCO_NOERROR)
   writelog(ERROR_M,hdriver,"Open_Cam_Ext: GET_CAMERA_DESCRIPTION failed with 0x%x",err);

  get_actual_cl_config();
  get_firmwarerev();
  get_lut_info();


  char evname[25];
  sprintf(evname,"%s%d",COM_MUTEX_NAME,num);
/*
  hComMutex=CreateMutex(NULL,FALSE,evname);
  int e=GetLastError();
  writelog(INIT_M,hdriver,"Open_Cam_Ext: Mutex 0x%x name %s created",hComMutex,evname);
  hComMutex=CreateMutex(NULL,FALSE,NULL);
  writelog(INIT_M,hdriver,"Open_Cam_Ext: GetLastError returned %d",e);
*/

  if(transferpar.baudrate!=115200)
   set_baudrate(115200);

  connected|=(1<<boardnr);

  return PCO_NOERROR;
}


DWORD CPco_cl_com::Close_Cam()
{
  writelog(PROCESS_M,hdriver,"Close_Cam:");
  if((hdriver==NULL)&&(serialRef==NULL))
  {
   writelog(INIT_M,hdriver,"Close_Cam: driver was closed before");
   return PCO_NOERROR;
  }

/*
  if(hComMutex)
  {
   writelog(INIT_M,hdriver,"Close_Cam: Close Mutex 0x%x");
   CloseHandle(hComMutex);
  }
  hComMutex=NULL;
*/
  if(serialRef!=NULL)
  {
   writelog(PROCESS_M,hdriver,"Close_Cam: clSerialClose");
   clSerialClose(serialRef);
   serialRef=NULL;
  }

  connected&=(1<<boardnr);
  boardnr=-1;
  hdriver=NULL;

  return PCO_NOERROR;
}

////////////////////////////////////////////////////////////////////////////////////////////
//
// CAMERA IO FUNCTION
//
//
////////////////////////////////////////////////////////////////////////////////////////////
DWORD CPco_cl_com::Control_Command(void *buf_in,DWORD size_in,
                                   void *buf_out,DWORD size_out)
{
  DWORD err=PCO_NOERROR;
  unsigned char buffer[PCO_SC2_DEF_BLOCK_SIZE];
  unsigned int size;
  WORD com_in,com_out;

/*
  if(hComMutex==NULL)
   writelog(COMMAND_M,hdriver,"Control_Command: No Mutex no wait");
  else
  {
   err = WaitForSingleObject(hComMutex,tab_timeout.command*3);
   writelog(COMMAND_M,hdriver,"Control_Command: Wait Mutex 0x%x done err=%d",hComMutex,err);
   switch (err)
   {
    case WAIT_OBJECT_0:
     break;
// Cannot get mutex ownership due to time-out.
    case WAIT_TIMEOUT:
    {
     writelog(ERROR_M,hdriver,"Control_Command: Timeout waiting for Mutex 0x%x",hComMutex);
     return PCO_ERROR_TIMEOUT | PCO_ERROR_DRIVER | PCO_ERROR_DRIVER_USB;
    }

// Got ownership of the abandoned mutex object.
    case WAIT_ABANDONED:
    {
     writelog(ERROR_M,hdriver,"Control_Command: Mutex abandoned");
     return PCO_ERROR_INVALIDHANDLE | PCO_ERROR_DRIVER | PCO_ERROR_DRIVER_USB;
    }
   }
  }
*/


//  if(clFlushPort)
  {
   err=clFlushPort(serialRef);
   if((int)err<0)
   {
    writelog(ERROR_M,hdriver,"Control_Command: pclSerialFlush error = %d",err);
   }
  }


  writelog(COMMAND_M,hdriver,"Control_Command: start= 0x%04x timeout %d",*(unsigned short*)buf_in,tab_timeout.command);

  com_out=0;
  com_in=*((WORD*)buf_in);
  memset(buffer,0,PCO_SC2_DEF_BLOCK_SIZE);

  size=size_in;
  err=build_checksum((unsigned char*)buf_in,(int*)&size);

  err=clSerialWrite(serialRef,(char*)buf_in,&size,tab_timeout.command);
  if((int)err<0)
  {
   writelog(ERROR_M,hdriver,"Control_Command: pclSerialWrite error = %d",err);
   err=PCO_ERROR_DRIVER_IOFAILURE | PCO_ERROR_DRIVER_CAMERALINK;
   goto sercom_out;
  }

  size=sizeof(WORD)*2;

  err=clSerialRead(serialRef,(char*)&buffer[0],&size,tab_timeout.command*2);
  if((int)err<0)
  {
   writelog(ERROR_M,hdriver,"Control_Command: pclSerialRead size:4 back %d error = %d",size,err);
   err=PCO_ERROR_DRIVER_IOFAILURE | PCO_ERROR_DRIVER_CAMERALINK;
   goto sercom_out;
  }

  com_out=*((WORD*)buffer);

  WORD *b;
  b=(WORD *)buffer; //size of packet is second WORD
  b++;
  size=(int)*b;
  if(size>PCO_SC2_DEF_BLOCK_SIZE)
   size=PCO_SC2_DEF_BLOCK_SIZE;
  size-=sizeof(WORD)*2;

  writelog(INTERNAL_1_M,hdriver,"Control_Command: before read com_out=0x%04x size %d",com_out,size);
  if((int)size<0)
  {
   writelog(ERROR_M,hdriver,"Control_Command: SerialRead remaining size<0 %d err %d com:0x%04x",size,err,*(WORD *)buffer);
   err=PCO_ERROR_DRIVER_IOFAILURE | PCO_ERROR_DRIVER_CAMERALINK;
   goto sercom_out;
  }

  if(com_in!=(com_out&0xFF3F))
  {
   writelog(ERROR_M,hdriver,"Control_Command: SerialRead comin  0x%04x != comout&0xFF3F 0x%04x",com_in,com_out&0xFF3F);
   err=PCO_ERROR_DRIVER_IOFAILURE | PCO_ERROR_DRIVER_CAMERALINK;
   goto sercom_out;
  }

  err=clSerialRead(serialRef,(char*)&buffer[sizeof(WORD)*2],&size,tab_timeout.command*2);
  if((int)err<0)
  {
   writelog(ERROR_M,hdriver,"Control_Command: pclSerialRead size:%d back %d error = %d",*b,size,err);
   err=PCO_ERROR_DRIVER_IOFAILURE | PCO_ERROR_DRIVER_CAMERALINK;
   goto sercom_out;
  }

  err=PCO_NOERROR;

  com_out=*((WORD*)buffer);
  if((com_out&RESPONSE_ERROR_CODE)==RESPONSE_ERROR_CODE)
  {
   SC2_Failure_Response resp;
   memcpy(&resp,buffer,sizeof(SC2_Failure_Response));
   err=resp.dwerrmess;
   if((err&0xC000FFFF)==PCO_ERROR_FIRMWARE_NOT_SUPPORTED)
    writelog(INTERNAL_1_M,hdriver,"Control_Command: com 0x%x FIRMWARE_NOT_SUPPORTED",com_in);
   else
    writelog(ERROR_M,hdriver,"Control_Command: com 0x%x RESPONSE_ERROR_CODE error 0x%x",com_in,err);
   size= sizeof(SC2_Failure_Response);
  }
  else
   size=size_out;

  if(err==PCO_NOERROR)
  {
   if(com_out!=(com_in|RESPONSE_OK_CODE))
   {
    err=PCO_ERROR_DRIVER_DATAERROR | PCO_ERROR_DRIVER_CAMERALINK;
    writelog(ERROR_M,hdriver,"Control_Command: Data error com_out 0x%04x should be 0x%04x",com_out,com_in|RESPONSE_OK_CODE);
   }
  }

  writelog(INTERNAL_1_M,hdriver,"Control_Command: before test_checksum read=0x%04x size %d",com_out,size);
  if(test_checksum(buffer,(int*)&size)==PCO_NOERROR)
  {
   size-=1;
   if(size<size_out)
    size_out=size;
   memcpy(buf_out,buffer,size_out);
  }
  else
   err=test_checksum(buffer,(int*)&size);

sercom_out:
/*  if(hComMutex)
  {
   if(!ReleaseMutex(hComMutex))
    writelog(ERROR_M,hdriver,"Control_Command: release Mutex failed");
   writelog(COMMAND_M,hdriver,"Control_Command: Release Mutex 0x%x done",hComMutex);
  }
*/

  writelog(COMMAND_M,hdriver,"Control_Command: Control_command end 0x%04x err 0x%x",com_out,err);
  return err ;
}

DWORD CPco_cl_com::Set_Timeouts(void *timetable,DWORD len)
{
  PCO_SC2_TIMEOUTS* tab;
  tab=(PCO_SC2_TIMEOUTS*)timetable;
  if(len>=4)
   tab_timeout.command=tab->command;
  if(len>=8)
   tab_timeout.image=tab->image;
  if(len>=12)
   tab_timeout.transfer=tab->transfer;

  return PCO_NOERROR;
}


DWORD CPco_cl_com::PCO_GetRecordingState(WORD *recstate)
{
  SC2_Recording_State_Response resp;
  SC2_Simple_Telegram com;
  DWORD err=PCO_NOERROR;

  com.wCode=GET_RECORDING_STATE;
  com.wSize=sizeof(SC2_Simple_Telegram);
  err=Control_Command(&com,sizeof(SC2_Simple_Telegram),
                      &resp,sizeof(SC2_Recording_State_Response));


  *recstate=resp.wState;
  return err;

}

#define REC_WAIT_TIME 500

DWORD CPco_cl_com::PCO_SetRecordingState(WORD recstate)
{
  WORD g_state,x;
  DWORD err=PCO_NOERROR;

  PCO_GetRecordingState(&g_state);

  if(g_state!=recstate)
  {
   DWORD s,ns;
   SC2_Set_Recording_State com;
   SC2_Recording_State_Response resp;

   com.wCode=SET_RECORDING_STATE;
   com.wState=recstate;
   com.wSize=sizeof(SC2_Set_Recording_State);
   err=Control_Command(&com,sizeof(SC2_Set_Recording_State),
                       &resp,sizeof(SC2_Recording_State_Response));

   if(err!=0)
    return err;

   PCO_GetCOCRuntime(&s,&ns);

   ns/=1000000;
   ns+=1;
   ns+=s*1000;

   ns+=REC_WAIT_TIME;
   ns/=50;

   for(x=0;x<ns;x++)
   {
    PCO_GetRecordingState(&g_state);
    if(g_state==recstate)
     break;

    Sleep(50);
   }
   if(x>=ns)
    err=PCO_ERROR_TIMEOUT;
  }
  return err;
}


DWORD CPco_cl_com::PCO_SetTimestampMode(WORD mode)
{
  SC2_Timestamp_Mode_Response resp;
  SC2_Set_Timestamp_Mode com;
  DWORD err=PCO_NOERROR;

  com.wMode=mode;

  com.wCode=SET_TIMESTAMP_MODE;
  com.wSize=sizeof(com);
  err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));
  return err;
}

DWORD CPco_cl_com::PCO_GetTimestampMode(WORD *mode)
{
  SC2_Timestamp_Mode_Response resp;
  SC2_Simple_Telegram com;
  DWORD err=PCO_NOERROR;

  com.wCode=GET_TIMESTAMP_MODE;
  com.wSize=sizeof(com);
  err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));
  *mode=resp.wMode;

  return err;
}



DWORD CPco_cl_com::PCO_ArmCamera(void)
{
  SC2_Arm_Camera_Response resp;
  SC2_Simple_Telegram com;
  DWORD err=PCO_NOERROR;
  DWORD time=5000;

  Set_Timeouts(&time,sizeof(DWORD));
  com.wCode=ARM_CAMERA;
  com.wSize=sizeof(SC2_Simple_Telegram);
  err=Control_Command(&com,sizeof(SC2_Simple_Telegram),
                      &resp,sizeof(SC2_Arm_Camera_Response));

  time=PCO_SC2_COMMAND_TIMEOUT;
  Set_Timeouts(&time,sizeof(DWORD));
  return err;
}

DWORD CPco_cl_com::PCO_GetCOCRuntime(DWORD *s,DWORD *ns)
{
  SC2_COC_Runtime_Response resp;
  SC2_Simple_Telegram com;
  DWORD err=PCO_NOERROR;

  com.wCode=GET_COC_RUNTIME;
  com.wSize=sizeof(com);
  err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

  if(err==PCO_NOERROR)
  {
   *s=resp.dwtime_s;
   *ns=resp.dwtime_ns;
  }

  return err;
}

DWORD CPco_cl_com::PCO_GetTemperature(SHORT *sCCDTemp,SHORT *sCAMTemp,SHORT *sExtTemp)
{
  SC2_Temperature_Response resp;
  SC2_Simple_Telegram com;
  DWORD err=PCO_NOERROR;

  com.wCode=GET_TEMPERATURE;
  com.wSize=sizeof(com);
  err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

  if(err==PCO_NOERROR)
  {
   *sCCDTemp=resp.sCCDtemp;
   *sCAMTemp=resp.sCamtemp;
   *sExtTemp=resp.sPStemp;
  }

  return err;
}


DWORD CPco_cl_com::PCO_RequestImage()
{
   SC2_Request_Image com;
   SC2_Request_Image_Response resp;
   DWORD err;

//send request to camera
   com.wCode=REQUEST_IMAGE;
   com.wSize=sizeof(SC2_Request_Image);
   err=Control_Command(&com,sizeof(SC2_Request_Image),
                       &resp,sizeof(SC2_Request_Image_Response));

   if(err!=PCO_NOERROR)
    writelog(ERROR_M,hdriver,"Send_Request_Image: err 0x%x com 0x%x",err,com.wCode);

   return err;
}


DWORD CPco_cl_com::PCO_GetBitAlignment(WORD *align)
{
  SC2_Bit_Alignment_Response resp;
  SC2_Simple_Telegram com;
  DWORD err=PCO_NOERROR;

  com.wCode=GET_BIT_ALIGNMENT;
  com.wSize=sizeof(com);
  err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

  *align=resp.wAlignment;
  return err;

}

DWORD CPco_cl_com::PCO_SetBitAlignment(WORD align)
{
   DWORD err;

   SC2_Set_Bit_Alignment com;
   SC2_Bit_Alignment_Response resp;

   com.wCode=SET_BIT_ALIGNMENT;
   com.wAlignment=align;
   com.wSize=sizeof(com);
   err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

   return err;
}



DWORD CPco_cl_com::PCO_GetTransferParameter(void* buf,int length)
{
  memcpy(buf,&transferpar,length);
  return PCO_NOERROR;
}


DWORD CPco_cl_com::PCO_SetTransferParameter(void* buf,int length)
{
  PCO_SC2_CL_TRANSFER_PARAM par;
  DWORD err=PCO_NOERROR;

  par.ClockFrequency=transferpar.ClockFrequency;
  par.CCline=transferpar.CCline;
  par.Transmit=transferpar.Transmit;
  par.DataFormat=transferpar.DataFormat;

  memcpy(&par,buf,length);

  if(par.baudrate!=transferpar.baudrate)
   err=set_baudrate(par.baudrate);

  if(err==PCO_NOERROR)
   err=set_cl_config(par);

  err=get_actual_cl_config();

  return err;
}


DWORD CPco_cl_com::PCO_SetDelayExposure(DWORD delay,DWORD expos)
{
   DWORD err;

   SC2_Set_Delay_Exposure com;
   SC2_Delay_Exposure_Response resp;

   com.wCode=SET_DELAY_EXPOSURE_TIME;
   com.wSize=sizeof(SC2_Set_Delay_Exposure);
   com.dwDelay=delay;
   com.dwExposure=expos;
   err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

   return err;
}

DWORD CPco_cl_com::PCO_GetDelayExposure(DWORD *delay,DWORD *expos)
{
   DWORD err;

   SC2_Simple_Telegram com;
   SC2_Delay_Exposure_Response resp;

   com.wCode=GET_DELAY_EXPOSURE_TIME;
   com.wSize=sizeof(SC2_Simple_Telegram);
   err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

   if(err==PCO_NOERROR)
   {
    *delay=resp.dwDelay;
    *expos=resp.dwExposure;
   }

   return err;
}

DWORD CPco_cl_com::PCO_SetTimebase(WORD delay,WORD expos)
{
   DWORD err;

   SC2_Set_Timebase com;
   SC2_Timebase_Response resp;

   com.wCode=SET_TIMEBASE;
   com.wSize=sizeof(SC2_Set_Timebase);
   com.wTimebaseDelay=delay;
   com.wTimebaseExposure=expos;
   err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

   return err;
}

DWORD CPco_cl_com::PCO_GetTimebase(WORD *delay,WORD *expos)
{
   DWORD err;

   SC2_Simple_Telegram com;
   SC2_Timebase_Response resp;

   com.wCode=GET_TIMEBASE;
   com.wSize=sizeof(SC2_Simple_Telegram);
   err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

   if(err==PCO_NOERROR)
   {
    *delay=resp.wTimebaseDelay;
    *expos=resp.wTimebaseExposure;
   }

   return err;
}



DWORD CPco_cl_com::PCO_GetInfo(DWORD typ,void* buf,int length)
{
   DWORD err;
   int len;

   SC2_Get_Info_String_Response resp;
   SC2_Get_Info_String com;

   com.wCode=GET_INFO_STRING;
   com.wSize=sizeof(SC2_Get_Info_String);
   com.dwType=typ;
   err=Control_Command(&com,sizeof(SC2_Get_Info_String),
                       &resp,sizeof(SC2_Get_Info_String_Response));


   if(err==PCO_NOERROR)
   {
    len=(int)strlen(resp.szName);
    if(len<length)
    {
     memcpy(buf,resp.szName,len);
     memset((char*)buf+len,0,length-len);
    }
    else
     memset(buf,0,length);

   }
   return err;
}

DWORD CPco_cl_com::PCO_GetActualSize(DWORD *width,DWORD *height)
{
   DWORD err;

   SC2_Simple_Telegram com;
   SC2_ROI_Response resp;

   com.wCode=GET_ROI;
   com.wSize=sizeof(SC2_Simple_Telegram);
   err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

   if(err==PCO_NOERROR)
   {
    *width=resp.wROI_x1-resp.wROI_x0+1;
    *height=resp.wROI_y1-resp.wROI_y0+1;
   }

   return err;
}

DWORD CPco_cl_com::PCO_GetCameraDescriptor(SC2_Camera_Description_Response *descript)
{
  memcpy(descript,&description,sizeof(SC2_Camera_Description_Response));
  return PCO_NOERROR;
}

DWORD CPco_cl_com::PCO_SetADCOperation(WORD num)
{
   DWORD err;

   SC2_Set_ADC_Operation com;
   SC2_ADC_Operation_Response resp;

   com.wCode=SET_ADC_OPERATION;
   com.wMode=num;
   com.wSize=sizeof(SC2_Set_ADC_Operation);
   err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

   return err;

}

DWORD CPco_cl_com::PCO_GetPixelRate(DWORD *PixelRate)
{
   DWORD err;

   SC2_Simple_Telegram com;
   SC2_Pixelrate_Response resp;

   com.wCode=GET_PIXELRATE;
   com.wSize=sizeof(com);
   err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));
   if(err==PCO_NOERROR)
    *PixelRate=resp.dwPixelrate;

   return err;
}

DWORD CPco_cl_com::PCO_SetPixelRate(DWORD PixelRate)
{
   DWORD err;

   SC2_Set_Pixelrate com;
   SC2_Pixelrate_Response resp;

   com.wCode=SET_PIXELRATE;
   com.dwPixelrate=PixelRate;
   com.wSize=sizeof(SC2_Set_Pixelrate);
   err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

   return err;
}

DWORD CPco_cl_com::PCO_SetLut(WORD Identifier,WORD Parameter)
{
   DWORD err;

   SC2_Set_Lookuptable com;
   SC2_Set_Lookuptable_Response resp;

   com.wCode=SET_LOOKUPTABLE;
   com.wIdentifier=Identifier;
   com.wParameter=Parameter;

   com.wSize=sizeof(SC2_Set_Lookuptable);
   err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

   return err;
}

DWORD CPco_cl_com::PCO_GetLut(WORD *Identifier,WORD *Parameter)
{
   SC2_Simple_Telegram com;
   SC2_Set_Lookuptable_Response resp;
   DWORD err;

   com.wCode=GET_LOOKUPTABLE;
   com.wSize=sizeof(com);
   err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

   if(err==PCO_NOERROR)
   {
    *Identifier=resp.wIdentifier;
    *Parameter=resp.wParameter;
   }
   return err;
}


DWORD CPco_cl_com::PCO_ResetSettingsToDefault()
{
   DWORD err;
   SC2_Simple_Telegram com;
   SC2_Reset_Settings_To_Default_Response resp;

   com.wCode=RESET_SETTINGS_TO_DEFAULT;
   com.wSize=sizeof(com);
   err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

   return err;
}

DWORD CPco_cl_com::PCO_SetTriggerMode(WORD mode)
{
   DWORD err;
   SC2_Set_Trigger_Mode com;
   SC2_Trigger_Mode_Response resp;

   com.wCode=SET_TRIGGER_MODE;
   com.wMode=mode;
   com.wSize=sizeof(SC2_Set_Trigger_Mode);
   err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

   return err;

}

DWORD CPco_cl_com::PCO_ForceTrigger(WORD *trigger)
{
   DWORD err;
   SC2_Simple_Telegram com;
   SC2_Force_Trigger_Response resp;

   com.wCode=FORCE_TRIGGER;
   com.wSize=sizeof(SC2_Simple_Telegram);
   err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

   *trigger=resp.wReturn;
   return err;
}


DWORD CPco_cl_com::PCO_GetCameraType(WORD *camtype,DWORD *serialnumber)
{
   DWORD err;
   SC2_Simple_Telegram com;
   SC2_Camera_Type_Response resp;

   com.wCode=GET_CAMERA_TYPE;
   com.wSize=sizeof(SC2_Simple_Telegram);
   err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

   *camtype=resp.wCamType;
   *serialnumber=resp.dwSerialNumber;

   return err;
}

DWORD CPco_cl_com::PCO_SetCameraToCurrentTime()
{
   DWORD err;
   SC2_Set_Date_Time com;
   SC2_Date_Time_Response resp;

   struct timeval tv;
   struct tm st;

   gettimeofday(&tv,NULL);

   localtime_r(&tv.tv_sec,&st);
   com.wCode=SET_DATE_TIME;           
   com.wSize=sizeof(SC2_Set_Date_Time);
   com.bDay=st.tm_mday;
   com.bMonth=st.tm_mon+1;
   com.wYear=st.tm_year+1900;
   com.wHours=st.tm_hour;
   com.bMinutes=st.tm_min;
   com.bSeconds=st.tm_sec;
   err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

   return err;
}
