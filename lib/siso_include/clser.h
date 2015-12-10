//+-------------------------------------------------------------------
//
//
//  Copyright (C) Silicon Software GmbH, 2003-2008
//
//  File:       clser.h
//
//
//--------------------------------------------------------------------

#ifndef _SISOCLSER
#define _SISOCLSER

#define CL_OK 0
#define CL_ERR_NO_ERR						 0
#define CL_ERR_BUFFER_TOO_SMALL			-10001
#define CL_ERR_MANU_DOES_NOT_EXIST		-10002
#define CL_ERR_PORT_IN_USE				-10003
#define CL_ERR_TIMEOUT					-10004
#define CL_ERR_INVALID_INDEX			-10005
#define CL_ERR_INVALID_REFERENCE		-10006
#define CL_ERR_ERROR_NOT_FOUND			-10007
#define CL_ERR_BAUD_RATE_NOT_SUPPORTED	-10008
#define CL_ERR_FUNCTION_NOT_FOUND		-10099

#define CL_BAUDRATE_9600		1
#define CL_BAUDRATE_19200		2
#define CL_BAUDRATE_38400		4
#define CL_BAUDRATE_57600		8
#define CL_BAUDRATE_115200		16
#define CL_BAUDRATE_230400		32
#define CL_BAUDRATE_460800		64
#define CL_BAUDRATE_921600		128


#ifdef __cplusplus
extern "C" {
#endif

int  clSerialInit	(unsigned int serialIndex, void** serialRefPtr);
int  clSerialRead	(void* serialRef, char * buffer,unsigned int * bufferSize,unsigned int serialTimeOut);
int  clSerialWrite	(void* serialRef, char * buffer,unsigned int * bufferSize,unsigned int serialTimeOut);
void clSerialClose	(void* serialRef);
int  clSetParity	(void* serialRef,unsigned int parityOn);
int  clGetManufacturerInfo    (char* ManufacturerName,unsigned int* bufferSize,unsigned int* version);
int  clGetNumSerialPorts      (unsigned int* numSerialPorts);
int  clGetSerialPortIdentifier(unsigned int serialIndex,char* portID,unsigned int* bufferSize);
int  clGetNumBytesAvail		(void* serialRef,unsigned int* bufferSize);
int  clFlushPort			(void* serialRef);
int  clGetSupportedBaudRates(void* serialRef,unsigned int* baudRates);
int  clSetBaudRate			(void* serialRef,unsigned int baudRate);
int  clGetErrorText			(int errorCode,char *errorText,unsigned int* errorTextSize);

#ifdef __cplusplus
}
#endif

#endif
