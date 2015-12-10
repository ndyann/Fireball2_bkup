//
//  MINT
//	Logging functions
//
//  Created by Lucas Sousa de Oliveira on 07/18/12.
//  Property of NASA - Goddard Space Flight Center
//
#ifndef rubble_mint_log
#define rubble_mint_log

#include "parameters.hpp"

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string>
using namespace std;

#define OK			0x00000000
#define NOTHING		0xFFFFFFFF
#define DISABLED	0xFFFF0000

#define FATAL		0xF0000000
#define ERROR		0x40000000
#define WARNING 	0x20000000
#define INFO        0x10000000

// JZ. I change the paths for the logs
#define HD1_PATH "/home/salo/FB/Fireball2/mint_camera_only/log1"
#define HD2_PATH "/home/salo/FB/Fireball2/mint_camera_only/log2"
#define USB_PATH "/home/salo/FB/Fireball2/mint_camera_only/usb"

enum devices {HD1=0,HD2,USB};

int 	open_log(devices, string);
int 	log(string, unsigned int, string);
int		close_logs(void);

#endif
