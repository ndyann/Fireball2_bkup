//-----------------------------------------------------------------//
// Name        | reorder_func.cpp            | Type: (*) source    //
//-------------------------------------------|       ( ) header    //
// Project     | pco.camera                  |       ( ) others    //
//-----------------------------------------------------------------//
// Platform    | Linux                                             //
//-----------------------------------------------------------------//
// Environment |                                                   //
//             |                                                   //
//-----------------------------------------------------------------//
// Purpose     | pco.camera - edge                                 //
//-----------------------------------------------------------------//
// Author      | MBL, PCO AG                                       //
//-----------------------------------------------------------------//
// Revision    | rev. 1.03                                         //
//-----------------------------------------------------------------//
// Notes       | functions for reordering image data               //
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
// Free Software Foundation, Inc., 59 Temple Place - Suite 330,    //
// Boston, MA 02111-1307, USA                                      //
//-----------------------------------------------------------------//

//-----------------------------------------------------------------//
// Revision History:                                               //
//-----------------------------------------------------------------//
// Rev.:     | Date:      | Changed:                               //
// --------- | ---------- | ---------------------------------------//
//  1.01     | 08.01.2011 | ported from Windows SDK                //
//           |            |                                        //
// --------- | ---------- | ---------------------------------------//
//  1.02     | 14.01.2011 |                                        //
//           |            |                                        //
// --------- | ---------- | ---------------------------------------//
//  1.03     | 24.01.2012 | sort lines functions added             //
//           |            |                                        //
//           |            |                                        //
//-----------------------------------------------------------------//
//  0.0x     | xx.xx.2012 |                                        //
//           |            |                                        //
//-----------------------------------------------------------------//


#include "pco_cl_include.h"


void extract_image_me4(void *bufout,void *bufin,int act_width,int act_height)
{

  int x,y,off;
//  DWORD *picadr_in;
//  DWORD *picadr_out;
  DWORD *lineadr_in;
  DWORD *lineadr_out;
  DWORD a;

//  picadr_in=(DWORD *)bufadr;
//  picadr_out=(DWORD *)bufent->bufadr;
  off=(act_width*12)/32;
  lineadr_in=(DWORD *)bufin; //picadr_in;//+y*off;
  lineadr_out=(DWORD *)bufout;//picadr_out;//+y*entry->act_width;

  for(y=0;y<act_height;y++)
  {
   //          lineadr_in=picadr_in+y*off;
   //          lineadr_out=picadr_out+y*entry->act_width;
   for(x=0;x<off;)
   {
    a = (*lineadr_in&0x0000FFF0)>>4;
    a|= (*lineadr_in&0x0000000F)<<24;
    a|= (*lineadr_in&0xFF000000)>>8;
    *lineadr_out=a;
    lineadr_out++;

    a = (*lineadr_in&0x00FF0000)>>12;
    lineadr_in++;
    x++;
    a|= (*lineadr_in&0x0000F000)>>12;
    a|= (*lineadr_in&0x00000FFF)<<16;
    *lineadr_out=a;
    lineadr_out++;
    a = (*lineadr_in&0xFFF00000)>>20;
    a|= (*lineadr_in&0x000F0000)<<8;
    lineadr_in++;
    x++;
    a|= (*lineadr_in&0x0000FF00)<<8;
    *lineadr_out=a;
    lineadr_out++;
    a = (*lineadr_in&0x000000FF)<<4;
    a|= (*lineadr_in&0xF0000000)>>28;
    a|= (*lineadr_in&0x0FFF0000);
    *lineadr_out=a;
    lineadr_out++;
    lineadr_in++;
    x++;
   }
  }
}


void Extract_Line(int width,void *bufout,void* bufin)
{
  int x,off;
  DWORD *lineadr_in;
  DWORD *lineadr_out;
  DWORD a;

  off=(width*12)/32;
  lineadr_in=(DWORD *)bufin;
  lineadr_out=(DWORD *)bufout;

   for(x=0;x<off;)
   {
    a = (*lineadr_in&0x0000FFF0)>>4;
    a|= (*lineadr_in&0x0000000F)<<24;
    a|= (*lineadr_in&0xFF000000)>>8;
    *lineadr_out=a;
    lineadr_out++;

    a = (*lineadr_in&0x00FF0000)>>12;
    lineadr_in++;
    x++;
    a|= (*lineadr_in&0x0000F000)>>12;
    a|= (*lineadr_in&0x00000FFF)<<16;
    *lineadr_out=a;
    lineadr_out++;
    a = (*lineadr_in&0xFFF00000)>>20;
    a|= (*lineadr_in&0x000F0000)<<8;
    lineadr_in++;
    x++;
    a|= (*lineadr_in&0x0000FF00)<<8;
    *lineadr_out=a;
    lineadr_out++;
    a = (*lineadr_in&0x000000FF)<<4;
    a|= (*lineadr_in&0xF0000000)>>28;
    a|= (*lineadr_in&0x0FFF0000);
    *lineadr_out=a;
    lineadr_out++;
    lineadr_in++;
    x++;
   }
}


void Extract_A(int width,int height,void *adr_out,void* adr_in)
{
  unsigned short *line_1;
  unsigned short *line_n;
  unsigned short *line_in;
  int off=(width*12)/16;

  line_1=(unsigned short*)adr_out;
  line_n=(unsigned short*)adr_out;
  line_n+=(height-1)*width;
  line_in=(unsigned short*)adr_in;

  for(int y=0;y<height/2;y++)
  {
   Extract_Line(width,line_1,line_in);
   line_in+=off;
   Extract_Line(width,line_n,line_in);
   line_in+=off;

   line_1+=width;
   line_n-=width;
  }
}

void Extract_B(int width,int height,void *adr_out,void* adr_in)
{
  unsigned short *line_1;
  unsigned short *line_n;
  unsigned short *line_in;
  int off=(width*12)/16;

  line_1=(unsigned short*)adr_out;
  line_1+=(height/2-1)*width;
  line_n=(unsigned short*)adr_out;
  line_n+=(height/2)*width;
  line_in=(unsigned short*)adr_in;

  for(int y=0;y<height/2;y++)
  {
   Extract_Line(width,line_1,line_in);
   line_in+=off;
   Extract_Line(width,line_n,line_in);
   line_in+=off;

   line_1-=width;
   line_n+=width;
  }

}

void Extract_C(int width,int height,void *adr_out,void* adr_in)
{
  unsigned short *line_1;
  unsigned short *line_n;
  unsigned short *line_in;
  int off=(width*12)/16;


  line_1=(unsigned short*)adr_out;
  line_1+=(height/2-1)*width;
  line_n=(unsigned short*)adr_out;
  line_n+=(height-1)*width;
  line_in=(unsigned short*)adr_in;

  for(int y=0;y<height/2;y++)
  {
   Extract_Line(width,line_1,line_in);
   line_in+=off;
   Extract_Line(width,line_n,line_in);
   line_in+=off;

   line_1-=width;
   line_n-=width;
  }
}

void Extract_D(int width,int height,void *adr_out,void* adr_in)
{
  unsigned short *line_1;
  unsigned short *line_n;
  unsigned short *line_in;
  int off=(width*12)/16;

  line_1=(unsigned short*)adr_out;
  line_n=(unsigned short*)adr_out;
  line_n+=(height/2)*width;
  line_in=(unsigned short*)adr_in;

  for(int y=0;y<height/2;y++)
  {
   Extract_Line(width,line_1,line_in);
   line_in+=off;
   Extract_Line(width,line_n,line_in);
   line_in+=off;

   line_1+=width;
   line_n+=width;
  }
}

void Extract_S(int width,int height,void *adr_out,void* adr_in)
{
  unsigned short *line_out;
  unsigned short *line_in;
  int off=(width*12)/16;

  line_out=(unsigned short*)adr_out;
  line_in=(unsigned short*)adr_in;

  for(int y=0;y<height;y++)
  {
   Extract_Line(width,line_out,line_in);
   line_in+=off;
   line_out+=width;
  }
}



void sort_lines_A(int width,int height,void *adr_out,void* adr_in)
{
  unsigned short *line_1;
  unsigned short *line_n;
  unsigned short *line_in;

  line_1=(unsigned short*)adr_out;
  line_n=(unsigned short*)adr_out;
  line_n+=(height-1)*width;
  line_in=(unsigned short*)adr_in;

  for(int y=0;y<height/2;y++)
  {
   memcpy(line_1,line_in,width*sizeof(unsigned short));
   line_in+=width;
   memcpy(line_n,line_in,width*sizeof(unsigned short));
   line_in+=width;

   line_1+=width;
   line_n-=width;
  }
}

void sort_lines_B(int width,int height,void *adr_out,void* adr_in)
{
  unsigned short *line_1;
  unsigned short *line_n;
  unsigned short *line_in;


  line_1=(unsigned short*)adr_out;
  line_1+=(height/2-1)*width;
  line_n=(unsigned short*)adr_out;
  line_n+=(height/2)*width;
  line_in=(unsigned short*)adr_in;

  for(int y=0;y<height/2;y++)
  {
   memcpy(line_1,line_in,width*sizeof(unsigned short));
   line_in+=width;
   memcpy(line_n,line_in,width*sizeof(unsigned short));
   line_in+=width;

   line_1-=width;
   line_n+=width;
  }

}

void sort_lines_C(int width,int height,void *adr_out,void* adr_in)
{
  unsigned short *line_1;
  unsigned short *line_n;
  unsigned short *line_in;


  line_1=(unsigned short*)adr_out;
  line_1+=(height/2-1)*width;
  line_n=(unsigned short*)adr_out;
  line_n+=(height-1)*width;
  line_in=(unsigned short*)adr_in;

  for(int y=0;y<height/2;y++)
  {
   memcpy(line_1,line_in,width*sizeof(unsigned short));
   line_in+=width;
   memcpy(line_n,line_in,width*sizeof(unsigned short));
   line_in+=width;

   line_1-=width;
   line_n-=width;
  }
}

void sort_lines_D(int width,int height,void *adr_out,void* adr_in)
{
  unsigned short *line_1;
  unsigned short *line_n;
  unsigned short *line_in;

  line_1=(unsigned short*)adr_out;
  line_n=(unsigned short*)adr_out;
  line_n+=(height/2)*width;
  line_in=(unsigned short*)adr_in;

  for(int y=0;y<height/2;y++)
  {
   memcpy(line_1,line_in,width*sizeof(unsigned short));
   line_in+=width;
   memcpy(line_n,line_in,width*sizeof(unsigned short));
   line_in+=width;

   line_1+=width;
   line_n+=width;
  }
}


void extract_image(void *bufout,void *bufin,int act_width,int act_height,int DataFormat)
{
  if((DataFormat&PCO_CL_DATAFORMAT_MASK)!=PCO_CL_DATAFORMAT_5x12)
   return;

  if((DataFormat&SCCMOS_FORMAT_MASK)==SCCMOS_FORMAT_TOP_CENTER_BOTTOM_CENTER)
   Extract_A(act_width,act_height,bufout,bufin);
  else if((DataFormat&SCCMOS_FORMAT_MASK)==SCCMOS_FORMAT_CENTER_TOP_CENTER_BOTTOM)
   Extract_B(act_width,act_height,bufout,bufin);
  else if((DataFormat&SCCMOS_FORMAT_MASK)==SCCMOS_FORMAT_CENTER_TOP_BOTTOM_CENTER)
   Extract_C(act_width,act_height,bufout,bufin);
  else if((DataFormat&SCCMOS_FORMAT_MASK)==SCCMOS_FORMAT_TOP_CENTER_CENTER_BOTTOM)
   Extract_D(act_width,act_height,bufout,bufin);
}

void sort_image(void *bufout,void *bufin,int act_width,int act_height,int DataFormat)
{
  if((DataFormat&PCO_CL_DATAFORMAT_MASK)!=PCO_CL_DATAFORMAT_5x16)
   return;

  if((DataFormat&SCCMOS_FORMAT_MASK)==SCCMOS_FORMAT_TOP_CENTER_BOTTOM_CENTER)
   sort_lines_A(act_width,act_height,bufout,bufin);
  else if((DataFormat&SCCMOS_FORMAT_MASK)==SCCMOS_FORMAT_CENTER_TOP_CENTER_BOTTOM)
   sort_lines_B(act_width,act_height,bufout,bufin);
  else if((DataFormat&SCCMOS_FORMAT_MASK)==SCCMOS_FORMAT_CENTER_TOP_BOTTOM_CENTER)
   sort_lines_C(act_width,act_height,bufout,bufin);
  else if((DataFormat&SCCMOS_FORMAT_MASK)==SCCMOS_FORMAT_TOP_CENTER_CENTER_BOTTOM)
   sort_lines_D(act_width,act_height,bufout,bufin);
}

//size of bufin must be the size of the full image buffer
//size of bufout must be the size of the full image buffer
//act_width is actual image width in pixel
//act_height is actual image height in lines
//DataFormat the actual DataFormat of camera and grabber
void reorder_image(void *bufout,void *bufin,int act_width,int act_height,int DataFormat)
{
  if((DataFormat&PCO_CL_DATAFORMAT_MASK)==PCO_CL_DATAFORMAT_5x12)
  {
   if((DataFormat&SCCMOS_FORMAT_MASK)==SCCMOS_FORMAT_TOP_CENTER_BOTTOM_CENTER)
    Extract_A(act_width,act_height,bufout,bufin);
   else if((DataFormat&SCCMOS_FORMAT_MASK)==SCCMOS_FORMAT_CENTER_TOP_CENTER_BOTTOM)
    Extract_B(act_width,act_height,bufout,bufin);
   else if((DataFormat&SCCMOS_FORMAT_MASK)==SCCMOS_FORMAT_CENTER_TOP_BOTTOM_CENTER)
    Extract_C(act_width,act_height,bufout,bufin);
   else if((DataFormat&SCCMOS_FORMAT_MASK)==SCCMOS_FORMAT_TOP_CENTER_CENTER_BOTTOM)
    Extract_D(act_width,act_height,bufout,bufin);
   else if((DataFormat&SCCMOS_FORMAT_MASK)==SCCMOS_FORMAT_TOP_BOTTOM)
    Extract_S(act_width,act_height,bufout,bufin);
  }
  else if((DataFormat&PCO_CL_DATAFORMAT_MASK)==PCO_CL_DATAFORMAT_5x16)
  {
   if((DataFormat&SCCMOS_FORMAT_MASK)==SCCMOS_FORMAT_TOP_CENTER_BOTTOM_CENTER)
    sort_lines_A(act_width,act_height,bufout,bufin);
   else if((DataFormat&SCCMOS_FORMAT_MASK)==SCCMOS_FORMAT_CENTER_TOP_CENTER_BOTTOM)
    sort_lines_B(act_width,act_height,bufout,bufin);
   else if((DataFormat&SCCMOS_FORMAT_MASK)==SCCMOS_FORMAT_CENTER_TOP_BOTTOM_CENTER)
    sort_lines_C(act_width,act_height,bufout,bufin);
   else if((DataFormat&SCCMOS_FORMAT_MASK)==SCCMOS_FORMAT_TOP_CENTER_CENTER_BOTTOM)
    sort_lines_D(act_width,act_height,bufout,bufin);
   else if((DataFormat&SCCMOS_FORMAT_MASK)==SCCMOS_FORMAT_TOP_BOTTOM)
    memcpy(bufout,bufin,act_width*act_height*sizeof(unsigned short));
  }
  else if((DataFormat&PCO_CL_DATAFORMAT_MASK)==PCO_CL_DATAFORMAT_5x12R)
  {
   int w=(act_width*12)/16;
   if((DataFormat&SCCMOS_FORMAT_MASK)==SCCMOS_FORMAT_TOP_CENTER_BOTTOM_CENTER)
    Extract_A(w,act_height,bufout,bufin);
   else if((DataFormat&SCCMOS_FORMAT_MASK)==SCCMOS_FORMAT_CENTER_TOP_CENTER_BOTTOM)
    Extract_B(w,act_height,bufout,bufin);
   else if((DataFormat&SCCMOS_FORMAT_MASK)==SCCMOS_FORMAT_CENTER_TOP_BOTTOM_CENTER)
    Extract_C(w,act_height,bufout,bufin);
   else if((DataFormat&SCCMOS_FORMAT_MASK)==SCCMOS_FORMAT_TOP_CENTER_CENTER_BOTTOM)
    Extract_D(w,act_height,bufout,bufin);
   else if((DataFormat&SCCMOS_FORMAT_MASK)==SCCMOS_FORMAT_TOP_BOTTOM)
    Extract_S(w,act_height,bufout,bufin);
  }
  else if((DataFormat&PCO_CL_DATAFORMAT_MASK)==PCO_CL_DATAFORMAT_5x12L)
  {


  }

  return;
}

//linenumber start from 1 to act_height
//size of bufin must be the size of the full image buffer
//size of bufout can be only the size of one line
//act_width is actual image width in pixel
//act_height is actual image height in lines
//DataFormat the actual DataFormat of camera and grabber
void get_image_line(void *bufout,void *bufin,int line_number,int act_width,int act_height,int DataFormat)
{
  unsigned short *adr;
  int off;
  line_number-=1;
  if((DataFormat&PCO_CL_DATAFORMAT_MASK)==PCO_CL_DATAFORMAT_5x12)
  {
   off=(act_width*12)/16;
   adr=(unsigned short*)bufin;
   if((DataFormat&SCCMOS_FORMAT_MASK)==SCCMOS_FORMAT_TOP_CENTER_BOTTOM_CENTER)
   {
    if(line_number<act_height/2) //upper half of image
    {
     adr+=line_number*off*2;
    }
    else
    {
     adr+=off;
     adr+=(act_height-line_number-1)*off*2;
    }
    Extract_Line(act_width,bufout,adr);
   }
   else if((DataFormat&SCCMOS_FORMAT_MASK)==SCCMOS_FORMAT_CENTER_TOP_CENTER_BOTTOM)
   {
    if(line_number<act_height/2) //upper half of image
    {
     adr+=(act_height/2-line_number-1)*off*2;
    }
    else
    {
     adr+=off;
     adr+=line_number*off*2;
    }
    Extract_Line(act_width,bufout,adr);
   }
   else if((DataFormat&SCCMOS_FORMAT_MASK)==SCCMOS_FORMAT_CENTER_TOP_BOTTOM_CENTER)
   {
    if(line_number<act_height/2) //upper half of image
    {
     adr+=(act_height/2-line_number-1)*off*2;
    }
    else
    {
     adr+=off;
     adr+=(act_height-line_number-1)*off*2;
    }
    Extract_Line(act_width,bufout,adr);
   }
   else if((DataFormat&SCCMOS_FORMAT_MASK)==SCCMOS_FORMAT_TOP_CENTER_CENTER_BOTTOM)
   {
    if(line_number<act_height/2) //upper half of image
    {
     adr+=line_number*off*2;
    }
    else
    {
     adr+=off;
     adr+=line_number*off*2;
    }
    Extract_Line(act_width,bufout,adr);
   }
  }
  else if((DataFormat&PCO_CL_DATAFORMAT_MASK)==PCO_CL_DATAFORMAT_5x16)
  {
   off=act_width;
   adr=(unsigned short*)bufin;
   if((DataFormat&SCCMOS_FORMAT_MASK)==SCCMOS_FORMAT_TOP_CENTER_BOTTOM_CENTER)
   {
    if(line_number<act_height/2) //upper half of image
    {
     adr+=line_number*off*2;
    }
    else
    {
     adr+=off;
     adr+=(act_height-line_number-1)*off*2;
    }
    memcpy(bufout,adr,act_width*2);
   }
   else if((DataFormat&SCCMOS_FORMAT_MASK)==SCCMOS_FORMAT_CENTER_TOP_CENTER_BOTTOM)
   {
    if(line_number<act_height/2) //upper half of image
    {
     adr+=(act_height/2-line_number-1)*off*2;
    }
    else
    {
     adr+=off;
     adr+=line_number*off*2;
    }
    memcpy(bufout,adr,act_width*2);
   }
   else if((DataFormat&SCCMOS_FORMAT_MASK)==SCCMOS_FORMAT_CENTER_TOP_BOTTOM_CENTER)
   {
    if(line_number<act_height/2) //upper half of image
    {
     adr+=(act_height/2-line_number-1)*off*2;
    }
    else
    {
     adr+=off;
     adr+=(act_height-line_number-1)*off*2;
    }
    memcpy(bufout,adr,act_width*2);
   }
   else if((DataFormat&SCCMOS_FORMAT_MASK)==SCCMOS_FORMAT_TOP_CENTER_CENTER_BOTTOM)
   {
    if(line_number<act_height/2) //upper half of image
    {
     adr+=line_number*off*2;
    }
    else
    {
     adr+=off;
     adr+=line_number*off*2;
    }
    memcpy(bufout,adr,act_width*2);
   }
  }
}


