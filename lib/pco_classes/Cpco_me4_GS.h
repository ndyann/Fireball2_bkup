//-----------------------------------------------------------------//
// Name        | Cpco_me4_GS.h               | Type: ( ) source    //
//-------------------------------------------|       (*) header    //
// Project     | pco.camera                  |       ( ) others    //
//-----------------------------------------------------------------//
// Platform    | Linux                                             //
//-----------------------------------------------------------------//
// Environment |                                                   //
//             |                                                   //
//-----------------------------------------------------------------//
// Purpose     | pco.edge - CameraLink ME4 API                     //
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
// Revision History:                                               //
//  see Cpco_cl_me4_GS.cpp                                         //
//-----------------------------------------------------------------//


#include "Cpco_me4.h"

#define HAP_VERSION_NOISE 12

#define ID_DOUBLEMODE      0x01
#define ID_TIMESTAMP       0x02
#define ID_NOISEFILTER     0x03
#define ID_COLORSENSOR     0x04

class CPco_me4_edge_GS : public CPco_me4
{

public:
  DWORD Set_Grabber_Size(int width,int height);
  DWORD Set_Grabber_Param(int id, int value);
  DWORD Open_Grabber(int board);

  void Extract_Image(void *bufout,void *bufin,int width,int height);
  void Get_Image_Line(void *bufout,void *bufin,int linenumber,int width,int height);

protected:
  DWORD set_actual_size_global_shutter(int width,int height,int xlen,int ylen,int xoff,int yoff,int doublemode);
  DWORD set_actual_noisefilter(int noisefilter,int color);
  DWORD set_actual_timestamp(int timestamp);

  int act_doublemode;
  int act_timestamp;
  int act_noisefilter;
  int act_colorsensor;

};
