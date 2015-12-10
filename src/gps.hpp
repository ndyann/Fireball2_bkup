//
//  MINT
//	GPS reading functions
//
//  Created by Lucas Sousa de Oliveira on 8/13/12.
//  Property of NASA - Goddard Space Flight Center
//
#ifndef rubble_mint_gps
#define rubble_mint_gps

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <stdint.h>   /* Standard types */
#include <string.h>   /* String function definitions */
#include <unistd.h>   /* UNIX standard function definitions */
#include <fcntl.h>    /* File control definitions */
#include <errno.h>    /* Error number definitions */
#include <termios.h>  /* POSIX terminal control definitions */
#include <sys/ioctl.h> // JZ. c lib for io control in linux
#include <getopt.h>    // JZ. c lib to parse command line options
#include <stropts.h>    // JZ. c lib for compiling

#include "log.hpp"
#include "parameters.hpp"

void		get_time(char *str,struct tm* t);
void		get_pos(char *str,float& lat,float& log);

/* JZ. Commented out to avoid GPS_PORT and GPS_BYTES compilation errors

uint32_t	get_string(int fd,char str[30]);
uint32_t	get_usb_ro(int& fd,int baud);
uint32_t	close_usb(int fd);

*/ // JZ. Commented out to avoid GPS_PORT and GPS_BYTES compilation errors

#endif
