//-----------------------------------------------------------------//
// Name        | sc2_cl_ser.h                | Type: ( ) source    //
//-------------------------------------------|       (*) header    //
// Project     | SC2                         |       ( ) others    //
//-----------------------------------------------------------------//
// Platform    | WINDOWS 2000/XP                                   //
//-----------------------------------------------------------------//
// Environment | Microsoft Visual C++ 6.0                          //
//             |                                                   //
//-----------------------------------------------------------------//
// Purpose     | SC2 - CameraLink Serial                           //
//-----------------------------------------------------------------//
// Author      | MBL, PCO Computer Optics GmbH                     //
//-----------------------------------------------------------------//
// Revision    | rev. 0.01 rel. 0.00                               //
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
//  0.01     | 15.04.2004 |  new file                              //
//-----------------------------------------------------------------//
//  0.0x     | xx.xx.200x |                                        //
//-----------------------------------------------------------------//

#ifndef _CLSER____H_
#define _CLSER____H_


#define CL_BAUDRATE_9600         1     //0x01
#define CL_BAUDRATE_19200        2     //0x02
#define CL_BAUDRATE_38400        4     //0x04
#define CL_BAUDRATE_57600        8     //0x08
#define CL_BAUDRATE_115200	    16     //0x10
#define CL_BAUDRATE_230400	    32     //0x20
#define CL_BAUDRATE_460800	    64     //0x40
#define CL_BAUDRATE_921600 	   128     //0x80

const int baud[9]={9600,19200,38400,57600,115200,230400,460800,921600,0};



#ifndef CLSER___EXPORT
 #define CLSER___EXPORT __declspec(dllimport)
#endif


//#define CLSER___CC __stdcall

#ifndef CLSER___CC
 #define CLSER___CC __cdecl
#endif

#ifdef __cplusplus
extern "C"{
#endif

CLSER___EXPORT int CLSER___CC
clSerialInit(unsigned long serialIndex, void** serialRefPtr);

CLSER___EXPORT int CLSER___CC
clSerialRead(void* serialRef, char* buffer,unsigned long* bufferSize,
unsigned long serialTimeout);

CLSER___EXPORT int CLSER___CC
clSerialWrite(void* serialRef, char* buffer, unsigned long* bufferSize,
unsigned long serialTimeout);

CLSER___EXPORT int CLSER___CC
clSerialClose(void* serialRef);

CLSER___EXPORT int CLSER___CC
clSetBaudRate(void* serialRef,unsigned int baudRate);

CLSER___EXPORT int CLSER___CC
clGetSupportedBaudRates (void* serialRef, unsigned int* baudRates);

CLSER___EXPORT int CLSER___CC
clGetNumBytesAvail(void* serialRef,unsigned long* bufferSize);

CLSER___EXPORT int CLSER___CC
clFlushPort(void* serialRef);

CLSER___EXPORT int CLSER___CC
clSetParity (void* serialRef, unsigned int parityOn);


CLSER___EXPORT int CLSER___CC
clGetNumSerialPorts(unsigned int* numSerialPorts);

CLSER___EXPORT int CLSER___CC
clGetManufacturerInfo(char* manufacturerName, unsigned long* bufferSize, unsigned int* version);

CLSER___EXPORT int CLSER___CC
clGetSerialPortIdentifier(unsigned long index,char *type,unsigned long *buffersize);


#ifdef __cplusplus
}
#endif
#endif

