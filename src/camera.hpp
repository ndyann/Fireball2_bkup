//
//  MINT
//	Wrapper for the PCO.Edge camera libraries
//
//  Adapted from the library's sample codes.
//
//  Created by Lucas Sousa de Oliveira on 07/11/12.
//  Property of NASA - Goddard Space Flight Center
//
#ifndef rubble_mint_camera
#define rubble_mint_camera

#include <stdint.h>
#include "VersionNo.h"
#include "pco_cl_include.h"
#include "Cpco_cl_com.h"
#include "Cpco_me4.h"
#include "file12.h"
#include <string>
#include "log.hpp"

#ifndef NAN
#define NAN		0x7FC00000 // = s111 1111 1axx xxxx xxxx xxxx xxxx xxxx ( according to http://en.wikipedia.org/wiki/NaN#Floating_point )
#endif

using namespace std;

// Prototypes
uint32_t    start_camera        (CPco_cl_com&,CPco_me4_edge&);
uint32_t    close_camera        (CPco_cl_com&,CPco_me4_edge&);
uint32_t    grab_single         (CPco_me4* grabber,uint16_t**);
int			img_nr_from_tmstamp	(void*,int);
uint32_t    log_camera_info     (CPco_cl_com&,CPco_me4_edge&);
uint32_t    get_camera_info		(CPco_cl_com&,CPco_me4_edge&,float[8]);

#endif
