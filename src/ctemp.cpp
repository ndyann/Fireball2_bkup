//
//  MINT
//	PCO.edge temperature reader
//
//  Created by Lucas Sousa de Oliveira on 08/27/12.
//  Property of NASA - Goddard Space Flight Center
//
#include "camera.hpp"
#include "log.hpp"

#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include <stdlib.h>
#include <signal.h>

void terminate (int);

CPco_cl_com camera;
CPco_me4_edge grabber;

int main(void){
    if (signal (SIGINT, terminate) == SIG_IGN)
        signal (SIGINT, SIG_IGN);
    
    start_camera    (camera,grabber); //Critical action
    
    for (int i=0; i<100; i++) {
        log_camera_info (camera,grabber);
        
        //Wait 5 minutes
        sleep(5);
    }
    
    close_camera(camera,grabber);
    return 0;
}

void terminate (int signum) {
    log(__FUNCTION__,OK,"Terminating program!");
    if (signum == SIGINT) {
        close_camera(camera,grabber);
        exit(signum);
    }
}