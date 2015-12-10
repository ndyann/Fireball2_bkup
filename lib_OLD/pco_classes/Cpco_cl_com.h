//-----------------------------------------------------------------//
// Name        | Cpco_cl_com.h               | Type: ( ) source    //
//-------------------------------------------|       (*) header    //
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
// Notes       | Common functions                                  //
//             |                                                   //
//             |                                                   //
//-----------------------------------------------------------------//
// (c) 2010 - 2012 PCO AG                                          //
// Donaupark 11 D-93309  Kelheim / Germany                         //
// Phone: +49 (0)9441 / 2005-0   Fax: +49 (0)9441 / 2005-20        //
// Email: info@pco.de                                              //
//-----------------------------------------------------------------//

//-----------------------------------------------------------------//
// Revision History:                                               //
//  see Cpco_cl_com.cpp                                            //
//-----------------------------------------------------------------//


#include "pco_cl_include.h"
#include "../siso_include/clser.h"

#define COM_MUTEX_NAME "PCO_ME4_COM_"

#if !defined (MAX_PATH)
#define MAX_PATH 1024
#endif


const int baudrates[8]={9600,19200,38400,57600,115200,230400,460800,0};

class CPco_cl_com
{
 public:
  CPco_Log *log;
  WORD   num_lut;
  SC2_LUT_DESC cam_lut[10];

 protected:

//common cameralink
  PCO_HANDLE hdriver;
  DWORD  initmode;
  DWORD  boardnr;
  DWORD  camerarev;

  void* serialRef;

//  HANDLE hComMutex;
  PCO_SC2_TIMEOUTS tab_timeout;
  PCO_SC2_CL_TRANSFER_PARAM transferpar;
  SC2_Camera_Description_Response description;

//functions
 public:
  CPco_cl_com();
  CPco_cl_com(int num);

  ~CPco_cl_com();

  DWORD Open_Cam(DWORD num);
  DWORD Open_Cam_Ext(DWORD num,SC2_OpenStruct *open);
  DWORD Close_Cam();
  DWORD Control_Command(void *buf_in,DWORD size_in,
                        void *buf_out,DWORD size_out);
  DWORD Set_Timeouts(void *timetable,DWORD len);
  DWORD Get_Transfer_Parameters(void *buffer,int len);
  DWORD Set_Transfer_Parameters(void *buffer,int len);

  DWORD PCO_GetRecordingState(WORD *recstate);
  DWORD PCO_SetRecordingState(WORD recstate);
  DWORD PCO_ArmCamera(void);

  DWORD PCO_SetTimestampMode(WORD mode);
  DWORD PCO_GetTimestampMode(WORD *mode);
  DWORD PCO_GetCOCRuntime(DWORD *s,DWORD *ns);
  DWORD PCO_GetTemperature(SHORT *sCCDTemp,SHORT *sCAMTemp,SHORT *sExtTemp);
  DWORD PCO_RequestImage(void);
  DWORD PCO_GetBitAlignment(WORD *align);
  DWORD PCO_SetBitAlignment(WORD align);
  DWORD PCO_GetTransferParameter(void* buf,int length);
  DWORD PCO_SetTransferParameter(void* buf,int length);
  DWORD PCO_SetDelayExposure(DWORD delay,DWORD expos);
  DWORD PCO_GetDelayExposure(DWORD *delay,DWORD *expos);
  DWORD PCO_SetTimebase(WORD delay,WORD expos);
  DWORD PCO_GetTimebase(WORD *delay,WORD *expos);
  DWORD PCO_GetInfo(DWORD typ,void* buf,int length);
  DWORD PCO_GetActualSize(DWORD *width,DWORD *height);
  DWORD PCO_GetCameraDescriptor(SC2_Camera_Description_Response *description);
  DWORD PCO_SetADCOperation(WORD num);
  DWORD PCO_GetPixelRate(DWORD *PixelRate);
  DWORD PCO_SetPixelRate(DWORD PixelRate);

  DWORD PCO_GetCorrectionMode(WORD *corrmode);
  DWORD PCO_SetCorrectionMode(WORD corrmode);
  DWORD PCO_SetLut(WORD Identifier,WORD Parameter);
  DWORD PCO_GetLut(WORD *Identifier,WORD *Parameter);

  DWORD PCO_ResetSettingsToDefault();
  DWORD PCO_SetTriggerMode(WORD mode);
  DWORD PCO_ForceTrigger(WORD *trigger);
  DWORD PCO_GetCameraType(WORD *camtype,DWORD *serialnumber);

  DWORD PCO_SetCameraToCurrentTime();


  DWORD PCO_GetHWIOSignalCount(WORD *numSignals);
  DWORD PCO_GetHWIOSignalDescriptor(WORD SignalNum,SC2_Get_HW_IO_Signal_Descriptor_Response *SignalDesc);
  DWORD PCO_GetHWIOSignal(WORD SignalNum,SC2_Get_HW_IO_Signal_Response *OutSignal);
  DWORD PCO_SetHWIOSignal(WORD SignalNum,SC2_Set_HW_IO_Signal *InSignal);


 protected:
  DWORD Get_Serial_Functions();
  DWORD build_checksum(unsigned char *buf,int *size);
  DWORD test_checksum(unsigned char *buf,int *size);
  DWORD scan_camera();
  DWORD set_baudrate(int baudrate);
  DWORD get_firmwarerev();
  DWORD get_actual_cl_config();
  DWORD set_cl_config(PCO_SC2_CL_TRANSFER_PARAM cl_par);
  DWORD get_lut_info();
  void writelog(DWORD lev,PCO_HANDLE hdriver,const char *str,...);
};



