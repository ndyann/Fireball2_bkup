#include "fireball_camera.h"
#include <cstdio>
#include <cstdlib>
#include <time.h>
#include <sys/unistd.h>

int main(){
  int width, height;
  uint16_t *pb;
  fireball_camera fcam;
  char fname[]="outfilex.arr";
  int i,j;
  FILE *fp;
  timespec tstart,ts;
  float tim;
  char com[255];
  
fcam.start();
  printf("Done starting.\n");
  fflush(stdout);
  fcam.set_params();

  sprintf(com,"date");
  system(com);//clock_gettime(CLOCK_REALTIME,&tstart);
  for(i=0;i<300; i++){
    if (i%50==0) printf("%d\n",i);
    fcam.grab_single();
  };
  system("date");//clock_gettime(CLOCK_REALTIME,&tstart);
  //  clock_gettime(CLOCK_REALTIME,&ts);
  tim = (ts.tv_sec-tstart.tv_sec);
  printf("300 grabs is: %f s\n",tim);



  pb = fcam.camera_parameters.picbuf;
  width = fcam.camera_parameters.width;
  height = fcam.camera_parameters.height;



  printf("Took an image of width %d, height %d\n",width,height);  
  fp = fopen(fname,"w");
  printf("opened file\n");

  printf("The size of the buffer is: %d\n",sizeof(pb));

  //  int sz = height*width;
  int sz = 11059200;
  //  for(i=0;i<height;i++){
  //   for(j=0;j<width;j++){
  //     fprintf(fp,"%d ",pb[j+i*width]);
  //  };
  //  fprintf(fp,"\n");
  // };  

  fwrite(pb,sz, 1, fp);

  //  for(i=0;i<sz; i++) fprintf(fp,"%d ",pb[i]);

  fclose(fp);
    fcam.stop();
  return 0;
};
