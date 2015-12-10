//-----------------------------------------------------------------//
// Name        | defs.h                      | Type: ( ) source    //
//-------------------------------------------|       (*) header    //
// Project     | pco.camera                  |       ( ) others    //
//-----------------------------------------------------------------//
// Platform    | Linux                                             //
//-----------------------------------------------------------------//
// Environment |                                                   //
//             |                                                   //
//-----------------------------------------------------------------//
// Purpose     | pco defines                                       //
//-----------------------------------------------------------------//
// Author      | MBL, PCO AG                                       //
//-----------------------------------------------------------------//
// Revision    | rev. 1.01                                         //
//-----------------------------------------------------------------//
// Notes       |                                                   //
//             |                                                   //
//             |                                                   //
//-----------------------------------------------------------------//
// (c) 2010 - 2012 PCO AG                                          //
// Donaupark 11 D-93309  Kelheim / Germany                         //
// Phone: +49 (0)9441 / 2005-0   Fax: +49 (0)9441 / 2005-20        //
// Email: info@pco.de                                              //
//-----------------------------------------------------------------//

#ifndef _DEFS_H_
#define _DEFS_H_

#include <stdint.h>

typedef uint8_t BYTE;
typedef uint16_t WORD;
typedef uint32_t DWORD;
typedef uint64_t UINT64;

typedef int16_t SHORT;
typedef int32_t LONG;

typedef void* PCO_HANDLE;
typedef void* LPVOID;

typedef int __int32;
typedef unsigned int __uint32;

typedef long long int __int64;
typedef unsigned long long int __uint64;

#define far

#define FALSE 0
#define TRUE 1

#define MAX_TELEGRAM_LENGTH 300
#define PCO_SC2_FW_REALSIZE 0x00010000


#endif
