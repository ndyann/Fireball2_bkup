
//-----------------------------------------------------------------//
// Name        | pco_cl_includes.h           | Type: ( ) source    //
//-------------------------------------------|       (*) header    //
// Project     | SC2                         |       ( ) others    //
//-----------------------------------------------------------------//
// Platform    | Linux                                             //
//-----------------------------------------------------------------//
// Environment |                                                   //
//             |                                                   //
//-----------------------------------------------------------------//
// Purpose     | pco.camera- common header                         //
//-----------------------------------------------------------------//
// Author      | MBL, PCO AG                                       //
//-----------------------------------------------------------------//
// Revision    | rev. 1.03                                         //
//-----------------------------------------------------------------//
// Notes       | Common include files                              //
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
//-----------------------------------------------------------------//
// Rev.:     | Date:      | Changed:                               //
// --------- | ---------- | ---------------------------------------//
//  1.01     | 11.05.2010 |  new file                              //
//-----------------------------------------------------------------//
//  1.02     | 14.01.2011 |  new defines                           //
//-----------------------------------------------------------------//
//  1.03     | 24.01.2012 |  new defines                           //
//           |            |  add header Cpco_log.h                 //
//           |            |                                        //
//-----------------------------------------------------------------//
//  0.0x     | xx.xx.200x |                                        //
//-----------------------------------------------------------------//

#include <fcntl.h>      /* open */
#include <unistd.h>     /* exit */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <memory.h>
#include <ctype.h>
#include <time.h>
#include <pthread.h>

#include <sys/time.h>

#include <stdint.h>

 typedef uint8_t BYTE;
 typedef uint16_t WORD;
 typedef uint32_t DWORD;
 typedef uint64_t UINT64;

 typedef int16_t SHORT;
 typedef int32_t LONG;

 typedef void* PCO_HANDLE;
 typedef void* LPVOID;

 typedef bool  BOOL;

#define far
#define WINAPI
#define INFINITE        0xFFFFFFFF
#define WAIT_OBJECT_0   0x00000000
#define WAIT_TIMEOUT    0x00000102
#define WAIT_FAILED     0xFFFFFFFF
#define WAIT_ABANDONED  0x00000080

 typedef DWORD (*PTHREAD_FUNC)(void *dat);
 typedef PTHREAD_FUNC LPTHREAD_START_ROUTINE;


#define TRUE 1
#define FALSE 0


#include "../pco_include/Cpco_log.h"

#pragma pack(push)
#pragma pack(1)

#include "../pco_include/sc2_cl.h"

//#include "../PCO_Include/sc2_cl_ser.h"

#include "../pco_include/SC2_SDKAddendum.h"

#include "../pco_include/PCO_err.h"
#include "../pco_include/sc2_command.h"
#include "../pco_include/sc2_telegram.h"
#include "../pco_include/sc2_defs.h"

#pragma pack(pop)
