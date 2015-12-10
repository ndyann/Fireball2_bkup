//
//  MINT
//	FITS Wrapper
//
//	Adapted from http://heasarc.gsfc.nasa.gov/docs/software/fitsio/cexamples.html
//	Link visited on 07/11/12.
//
//  Created by Lucas Sousa de Oliveira on 07/11/12.
//  Property of NASA - Goddard Space Flight Center
//

/*
//JZ. Include comments
#include <iostream>
//JZ. End added code
*/

#include "fitswrapper.hpp"

uint32_t    save_ptr2fits   (char* filename,uint32_t width,uint32_t height,uint16_t* buf){
    fitsfile* fptr;
    int status = 0;
    int naxis = 2;
    long naxes[2] = {width,height};
	char aux[1000],au2[1000];
    
    if (buf == NULL)
		return log(__FUNCTION__, ERROR, "No image on buffer!\n");
    
	log(__FUNCTION__, OK, "Creating file");
    fits_create_file(&fptr, filename, &status);
	if (status == 0){
		log(__FUNCTION__, OK, "File created");
	} else {
        fits_get_errstatus(status,aux);
		sprintf(au2, "Could not create file. Msg: %s", aux);
		return log(__FUNCTION__, ERROR, string(au2));
    }

	log(__FUNCTION__, OK, "Creating image structure");
	fits_create_img(fptr, SHORT_IMG, naxis, naxes, &status);
	if (status == 0){
		log(__FUNCTION__, OK, "Image created.");
	} else {
        fits_get_errstatus(status,aux);
		sprintf(au2, "Could not create image. Msg: %s", aux);
		return log(__FUNCTION__, ERROR, string(au2));
    }
    
	log(__FUNCTION__, OK, "Writing pixels to image structure");
	/*
	//JZ. Before writing the image, it must be in buf!!!
	using namespace std;
	cout << "The program is about to write the image in FITS" << endl;
	cout << "buf = " << buf << endl;
	cout << "*buf= " << buf[0] << "," << buf[1] << endl;
	cout << "Byte size of buf= " << sizeof(buf) << endl;
	cout << "Byte size of *buf= " << sizeof(*buf) << endl;
	//JZ. End of added code
	*/
	fits_write_img(fptr, TSHORT, 1, (naxes[0]-1)*(naxes[1]-1), buf, &status);
	if (status == 0){
		log(__FUNCTION__, OK, "Image pixels written.");
	} else {
        fits_get_errstatus(status,aux);
		sprintf(au2, "Could not write images pixels. Msg: %s", aux);
		return log(__FUNCTION__, ERROR, string(au2));
    }

	log(__FUNCTION__, OK, "Closing file");
	fits_close_file(fptr, &status);
	if (status == 0){
		log(__FUNCTION__, OK, "File closed.");
	} else {
        fits_get_errstatus(status,aux);
		sprintf(au2, "Could not close file. Msg: %s", aux);
		return log(__FUNCTION__, ERROR, string(au2));
    }
    
    return OK;
}
