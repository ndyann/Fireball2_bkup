#include "fireball_camera2.h"
#include "image_analysis.h"
#include <cstdio>
#include <cstdlib>
#include <time.h>
#include <sys/unistd.h>


int main(){
  //Test program
  int n_images = 30;
  int width, height;
  uint16_t *pb;
  int i;
  //To save the last image in an array
  char fname[]="outfilez.arr";
  char fname2[]="outfilewindow.arr";
  FILE *fp;
  FILE *fp2;
  //To measure time
  timespec tstart,ts;
  float tim;
  char com[255];

  //Creates camera object, starts, configures it sets exposure time
  fireball_camera2 fcam;
  //Right now, we need first to start and then configure the camera
  //If we want to change the order, we need to modify the functions
  fcam.start();
  printf("Camera started.\n");
  fflush(stdout);
  //fcam.set_params();
  // Configure camera
  fcam.cam_config();
  printf("Camera configured:\n");
  fflush(stdout);
  //fcam.set_exposure(20);
  /*
  //As a test we will time the time to grab n images
  sprintf(com,"date");
  system(com);
  for(i=0;i<n_images;i++){
    if(i%50==0) printf("%d\n",i);
    fcam.grab_single();
  };
  system("date");
  tim = (ts.tv_sec-tstart.tv_sec);
  printf("%d grabs is: %f s\n",n_images,tim);
  */
  //Second loop at new exposure time
  fcam.set_exposure(20);
   sprintf(com,"date");
  system(com);
  for(i=0;i<n_images;i++){
    if(i%50==0) printf("%d\n",i);
    fcam.grab_single();
  };
  system("date");
  tim = (ts.tv_sec-tstart.tv_sec);
  printf("%d grabs is: %f s\n",n_images,tim);
  
  
  //Some screen printouts
  pb = fcam.camera_parameters.picbuf;
  width = fcam.camera_parameters.width;
  height = fcam.camera_parameters.height;

  //Testing the source extraction function
  printf("Testing source extraction\n");
  double tx[8],ty[8],flux[8];
  char valid[8];
  int nstars;
  acq_func(pb, width, height, tx, ty, flux, valid, &nstars);
  fflush(stdout);
  printf("Function executed\n");

  //Testing window copying
  printf("Testing window copying\n");
  int bhs;
  bhs = 50;
  int j;
  int k;
  //I need to initiate a fake boxes structure
  //I want to refer easily to a row in a box, that is
  // pPox[box][row], so I need to declare it as pBox[box][y=row][x=col]
  uint16_t ***pBoxes = new uint16_t**[8];  //boxes, bhs = 50
  for (i=0; i<8; i++){
    pBoxes[i] = new uint16_t*[101];  //rows, bhs = 50
    for(j=0; j<101; j++){
      pBoxes[i][j] = new uint16_t [101];  //cols, bhs = 50
      for (k=0; k<101; k++){
        pBoxes[i][j][k]=0;
      }
    }
  }

  //pBoxes[0][23][1] = 8;
  //printf("pBoxes[0][23][1] = %d, *pBoxes[0][23] = %d\n",pBoxes[0][23][1],*(pBoxes[0][23]+1));

  //Allocate memory for window buffer
  //box = (uint16_t*)malloc(bhs*bhs*sizeof(uint16_t));
  
  char mask[8] = {1, 1, 1, 1, 1, 1, 1, 1};
  track(pb,width, height, tx, ty, valid, pBoxes, &bhs, mask);
  fflush(stdout);
  printf("Window copying executed\n");
  printf("pBoxes[0][0][0] = %d\n", pBoxes[0][0][0]);
  // Now the window is in pBoxes, let's save it as an array
  // Size required t o save the image
  fp2 = fopen(fname2,"w");
  int wz = 101*sizeof(uint16_t);
  for(i=0; i<101; i++){
    fwrite(pBoxes[0][i],wz, 1, fp2);
  };
  fclose(fp2);
  // Free the allocated memory
  //free(box);
  for (i=0; i<8; i++){
    for (j=0; j<101; j++){
      delete [] pBoxes[i][j];
    }
    delete [] pBoxes[i];
  }
  delete [] pBoxes;
  //How to use std::max_element on a 2D array
  int matrix[2][3] ={{9,8,1},{3,4,5}};
  printf("Max element %d\n", *std::max_element(matrix[0],matrix[0]+3));
  printf("Max element %d\n", *std::max_element(matrix[1],matrix[0]+3));
  printf("Max element %d\n", *std::max_element(matrix[0],matrix[0]+2));
  //printf("Max element %d\n", *std::max_element(matrix[1],matrix[0]+2));
  const int N = sizeof(matrix[1])/sizeof(int);
  printf("N = %d\n",N);
  printf("Max element %d\n", std::distance(matrix[1],std::max_element(matrix[1],matrix[1]+N)));


  printf("Took an image of width %d, height %d\n",width,height);  
  fp = fopen(fname,"w");
  printf("opened file\n");
  //Saving the image in an array
  printf("The size of the buffer is: %d\n",sizeof(pb));
  //Size required to save the image
  int sz = height*width*sizeof(uint16_t);
  fwrite(pb,sz, 1, fp);
  fclose(fp);

  //Stops the camera
  fcam.stop();
  return 0;
};
