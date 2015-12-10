//-----------------------------------------------------------------//
// Name        | Cpco_me4_GS.cpp             | Type: (*) source    //
//-------------------------------------------|       ( ) header    //
// Project     | SC2                         |       ( ) others    //
//-----------------------------------------------------------------//
// Platform    |                                                   //
//-----------------------------------------------------------------//
// Environment |                                                   //
//             |                                                   //
//-----------------------------------------------------------------//
// Purpose     |                                                   //
//-----------------------------------------------------------------//
// Author      | MBL, PCO AG                                       //
//-----------------------------------------------------------------//
// Revision    | rev. 1.03                                         //
//-----------------------------------------------------------------//
// Notes       | edge global shutter set_actual_size function      //
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
// Free Software Foundation, Inc., 59 Temple Place                 //
// - Suite 330, Boston, MA 02111-1307, USA                         //
//-----------------------------------------------------------------//

//-----------------------------------------------------------------//
// Revision History:                                               //
//-----------------------------------------------------------------//
// Rev.:     | Date:      | Changed:                               //
// --------- | ---------- | ---------------------------------------//
//  1.03     | 24.01.2012 | new class for pco.edge Global Shutter  //
//           |            |                                        //
//           |            |                                        //
//-----------------------------------------------------------------//
//  0.0x     | xx.xx.2009 |                                        //
//           |            |                                        //
//-----------------------------------------------------------------//



#include "Cpco_me4_GS.h"
#include "reorder_func.h"


#define LINETAB_SIZE 8192
#define OFFSET 300

#define HAPNAME "PCO_scmos_gl_linux32.hap"

static int strcpy_s(char* buf, int dwlen, const char* src)
{
  strcpy(buf,src);
  return 0;
}


DWORD CPco_me4_edge_GS::Set_Grabber_Size(int width,int height)
{
  DWORD err;
  writelog(PROCESS_M,hgrabber,"edge_GS set_actual_size: start w:%d h:%d",width,height);

  if(pco_hap_loaded!=PCO_SC2_CL_ME4_PCO_HAP_PCOGLSH)
   return PCO_ERROR_DRIVER_INVMODE | PCO_ERROR_DRIVER_CAMERALINK;

  if((DataFormat&PCO_CL_DATAFORMAT_MASK)!=PCO_CL_DATAFORMAT_5x12)
   return PCO_ERROR_DRIVER_INVMODE | PCO_ERROR_DRIVER_CAMERALINK;

  err=set_actual_size_global_shutter(width,height,width,height,0,0,act_doublemode);
  if(err==PCO_NOERROR)
  {
   act_width=width;
   act_height=height;
   act_dmalength=width*height*2;

   if(act_timestamp)
    err=set_actual_timestamp(act_timestamp);

   if((err==PCO_NOERROR)&&(act_noisefilter))
    err=set_actual_noisefilter(act_noisefilter,act_colorsensor);
  }

  return err;
}


DWORD CPco_me4_edge_GS::Set_Grabber_Param(int id,int value)
{
  switch(id)
  {
   default:
    return PCO_ERROR_WRONGVALUE | PCO_ERROR_DRIVER | PCO_ERROR_DRIVER_CAMERALINK;

   case ID_DOUBLEMODE:
    act_doublemode=value;
    break;

   case ID_TIMESTAMP:
    act_timestamp=value;
    break;

   case ID_NOISEFILTER:
    act_noisefilter=value;
    break;

   case ID_COLORSENSOR:
    act_colorsensor=value;
    break;
  }
  return PCO_NOERROR;
}


DWORD CPco_me4_edge_GS::Open_Grabber(int board)
{
  int err=PCO_NOERROR;

  if(fg!=NULL)
  {
   writelog(INIT_M,(PCO_HANDLE)1,"Open_Grabber: grabber was opened before");
   return PCO_NOERROR;
  }

  int num,type;

//reset this settings
  me_boardnr=-1;
  port=PORT_A;

  act_doublemode=0;
  act_timestamp=0;
  act_noisefilter=0;
  act_colorsensor=0;

//scan for me4_board and adjust me4_boardnr
  num=0;
  for(int i=0;i<4;i++)
  {
   type=Fg_getBoardType(i);
   if(type<0)
    break;
   if((type==0xA41)||(type==0xA44))
   {
    if(((num*2)==board)||((num*2)+1==board))
    {
     me_boardnr=i;
     break;
    }
    num++;
   }
  }

  if(me_boardnr<0)
  {
   writelog(ERROR_M,hgrabber,"open_grabber: driver supports only up to 4 boards");

   return PCO_ERROR_DRIVER_NODRIVER | PCO_ERROR_DRIVER_CAMERALINK;
  }

  hgrabber=(void*)(0x1000+board);

  char buf[MAX_PATH];
//  int hFile;
//  char *sisodir;


  memset(buf,0,MAX_PATH);

  strcpy_s(buf,sizeof(buf),HAPNAME);
  pco_hap_loaded=0;

  writelog(INIT_M,hgrabber,"open_grabber: Fg_Init(%s,%d)",buf,me_boardnr);
  if((fg = Fg_Init(buf,me_boardnr)) == NULL)
  {
   err=Fg_getLastErrorNumber(NULL);
   writelog(ERROR_M,hgrabber,"open_grabber: Fg_Init(%s,%d) failed with err %d",buf,me_boardnr,err);
   if(err == FG_NO_VALID_LICENSE)
    writelog(ERROR_M,hgrabber,"open_grabber: Missing license for this mode");
   Fg_Error(fg);
   hgrabber=NULL;
   me_boardnr=-1;
   return PCO_ERROR_DRIVER_NODRIVER | PCO_ERROR_DRIVER_CAMERALINK;
  }


  err=PCO_NOERROR;
  if(Fg_getParameter(fg,FG_CAMSTATUS,&num,port)<0)
  {
   Fg_Error(fg);
   writelog(ERROR_M,hgrabber,"open_grabber: Fg_getParameter(,FG_CAMSTATUS,...) failed");
   err=PCO_ERROR_DRIVER_DATAERROR | PCO_ERROR_DRIVER_CAMERALINK;
  }

  if((num==0)&&(err==PCO_NOERROR))
  {
   writelog(ERROR_M,hgrabber,"open_grabber: Fg_getParameter(,FG_CAMSTATUS,...) no camera connected");
   err=PCO_ERROR_DRIVER_NOTINIT | PCO_ERROR_DRIVER_CAMERALINK;
  }
  int val;

  if(err==PCO_NOERROR)
  {
   int IdVersion;

   IdVersion = Fg_getParameterIdByName(fg, "Device1_Process0_version_Value");
   if(IdVersion<0)
    writelog(INIT_M,hgrabber,"open_grabber: HAP_PCOGLSH version not avaiable");
   else
   {
    int val;
    Fg_getParameterWithType(fg, IdVersion, &val, 0);
    writelog(INIT_M,hgrabber,"open_grabber: HAP_PCOGLSH version %d",val);
    act_sccmos_version=val;
   }

   DataFormat=SCCMOS_FORMAT_TOP_CENTER_BOTTOM_CENTER|PCO_CL_DATAFORMAT_5x12;
   pco_hap_loaded=PCO_SC2_CL_ME4_PCO_HAP_PCOGLSH;
  }

  if(err==PCO_NOERROR)
  {
   val=0xFFFFFFF; //(PCO_SC2_IMAGE_TIMEOUT_L*10)/1000;
   if(Fg_setParameter(fg, FG_TIMEOUT,&val,port)<0)
   {
    Fg_Error(fg);
    writelog(ERROR_M,hgrabber,"open_grabber: set FG_TIMEOUT failed");
    err=PCO_ERROR_DRIVER_DATAERROR | PCO_ERROR_DRIVER_CAMERALINK;
   }
  }

//set default sizes
  if(err==PCO_NOERROR)
   err=Set_Grabber_Size(2560,2160);

  if(err!=PCO_NOERROR)
  {
   Fg_FreeGrabber(fg);
   fg=NULL;
   me_boardnr=-1;
   hgrabber=NULL;
  }

  buf_manager=PCO_SC2_CL_INTERNAL_BUFFER;

  return err;
}


void CPco_me4_edge_GS::Extract_Image(void *bufout,void *bufin,int width,int height)
{
  int ysize,size;
  unsigned short *buf_in=(unsigned short*)bufin;
  unsigned short *buf_out=(unsigned short*)bufout;

  ysize=height;
  if(act_doublemode)
   ysize/=2;

  size=width*ysize;
  buf_in+=size;
  buf_out+=size;

  switch(DataFormat&SCCMOS_FORMAT_MASK)
  {
   default:
   case	SCCMOS_FORMAT_TOP_CENTER_BOTTOM_CENTER: //MODE_A
   {
    sort_lines_A(width,ysize,bufout,bufin);
    if(act_doublemode)
     sort_lines_A(width,ysize,buf_out,buf_in);
   }
   break;

   case	SCCMOS_FORMAT_CENTER_TOP_CENTER_BOTTOM: //MODE_B
   {
    sort_lines_B(width,ysize,bufout,bufin);
    if(act_doublemode)
     sort_lines_B(width,ysize,buf_out,buf_in);
   }
   break;

   case	SCCMOS_FORMAT_CENTER_TOP_BOTTOM_CENTER: //MODE_C:
   {
    sort_lines_C(width,ysize,bufout,bufin);
    if(act_doublemode)
     sort_lines_C(width,ysize,buf_out,buf_in);
   } 
   break;

   case	SCCMOS_FORMAT_TOP_CENTER_CENTER_BOTTOM: //MODE_D:
   {
    sort_lines_D(width,ysize,bufout,bufin);
    if(act_doublemode)
     sort_lines_D(width,ysize,buf_out,buf_in);
   }
   break;

   case	SCCMOS_FORMAT_TOP_BOTTOM: //MODE_E:
   {
    memcpy(bufout,bufin,size*sizeof(unsigned short));
    if(act_doublemode)
     memcpy(buf_out,buf_in,size*sizeof(unsigned short));
   }
   break;
  }
}

void CPco_me4_edge_GS::Get_Image_Line(void *bufout,void *bufin,int linenumber,int width,int height)
{
  WORD *buf;
  int ysize,size;

  ysize=height;
  if(act_doublemode)
   ysize/=2;

  size=width*ysize;

  buf=(WORD*)bufin;
  linenumber--;

  switch(DataFormat&SCCMOS_FORMAT_MASK)
  {
   default:
   case	SCCMOS_FORMAT_TOP_CENTER_BOTTOM_CENTER: //MODE_A
   {
    if((act_doublemode==1)&&(linenumber+1>ysize))
    {
     buf+=size;
     linenumber-=ysize;
    }

    if(linenumber<ysize/2)
    {
     buf+=(linenumber*width*2);
    }
    else
    {
     linenumber=ysize-linenumber;
     buf-=width;
     buf+=(linenumber*width*2);
    }
   }
   break;
  }
  memcpy(bufout,buf,width*sizeof(WORD));
}




DWORD CPco_me4_edge_GS::set_actual_noisefilter(int noisefiltermode,int color)
{
  unsigned int Value;
  int rcode, nError;

  if(pco_hap_loaded!=PCO_SC2_CL_ME4_PCO_HAP_PCOGLSH)
   return PCO_ERROR_DRIVER_INVMODE | PCO_ERROR_DRIVER_CAMERALINK;

  if(act_sccmos_version<HAP_VERSION_NOISE)
   return PCO_NOERROR;

  int NoiseEnable_Id = Fg_getParameterIdByName(fg, "Device1_Process0_NoiseFilter_MedianEnable_Value");
  int SetColour_Id   = Fg_getParameterIdByName(fg, "Device1_Process0_Kernel_SetColour_Value");

  writelog(INIT_M,hgrabber,"set_actual_noisefilter: Noisefilter  %s",noisefiltermode ? " ON" : "OFF");

  nError = 0;

  if(noisefiltermode)
   Value = 1;
  else
   Value = 0;
  rcode = Fg_setParameterWithType(fg, NoiseEnable_Id, &Value, 0, FG_PARAM_TYPE_INT32_T);
  if(rcode)
  {
   nError++;
   writelog(INIT_M,hgrabber,"set_actual_noisefilter: Error Noise set %d",Value);
  }
  else
   writelog(INIT_M,hgrabber,"set_actual_noisefilter: Noise set %d",Value);

  if(color)
   Value = 1;
  else
   Value = 0;

  rcode = Fg_setParameterWithType(fg, SetColour_Id, &Value, 0, FG_PARAM_TYPE_INT32_T);
  if(rcode)
  {
   nError++;
   writelog(INIT_M,hgrabber,"set_actual_noisefilter: Error noise color set %d",Value);
  }
  else
    writelog(INIT_M,hgrabber,"set_actual_noisefilter: Noise color set %d",Value);


  /*============ Error handling  ============== */

  if(nError)
  {
   writelog(ERROR_M,hgrabber,"set_actual_noisefilter: error because some FG_setParameter failed Errcount %d",nError);
   return PCO_ERROR_DRIVER_DATAERROR | PCO_ERROR_DRIVER_CAMERALINK;
  }

  return PCO_NOERROR;
}

DWORD CPco_me4_edge_GS::set_actual_timestamp(int timestamp)
{
  unsigned int Value;
  int rcode, nError;

  if(pco_hap_loaded!=PCO_SC2_CL_ME4_PCO_HAP_PCOGLSH)
   return PCO_ERROR_DRIVER_INVMODE | PCO_ERROR_DRIVER_CAMERALINK;

  if(act_sccmos_version<HAP_VERSION_NOISE)
   return PCO_NOERROR;

  nError = 0;

  int IdTimeStamp_Ascii_Enable  = Fg_getParameterIdByName(fg, "Device1_Process0_SubImages_TimeStamp_Ascii_Enable_Value");
  int IdTimeStamp_Binary_Enable = Fg_getParameterIdByName(fg, "Device1_Process0_SubImages_TimeStamp_Bin_Enable_Value");


  writelog(INIT_M,hgrabber,"set_actual_timestamp: 0x%04x",timestamp);

  /*============ Timestamp  ============== */

  if((timestamp == 0x1)||(timestamp == 0x2))
   Value = 1;
  else
   Value = 0;

  rcode = Fg_setParameterWithType(fg, IdTimeStamp_Binary_Enable, &Value, 0, FG_PARAM_TYPE_INT32_T);
  if(rcode)
  {
   nError++;
   writelog(INIT_M,hgrabber,"set_actual_timestamp: Error Timestamp Binary set %d",Value);
  }
  else
   writelog(INIT_M,hgrabber,"set_actual_timestamp: Timestamp Binary %d",Value);

  if((timestamp == 0x2) || (timestamp == 0x3))
   Value = 1;
  else
   Value = 0;
  rcode = Fg_setParameterWithType(fg, IdTimeStamp_Ascii_Enable, &Value, 0, FG_PARAM_TYPE_INT32_T);
  if(rcode)
  {
   nError++;
   writelog(INIT_M,hgrabber,"set_actual_timestamp: Error Timestamp ASCII set %d",Value);
  }
  else
    writelog(INIT_M,hgrabber,"set_actual_timestamp: Timestamp ASCII %d",Value);

  /*============ Error handling  ============== */

  if(nError)
  {
   writelog(ERROR_M,hgrabber,"set_actual_timestamp: error because some FG_setParameter failed Errcount %d",nError);
   return PCO_ERROR_DRIVER_DATAERROR | PCO_ERROR_DRIVER_CAMERALINK;
  }

  return PCO_NOERROR;
}


DWORD CPco_me4_edge_GS::set_actual_size_global_shutter(int width,int height,int xlen,int ylen,int xoff,int yoff,int doublemode)
{
  int rcode, nError;
  int Idxoff0, Idxlen0, Idyoff0, Idylen0;
  int Idxoff1, Idxlen1, Idyoff1, Idylen1;
  int Idxoff2, Idxlen2, Idyoff2, Idylen2;
  int Idxoff3, Idxlen3, Idyoff3, Idylen3;
  int Idoffset, Idenable, Idsplitter;
  int ConstLGShift4_Value_Id, ConstHGShift4_Value_Id;
  int ypos[LINETAB_SIZE];
  int granularity;
  int x_pre_len;
  unsigned int  Value;


  nError=0;

  writelog(INIT_M,hgrabber,"set_actual_size_global_shutter: START Double %s",doublemode ? "ON" : "OFF");

  if((act_sccmos_version<HAP_VERSION_NOISE)&&(doublemode!=0))
   return PCO_ERROR_DRIVER_INVMODE | PCO_ERROR_DRIVER_CAMERALINK;



  if(doublemode)
   height/=2;

  granularity=5;

  if(    (xoff < 0)
     ||  (xoff > 4095)
     ||  ((xoff % granularity) != 0)
     ||  (xlen < granularity)
     ||  (xlen > 4095)
     ||  ((xlen % granularity) != 0)
     ||  ((xoff + xlen) > 4096)
     ||  (yoff < 0)
     ||  (yoff > LINETAB_SIZE-1)
     ||  (ylen < 1)
     ||  (height > LINETAB_SIZE)
     ||  ((yoff + ylen) > LINETAB_SIZE)
    )
  {
   writelog(ERROR_M,hgrabber,"set_actual_size_global_shutter: Parameter check failed");
   return PCO_ERROR_DRIVER_DATAERROR | PCO_ERROR_DRIVER_CAMERALINK;
  }



  writelog(INIT_M,hgrabber,"set_actual_size_global_shutter: Get ID's");


  Idxoff0  = Fg_getParameterIdByName(fg, "Device1_Process0_ImageBufferReset_DRAM0_XOffset");
  Idxlen0  = Fg_getParameterIdByName(fg, "Device1_Process0_ImageBufferReset_DRAM0_XLength");
  Idyoff0  = Fg_getParameterIdByName(fg, "Device1_Process0_ImageBufferReset_DRAM0_YOffset");
  Idylen0  = Fg_getParameterIdByName(fg, "Device1_Process0_ImageBufferReset_DRAM0_YLength");

  Idxoff1  = Fg_getParameterIdByName(fg, "Device1_Process0_ImageBufferReset_DRAM1_XOffset");
  Idxlen1  = Fg_getParameterIdByName(fg, "Device1_Process0_ImageBufferReset_DRAM1_XLength");
  Idyoff1  = Fg_getParameterIdByName(fg, "Device1_Process0_ImageBufferReset_DRAM1_YOffset");
  Idylen1  = Fg_getParameterIdByName(fg, "Device1_Process0_ImageBufferReset_DRAM1_YLength");

  Idxoff2  = Fg_getParameterIdByName(fg, "Device1_Process0_ImageBufferData_DRAM0_XOffset");
  Idxlen2  = Fg_getParameterIdByName(fg, "Device1_Process0_ImageBufferData_DRAM0_XLength");
  Idyoff2  = Fg_getParameterIdByName(fg, "Device1_Process0_ImageBufferData_DRAM0_YOffset");
  Idylen2  = Fg_getParameterIdByName(fg, "Device1_Process0_ImageBufferData_DRAM0_YLength");

  Idxoff3  = Fg_getParameterIdByName(fg, "Device1_Process0_ImageBufferData_DRAM1_XOffset");
  Idxlen3  = Fg_getParameterIdByName(fg, "Device1_Process0_ImageBufferData_DRAM1_XLength");
  Idyoff3  = Fg_getParameterIdByName(fg, "Device1_Process0_ImageBufferData_DRAM1_YOffset");
  Idylen3  = Fg_getParameterIdByName(fg, "Device1_Process0_ImageBufferData_DRAM1_YLength");

  Idsplitter    = Fg_getParameterIdByName(fg, "Device1_Process0_DoubleFrameSplit_Splitter_ImageHeight");

  Idenable      = Fg_getParameterIdByName(fg, "Device1_Process0_EnableSubImage_Enable_Value");
  Idoffset      = Fg_getParameterIdByName(fg, "Device1_Process0_SubImages_Offset_Value");

  ConstLGShift4_Value_Id = Fg_getParameterIdByName(fg, "Device1_Process0_Match16Bit_ConstLGShift4_Value");
  ConstHGShift4_Value_Id = Fg_getParameterIdByName(fg, "Device1_Process0_Match16Bit_ConstHGShift4_Value");

  memset(ypos,0,sizeof(ypos));

  nError  = 0;


//==Image Buffer==//
  x_pre_len = xlen / 10;

  rcode = Fg_setParameterWithType(fg, Idxoff0, &xoff, 0, FG_PARAM_TYPE_UINT32_T);
  if(rcode)
   nError++;
  rcode = Fg_setParameterWithType(fg, Idxlen0, &x_pre_len, 0, FG_PARAM_TYPE_UINT32_T);
  if(rcode)
   nError++;
  rcode = Fg_setParameterWithType(fg, Idyoff0, &yoff, 0, FG_PARAM_TYPE_UINT32_T);
  if(rcode)
   nError++;
  rcode = Fg_setParameterWithType(fg, Idylen0, &height, 0, FG_PARAM_TYPE_UINT32_T);
  if(rcode)
   nError++;

  rcode = Fg_setParameterWithType(fg, Idxoff1, &xoff, 0, FG_PARAM_TYPE_UINT32_T);
  if(rcode)
   nError++;
  rcode = Fg_setParameterWithType(fg, Idxlen1, &x_pre_len, 0, FG_PARAM_TYPE_UINT32_T);
  if(rcode)
   nError++;
  rcode = Fg_setParameterWithType(fg, Idyoff1, &yoff, 0, FG_PARAM_TYPE_UINT32_T);
  if(rcode)
   nError++;
  rcode = Fg_setParameterWithType(fg, Idylen1, &height, 0, FG_PARAM_TYPE_UINT32_T);
  if(rcode)
   nError++;

  rcode = Fg_setParameterWithType(fg, Idxoff2, &xoff, 0, FG_PARAM_TYPE_UINT32_T);
  if(rcode)
   nError++;
  rcode = Fg_setParameterWithType(fg, Idxlen2, &x_pre_len, 0, FG_PARAM_TYPE_UINT32_T);
  if(rcode)
   nError++;
  rcode = Fg_setParameterWithType(fg, Idyoff2, &yoff, 0, FG_PARAM_TYPE_UINT32_T);
  if(rcode)
   nError++;
  rcode = Fg_setParameterWithType(fg, Idylen2, &height, 0, FG_PARAM_TYPE_UINT32_T);
  if(rcode)
   nError++;

  rcode = Fg_setParameterWithType(fg, Idxoff3, &xoff, 0, FG_PARAM_TYPE_UINT32_T);
  if(rcode)
   nError++;
  rcode = Fg_setParameterWithType(fg, Idxlen3, &x_pre_len, 0, FG_PARAM_TYPE_UINT32_T);
  if(rcode)
   nError++;
  rcode = Fg_setParameterWithType(fg, Idyoff3, &yoff, 0, FG_PARAM_TYPE_UINT32_T);
  if(rcode)
   nError++;
  rcode = Fg_setParameterWithType(fg, Idylen3, &height, 0, FG_PARAM_TYPE_UINT32_T);
  if(rcode)
   nError++;

  if(nError)
   writelog(INIT_M,hgrabber,"set_actual_size_global_shutter: Single parameter failed");
  else
   writelog(INIT_M,hgrabber,"set_actual_size_global_shutter: Single parameter OK");


/*============ CONST : Offset ============== */
  Value = 350;
//  rcode = Fg_setParameterWithType(entry->fg, Idoffset, &Value, 0, FG_PARAM_TYPE_INT32_T);
//  if(rcode)
//   nError++;


/*============ Image Splitter  ============== */

 Value = height;
 rcode = Fg_setParameterWithType(fg, Idsplitter, &Value, 0, FG_PARAM_TYPE_UINT32_T);
 if(rcode)
  nError++;
 else
  writelog(INIT_M,hgrabber,"set_actual_size_global_shutter: Idsplitter OK");


/*============ HG/LG Mult  ============== */
  Value = 128; //*32 << 2 = 128
  Fg_setParameterWithType(fg, ConstLGShift4_Value_Id, &Value, 0, FG_PARAM_TYPE_INT32_T);
  if(rcode)
   nError++;
  else
   writelog(INIT_M,hgrabber,"set_actual_size_global_shutter: LG Mult %d (%.02f)",Value,(float)(Value/4.0));

  Value = 13; //*(32/10)=3.2 << 2 = 13 -> 3.25
  Fg_setParameterWithType(fg, ConstHGShift4_Value_Id, &Value, 0, FG_PARAM_TYPE_INT32_T);
  if(rcode)
   nError++;
  else
   writelog(INIT_M,hgrabber,"set_actual_size_global_shutter: HG Mult %d (%.02f)",Value,(float)(Value/4.0));


  if(act_sccmos_version>=HAP_VERSION_NOISE)
  {
   int DoubleShutter_AppendNumber_Id;
   if(doublemode)
    Value=2;
   else
    Value=1;

   DoubleShutter_AppendNumber_Id = Fg_getParameterIdByName(fg,"Device1_Process0_DoubleShutter_AppendNumber");

   rcode = Fg_setParameterWithType(fg,DoubleShutter_AppendNumber_Id, &Value, 0, FG_PARAM_TYPE_UINT32_T);
   if(rcode)
    nError++;
   else
    writelog(INIT_M,hgrabber,"set_actual_size_global_shutter: Append Images set to %d ",Value);
  }
  else
   writelog(INIT_M,hgrabber,"set_actual_size_global_shutter: doublemode not possible with this version",Value);

  if(nError)
  {
   writelog(ERROR_M,hgrabber,"set_actual_size_global_shutter: Error because some FG_setParameter failed Errcount %d",nError);
   return PCO_ERROR_DRIVER_DATAERROR | PCO_ERROR_DRIVER_CAMERALINK;
  }

  writelog(INIT_M,hgrabber,"set_actual_size_global_shutter: done without error");
  return PCO_NOERROR;
}

