//-----------------------------------------------------------------//
// Name        | reorder_func.h              | Type: ( ) source    //
//-------------------------------------------|       (*) header    //
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
// Notes       | functions definition of reorder_func.cpp          //
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
//  see reorder_func.cpp                                           //
//-----------------------------------------------------------------//

//size of bufin must be the size of the full image buffer
//size of bufout must be the size of the full image buffer
//act_width is actual image width in pixel
//act_height is actual image height in lines
//DataFormat the actual DataFormat of camera and grabber
void reorder_image(void *bufout,void *bufin,int act_width,int act_height,int DataFormat);

//linenumber start from 1 to act_height
//size of bufin must be the size of the full image buffer
//size of bufout can be only the size of one line
//act_width is actual image width in pixel
//act_height is actual image height in lines
//DataFormat the actual DataFormat of camera and grabber
void get_image_line(void *bufout,void *bufin,int line_number,int act_width,int act_height,int DataFormat);


void sort_lines_A(int width,int height,void *adr_out,void* adr_in);
void sort_lines_B(int width,int height,void *adr_out,void* adr_in);
void sort_lines_C(int width,int height,void *adr_out,void* adr_in);
void sort_lines_D(int width,int height,void *adr_out,void* adr_in);
