//-----------------------------------------------------------------//
// Name        | sc2_cl.h                    | Type: ( ) source    //
//-------------------------------------------|       (*) header    //
// Project     | SC2                         |       ( ) others    //
//-----------------------------------------------------------------//
// Platform    | WINDOWS 2000/XP                                   //
//-----------------------------------------------------------------//
// Environment | Microsoft Visual C++ 6.0                          //
//             |                                                   //
//-----------------------------------------------------------------//
// Purpose     | SC2 - CameraLink Header                           //
//-----------------------------------------------------------------//
// Author      | MBL, PCO Computer Optics GmbH                     //
//-----------------------------------------------------------------//
// Revision    | rev. 0.30 rel. 0.00                               //
//-----------------------------------------------------------------//
// Notes       |                                                   //
//             |                                                   //
//             |                                                   //
//-----------------------------------------------------------------//
// (c) 2003 PCO Computer Optics GmbH * Donaupark 11 *              //
// D-93309      Kelheim / Germany * Phone: +49 (0)9441 / 2005-0 *  //
// Fax: +49 (0)9441 / 2005-20 * Email: info@pco.de                 //
//-----------------------------------------------------------------//


//-----------------------------------------------------------------//
// Revision History:                                               //
//-----------------------------------------------------------------//
// Rev.:     | Date:      | Changed:                               //
// --------- | ---------- | ---------------------------------------//
//  0.01     | 14.04.2004 | new file                               //
//-----------------------------------------------------------------//
//  0.30     | 17.12.2004 | new open_cam_ext                       //
//           |            | new flag definitions                   //
//-----------------------------------------------------------------//
//  1.00     | 20.10.2005 | new defines added                      //
//-----------------------------------------------------------------//
//  0.0x     | xx.xx.200x |                                        //
//-----------------------------------------------------------------//

#ifndef SC2_CL_H
#define SC2_CL_H

#ifdef _WINDOWS

#ifdef SC2_CL_EXPORTS
 #define CEXP __declspec(dllexport) WINAPI
#elif SC2_CL_EXPORT_DEF
 #define CEXP WINAPI
#else
#if !defined CEXP
#define CEXP __declspec(dllimport) WINAPI
#endif
#endif

#if !defined DWORD
 typedef unsigned long  DWORD;         // 32 Bit data type
#endif

#endif

#include "sc2_drv_struct.h"

//timeout values in ms
typedef struct _PCO_SC2_TIMEOUTS {
   unsigned int command;
   unsigned int image;
   unsigned int transfer;
}PCO_SC2_TIMEOUTS;

typedef struct _PCO_SC2_CL_TRANSFER_PARAM_I {
   unsigned int   baudrate;              //serial baudrate
   unsigned int   ClockFrequency;        // Pixelclock in Hz
   unsigned int   CCline;                // Usage of CameraLink CC1-CC4 lines
   unsigned int   DataFormat;            // register with Testpattern and Datasplitter switch
   unsigned int   Transmit;              // single or continuous transmitting images
}PCO_SC2_CL_TRANSFER_PARAM_I;


typedef struct _PCO_SC2_CL_IMAGE_PARAM {
   unsigned int width;
   unsigned int height;
   unsigned int xoff;
   unsigned int yoff;
}PCO_SC2_CL_IMAGE_PARAM;


//default values
#define PCO_SC2_DEF_BLOCK_SIZE 512
#define PCO_SC2_MIN_COMMAND_SIZE 5

#define PCO_SC2_COMMAND_TIMEOUT 200
#define PCO_SC2_IMAGE_TIMEOUT   1500
#define PCO_SC2_IMAGE_TIMEOUT_L 3000

#define PCO_SC2_EVENT_ENABLE           0x80000000
#define PCO_SC2_POWER_DOWN_EV          0x00000001
#define PCO_SC2_CAMERA_ERROR_EV        0x00000002
#define PCO_SC2_IMAGE_READY_EV         0x00000004

/*
#define PCO_SC2_CL_DATAFORMAT  0x0F
#define PCO_SC2_CL_FORMAT_1x16 0x01
#define PCO_SC2_CL_FORMAT_2x12 0x02
#define PCO_SC2_CL_FORMAT_3x8  0x03
#define PCO_SC2_CL_FORMAT_4x16 0x04
#define PCO_SC2_CL_FORMAT_5x16 0x05
*/

#define CL_GRABBER_CAP_FORMAT_BASE       0x00000007;
#define CL_GRABBER_CAP_FORMAT_1x16       0x00000001;
#define CL_GRABBER_CAP_FORMAT_2x12       0x00000002;
#define CL_GRABBER_CAP_FORMAT_3x8        0x00000004;

#define CL_GRABBER_CAP_FORMAT_4x16       0x00000010;
#define CL_GRABBER_CAP_FORMAT_5x16       0x00000020;
#define CL_GRABBER_CAP_FORMAT_5x12       0x00000040

#define CL_GRABBER_CAP_SCCMOS_FORMAT_MODE_AD 0x000F0000;
#define CL_GRABBER_CAP_SCCMOS_FORMAT_MODE_A  0x00010000;
#define CL_GRABBER_CAP_SCCMOS_FORMAT_MODE_B  0x00020000;
#define CL_GRABBER_CAP_SCCMOS_FORMAT_MODE_C  0x00040000;
#define CL_GRABBER_CAP_SCCMOS_FORMAT_MODE_D  0x00080000;


#define PCO_SC2_CL_TESTPATTERN 0x10

#define PCO_SC2_CL_NUMBER_OF_BUFFERS 3

#define NUMEVENTS 5
#define PCO_SC2_DRIVER_ENTRY_SIZE 10
//Maximum wait entries is set to 64 in winnt.h
#define PCO_SC2_WAIT_ENTRY_OV_SIZE 50
#define PCO_SC2_BUFFER_ENTRY_SIZE 30


#define PCO_SC2_CL_ALL_AQUIRE           0x003F
//
#define PCO_SC2_CL_START_CONT           0x0001
#define PCO_SC2_CL_NON_BLOCKING_BUFFER  0x0002
#define PCO_SC2_CL_BLOCKING_BUFFER      0x0004

#define PCO_SC2_CL_STARTED              0x0010
#define PCO_SC2_CL_START_INTERNAL       0x0020
#define PCO_SC2_CL_START_PENDING        0x0040
#define PCO_SC2_CL_STOP_PENDING         0x0080

#define PCO_SC2_CL_TRANSMIT_FLAGS       0x0F00

#define PCO_SC2_CL_TRANSMIT_SET         0x0100
#define PCO_SC2_CL_TRANSMIT_DISABLED    0x0200
#define PCO_SC2_CL_TRANSMIT_CHANGE      0x0400
#define PCO_SC2_CL_TRANSMIT_WAS_SET     0x0800

//buf_manager flag
#define PCO_SC2_CL_START_ALLOCATED      0x0001
#define PCO_SC2_CL_EXTERNAL_BUFFER      0x0002
#define PCO_SC2_CL_INTERNAL_BUFFER      0x0004
#define PCO_SC2_CL_ONLY_BUFFER_HEAD     0x0008
#define PCO_SC2_CL_NO_CONTEXT           0x0010
#define PCO_SC2_CL_NO_BUFFER_HEAD       0x0020


//initmode ME3 and ME4
#define PCO_SC2_CL_ME3_LOAD_APPLET      0x0F00

#define PCO_SC2_CL_ME3_LOAD_SINGLE_AREA 0x0100
#define PCO_SC2_CL_ME3_LOAD_DUAL_AREA   0x0200
#define PCO_SC2_CL_ME3_LOAD_SINGLE_LINE 0x0300
#define PCO_SC2_CL_ME3_LOAD_DUAL_LINE   0x0400

#define PCO_SC2_CL_ME4_LOAD_FULL_AREA   0x0500
#define PCO_SC2_CL_ME4_LOAD_PCOGLSH_HAP 0x0600
#define PCO_SC2_CL_ME4_LOAD_PCO03_HAP   0x0700

#define PCO_SC2_CL_ME4_PCO_APP_TAP8     0
#define PCO_SC2_CL_ME4_PCO_APP_TAP10    1

#define PCO_SC2_CL_ME4_PCO_HAP_PCO03    0x03
#define PCO_SC2_CL_ME4_PCO_HAP_PCOGLSH  0x04


//initmode BITFLOW
#define PCO_SC2_CL_BF_DUAL_BOARD_MODE   0x0100




#define PCO_SC2_CL_CCLINES_USAGE_MASK    0x000000FF
#define PCO_SC2_CL_CCLINES_SETTING_MASK  0xFFFF0000
#define PCO_SC2_CL_CCLINE1_SETTING_MASK  0x000F0000
#define PCO_SC2_CL_CCLINE1_SETTING_OFF   16
#define PCO_SC2_CL_CCLINE2_SETTING_MASK  0x00F00000
#define PCO_SC2_CL_CCLINE2_SETTING_OFF   20
#define PCO_SC2_CL_CCLINE3_SETTING_MASK  0x0F000000
#define PCO_SC2_CL_CCLINE3_SETTING_OFF   24
#define PCO_SC2_CL_CCLINE4_SETTING_MASK  0xF0000000
#define PCO_SC2_CL_CCLINE4_SETTING_OFF   28

enum PCO_CCsel {PCO_CC_EXSYNC,PCO_CC_NOT_EXSYNC,PCO_CC_HDSYNC,PCO_CC_NOT_HDSYNC,
                PCO_CC_STROBEPULSE,PCO_CC_NOT_STROBEPULSE,PCO_CC_CLK,
                PCO_CC_GND,PCO_CC_VCC};


//defines for synch

//GET_IMAGE_QUEUED
#define PCO_SC2_CL_SYNCH_SINGLE     0x01 //synchronous input with seriell commands
#define PCO_SC2_CL_SYNCH_CONTI      0x02 //driver set Transmit enable

//synch=0x10 read_image or request_image was already sent
//synch=0x20 image is marked as cancelled

//SET_IMAGE_BUFFER
#define PCO_SC2_CL_SYNCH_SENT    0x10 //read_image or request_image was already sent
#define PCO_SC2_CL_SYNCH_CANCEL  0x20 //image is marked as cancelled


#ifdef _WINDOWS


#ifdef __cplusplus
extern "C" {            //  Assume C declarations for C++
#endif  //C++

void CEXP Enable_Error_Messages(int x,void (*func)(PSTR sz,...));
DWORD CEXP Enable_Message_Log(int m,char *name);

DWORD CEXP Get_Camera_Error();

DWORD CEXP Check_Device_Availability(DWORD num);

DWORD CEXP Open_Cam_Ext(DWORD num,HANDLE *hdriver,SC2_OpenStruct *open);

DWORD CEXP Open_Cam(DWORD num,HANDLE *hdriver);

DWORD CEXP Close_Cam(HANDLE *hdriver);

DWORD CEXP Get_Pending_Buffer(HANDLE hdriver,int *count);

DWORD CEXP Get_Image_Queued(HANDLE hdriver,DWORD timeout,
                          DWORD segment,DWORD start,DWORD end,
                          DWORD synch,void* buf, DWORD len);

DWORD CEXP Set_Image_Buffer(HANDLE hdriver,HANDLE hEvent,
                          DWORD segment,DWORD start,DWORD end,
                          DWORD synch,void* buf, DWORD len,
                          DWORD *status);

DWORD CEXP Cancel_Images(HANDLE hdriver);


DWORD CEXP Control_Command(HANDLE hdriver,
                         void *buf_in,DWORD size_in,
                         void *buf_out,DWORD size_out);


DWORD CEXP Set_Timeouts(HANDLE hdriver,void *timetable,DWORD len);

DWORD CEXP Get_Transfer_Parameters(HANDLE hdriver,void *buffer,int len);
DWORD CEXP Set_Transfer_Parameters(HANDLE hdriver,void *buffer,int len);
DWORD CEXP Set_Image_Parameters(HANDLE hdriver,void *par,int len);


DWORD CEXP Set_buffer_management(HANDLE hdriver,int flag);
DWORD CEXP Allocate_Framebuffer(HANDLE hdriver,int nr_of_buffer,void *adr_in);
DWORD CEXP Free_Framebuffer(HANDLE hdriver,void *adr_in);
DWORD CEXP Start_Aquire(HANDLE hdriver,int nr_of_ima,int flag);
DWORD CEXP Stop_Aquire(HANDLE hdriver);
DWORD CEXP Wait_for_image(HANDLE hdriver,int *nr_of_pic);
DWORD CEXP Unblock_image_buffer(HANDLE hdriver,int nr_of_pic);
DWORD CEXP Get_image_buffer_adr(HANDLE hdriver,int nr_of_pic,void **adr,int *pitch);

DWORD CEXP Get_Generation_Count(HANDLE hdriver,DWORD *count);
DWORD CEXP Save_Protocol(HANDLE hdriver,char *filename,int *count);
DWORD CEXP Set_Protocol(HANDLE hdriver,int set, int *count);
DWORD CEXP Get_Protocol(HANDLE hdriver,int *set, int *count);

#ifdef __cplusplus
}
#endif  //C++

#endif

#endif //SC2_CL_H

