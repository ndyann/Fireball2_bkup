//-----------------------------------------------------------------//
// Name        | Cpco_me4.h                  | Type: ( ) source    //
//-------------------------------------------|       (*) header    //
// Project     | pco.camera                  |       ( ) others    //
//-----------------------------------------------------------------//
// Platform    | Linux                                             //
//-----------------------------------------------------------------//
// Environment |                                                   //
//             |                                                   //
//-----------------------------------------------------------------//
// Purpose     | pco.camera - CameraLink ME4 API                   //
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
//  see Cpco_cl_me4.cpp                                            //
//-----------------------------------------------------------------//


#include "../siso_include/fgrab_prototyp.h"
#include "../siso_include/fgrab_struct.h"
#include "../siso_include/fgrab_define.h"

#include "pco_cl_include.h"

class CPco_me4
{
public:
  CPco_Log *log;


protected:
  PCO_HANDLE hgrabber;

  Fg_Struct *fg;
  int pco_hap_loaded;
  int act_width,act_height;
  int port;
  int me_boardnr;
  int DataFormat;

  int buf_manager;
  dma_mem*  pMem0;
  dma_mem*  pMemInt;
  unsigned char** padr;
  int size_alloc;
  int nr_of_buffer;

  int aquire_status;
  int aquire_flag;
  int last_picnr;
  int act_dmalength;
  int act_sccmos_version;


public:
  CPco_me4();
  ~CPco_me4();


  virtual DWORD Set_Grabber_Size(int width,int height)=0;
  virtual DWORD Open_Grabber(int board)=0;
  virtual void Extract_Image(void *bufout,void *bufin,int width,int height)=0;
  virtual void Get_Image_Line(void *bufout,void *bufin,int linenumber,int width,int height)=0;

  virtual DWORD Check_DRAM_Status(char *mess,int mess_size,int *fill){return PCO_NOERROR;}

  DWORD Close_Grabber();
  DWORD Set_DataFormat(DWORD dataformat);
  DWORD Set_Grabber_Timeout(int timeout);

  DWORD Allocate_Framebuffer(int nr_of_buffer);
  DWORD Allocate_Framebuffer(int nr_of_buffer,void *adr_in);

  DWORD Free_Framebuffer();
  DWORD Free_Framebuffer(void *adr_in);

  DWORD Get_Framebuffer();

  DWORD Start_Aquire(int nr_of_ima,int flag);
  DWORD Start_Aquire(int num);
  DWORD Stop_Aquire();

  BOOL started();

  DWORD Wait_For_Image(int *nr_of_pic,int timeout);
  DWORD Get_last_Image(int *nr_of_pic);

  DWORD Get_Framebuffer_adr(int nr_of_pic,void **adr);
  int Get_actual_size(unsigned int *width,unsigned int *height,unsigned int *bitpix);

  DWORD Check_DMA_Length(int num);


protected:
  void writelog(DWORD lev,PCO_HANDLE hdriver,const char *str,...);
  void Fg_Error(Fg_Struct *fg);
};

class CPco_me4_edge : public CPco_me4
{

public:
  DWORD Set_Grabber_Size(int width,int height);
  DWORD Open_Grabber(int board);
  void Extract_Image(void *bufout,void *bufin,int width,int height);
  void Get_Image_Line(void *bufout,void *bufin,int linenumber,int width,int height);

  DWORD Check_DRAM_Status(char *mess,int mess_size,int *fill);

protected:
  int set_sccmos_size(int width,int height,int xlen,int ylen,int xoff,int yoff);
};

class CPco_me4_camera : public CPco_me4
{

public:
  DWORD Set_Grabber_Size(int width,int height);
  DWORD Open_Grabber(int board);
  void Extract_Image(void *bufout,void *bufin,int width,int height);
  void Get_Image_Line(void *bufout,void *bufin,int linenumber,int width,int height);

};

