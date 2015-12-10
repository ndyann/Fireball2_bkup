#include "message_function.h"
#include "message_data.h"
//#include "mpf_message_format.h"
//#include "tmtc_message_format.h"
#include <sys/unistd.h>
#include <sys/un.h>
#include <cstdio>
#include <cstdlib>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/poll.h>
#include <cstring>

#include "defines.h"

#define DEBUGLINE // printf("%s: %d\n",__FUNCTION__, __LINE__); fflush(stdin);

extern pthread_mutex_t guide_data_lock;

// file descriptors for various ports
int fd_udp, fd_gps, fd_mot, fd_tmtc;

// structures for sockets
struct sockaddr_in myaddr;
struct sockaddr_in addr_mpf;
struct sockaddr_in addr_tmtc;
struct sockaddr_in addr_det;

// fd_set for polling
fd_set readfs;


// message periods in ms                                    
                                 
unsigned char mpf_lengths[MPF_MESSAGES]={1, 50, 21, 21, 13};                    
unsigned int mpf_period[MPF_MESSAGES]={1000000,250000,10000000,20000000,1000000};
unsigned int mpf_cycles[MPF_MESSAGES];
unsigned char mpf_send[MPF_MESSAGES]={1,0,0,1,1};

unsigned char tmtc_lengths[TMTC_MESSAGES]={0,0,0,1,0,1};

using namespace std;

int ComSetup(){
 struct sockaddr_in remaddr;                     /* remote address */
  socklen_t addrlen = sizeof(remaddr);            /* length of addresses */
  int recvlen;
  char server[255];
  int i;

  struct termios oldtioTMTC, newtioTMTC;/* for TMTC serial */


  // Set up UDP port. First we deal with the local stuff                                    

  /* create a UDP socket */
  if ((fd_udp = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("cannot create socket\n");
    exit(-1);
  }
  /* bind the socket to any valid IP address and a specific port */

  memset((char *)&myaddr, 0, sizeof(myaddr));
  myaddr.sin_family = AF_INET;
  myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  myaddr.sin_port = htons(SERVICE_PORT);

  if (bind(fd_udp, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) {
    perror("bind failed");
    exit(-1);
  }

  // Next, we set up the remote ports(mpf, det)                                             
  memset((char *) &addr_mpf, 0, sizeof(addr_mpf));
  addr_mpf.sin_family = AF_INET;
  addr_mpf.sin_port = htons(MPF_PORT);
  sprintf(server,"%s",MPF_IP);
  if (inet_pton(AF_INET, server, &addr_mpf.sin_addr)==0) {
    fprintf(stderr, "inet_aton() failed\n");
    exit(1);
  };

  // this is TMTC for now, not det.
  memset((char *) &addr_det, 0, sizeof(addr_det));
  addr_mpf.sin_family = AF_INET;
  addr_mpf.sin_port = htons(MPF_PORT);
  sprintf(server,"%s",TMTC_IP);
  if (inet_pton(AF_INET, server, &addr_det.sin_addr)==0) {
    fprintf(stderr, "inet_aton() failed\n");
    exit(1);
  };

  /*
  / * This is where the initialization for the serial and USB port               
     hardware would go * /
  fd_tmtc = open(TMTC_PORT, O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK);
  if(fd_tmtc < 0){
   printf("Couldn't Open TMTC Serial Device.\n");
    exit(-1);
  };
  // configure the TMTC port                                                                
  tcgetattr(fd_tmtc, &oldtioTMTC);
  newtioTMTC.c_cflag = TMTCBAUDRATE | CS8 | CLOCAL | CREAD;
  newtioTMTC.c_iflag = IGNPAR;
  newtioTMTC.c_oflag = 0;
  newtioTMTC.c_lflag = 0;
  newtioTMTC.c_cc[VMIN] = 0;
  newtioTMTC.c_cc[VTIME] = 0;
  newtioTMTC.c_cc[VTIME] = 0;
  tcflush(fd_tmtc, TCIFLUSH);
  tcsetattr(fd_tmtc, TCSANOW, &newtioTMTC);
  */
  // compute cyles
  printf("%d\n",MPF_MESSAGES);
  for(i=0;i<MPF_MESSAGES;i++) { 
    mpf_cycles[i] = mpf_period[i]/COM_THREAD_SLEEP;
    printf("%d %d %d %d\n",i,mpf_period[i], COM_THREAD_SLEEP, mpf_cycles[i]);
  };
  fflush(stdin);

  
  return 0;
}; //ComSetup();  

// stop the press
int ComStop(){
  return 0;
}; //ComStop(); 


int ComClose(){
  return 0;
};


/*----------------------------------------------------------------------------*/
/* CaclulateCheckSum                                                    */
/*----------------------------------------------------------------------------*/
/*! Effectue le calcul du checksum sur le buffer d'octets en entree. Cette 
 *  methode a ete directement recupere du protocole FivCo.
 *
 *\param       (input) unsigned char * ByteTab : Buffer de donnees.
 *\param       (input) int Size : Nombre d'octets dans le buffer.
 *\retval      Checksum calcule sur le tableau de donnees.
 *\warning Aucun.
 */
/*----------------------------------------------------------------------------*/
short MPF_CalculateCheckSum( unsigned char * ByteTab, int Size )
{
  // This function return the checksum calculated
  static unsigned int Sum;
  static bool AddHighByte;
  static unsigned int ChecksumCalculated;

  Sum=0;
  AddHighByte = true;
  ChecksumCalculated = 0;

  for(int i=0;i<Size;i++)
    {
      if(AddHighByte)
        {
	  Sum+=((ByteTab[i])<<8)^0xFF00;
	  AddHighByte=false;
        }
      else
        {
	  Sum+=(ByteTab[i])^0x00FF;
	  AddHighByte=true;
        }
    }
  if (AddHighByte==false)
    Sum+= 0xFF;

  ChecksumCalculated = ((Sum>>16)&0xFFFF)+(Sum&0xFFFF);
  ChecksumCalculated = ((ChecksumCalculated>>16)&0xFFFF)+(ChecksumCalculated&0xFFFF);
    
  return ( short ) ChecksumCalculated;   
};



void *message_function(void *message_function_data){
  unsigned char buf[UDPBUFSIZE];
  unsigned short counter, mpf_counter;
  int recvlen;
  struct sockaddr_in remaddr;                     /* remote address */
  socklen_t addrlen = sizeof(remaddr);            /* length of             
                                                     addresses */
  char message[255];
  unsigned char mID;
  short crc, crcb;
  message_data_t *mdt;
  unsigned char *cptr;
  short *sptr;
  unsigned short *usptr;

  unsigned short len;

  // populate some mpf message stuff
  mdt = (message_data_t *)message_function_data;

  ((*(*mdt).mpf_data).mpf000).id = 0;
  (*(*mdt).mpf_data).mpf001.id = 1;
  (*(*mdt).mpf_data).mpf002.id = 2;
  (*(*mdt).mpf_data).mpf003.id = 3;
  (*(*mdt).mpf_data).mpf004.id = 4;
  
  (*(*mdt).mpf_data).mpf000.length = mpf_lengths[0];
  (*(*mdt).mpf_data).mpf001.length = mpf_lengths[1];
  (*(*mdt).mpf_data).mpf002.length = mpf_lengths[2];
  (*(*mdt).mpf_data).mpf003.length = mpf_lengths[3];
  (*(*mdt).mpf_data).mpf004.length = mpf_lengths[4];
    
  DEBUGLINE;

  // we are going to have two (so far) streams to monitor: UDP, serial(TMTC)
  // there will be more eventually, so this will need to be updated.
  struct pollfd fds[1];
  fds[0].fd = fd_udp;
  fds[0].events = POLLRDNORM;

  //  fds[1].fd = fd_tmtc;
  //  fds[1].events = POLLRDNORM;

    DEBUGLINE;

  counter = 0;
  while(1){
    poll(fds,2,0);
    // is there data available on channel 0? UDP?                                           
    if (fds[0].revents & POLLRDNORM) {
      DEBUGLINE;
      // read the port!                                                                     
      recvlen = recvfrom(fd_udp, buf, UDPBUFSIZE, 0, (struct sockaddr*)&remaddr, &addrlen); 
      // is the message non-zero?                                                           
      if (recvlen >= MPF_MIN_TOTAL_LENGTH){ 
	// we know the information (at this time) can only come from the MPF
	// will need extra logic to include the DET in this.
	buf[recvlen] = 0;
	int buflen = recvlen;
	if (ntohl(remaddr.sin_addr.s_addr) == 0x7F000001){
	mID = buf[4]; // 
	if( mID >= 0 || mID <= MPF_MESSAGES){
	  if(recvlen == mpf_lengths[mID]+7){
	    crcb = MPF_CalculateCheckSum(buf,buflen-2);
	    crc = *( (short*) (buf+buflen-2));
	    if(crc == crcb){ 
	      switch(mID){
	      case 0:
		memcpy( (void*) &(*(*mdt).mpf_data).mpf000, (void*) buf, buflen);
		break;
	      case 1:
		memcpy( (void*) &(*(*mdt).mpf_data).mpf001, (void*) buf, buflen);
		break;
	      case 2:
		memcpy( (void*) &(*(*mdt).mpf_data).mpf002, (void*) buf, buflen);
		break;
	      case 3:
		memcpy( (void*) &(*(*mdt).mpf_data).mpf003, (void*) buf, buflen);
		break;
	      case 4:
		memcpy( (void*) &(*(*mdt).mpf_data).mpf004, (void*) buf, buflen);
		break;
	      default:
		break;
	      }; // switch
	    }; // crc is fine.
	  }; // length of message is FINE.
	}; // mID valid
	} else {
	  // received from TMTC (detector) instead

	}
      }; //recvlen >= MPF_MIN_TOTAL_LENGTH     
    }; // received data from udp
    DEBUGLINE
    // is there data on the serial?                                                         
    // at the moment, that only means TMTC
      //    if (fds[1].revents & POLLRDNORM) {
      // read the port
      // push the things onto the message stack
      // parse message stack for a valid message
      // act on valid message
      //    };
    DEBUGLINE
    usleep(COM_THREAD_SLEEP);
    DEBUGLINE

      static int i;
    for(i=0;i<MPF_MESSAGES;i++){
      DEBUGLINE

            DEBUGLINE
      if(counter % mpf_cycles[i] == i){
	    DEBUGLINE
	if(mpf_send[i]){ // is this a message we send?
	  len = mpf_lengths[i]+7;
	switch(i){
	case 0:
	  DEBUGLINE
	  cptr = (unsigned char *)&((*(*mdt).mpf_data).mpf000);
	  DEBUGLINE
	  break;
	case 1:
	  cptr = (unsigned char *)&((*(*mdt).mpf_data).mpf001);
	  break;
	case 2:
	  cptr = (unsigned char *)&((*(*mdt).mpf_data).mpf002);
	  break;
	case 3:
	  cptr = (unsigned char *)&((*(*mdt).mpf_data).mpf003);
	  break;
	case 4:
	  cptr = (unsigned char *)&((*(*mdt).mpf_data).mpf004);
	  break;
	default:
	  break;
	}; // switch
	DEBUGLINE
	// fill out the count;
	usptr = (unsigned short *) cptr;
	*usptr = mpf_counter++;
	DEBUGLINE
	usptr = (unsigned short *) cptr+2;	
	*usptr = len-7;
	cptr[4]=i;
	// CRC
	sptr = (short *) (cptr+len-2);
	*sptr = MPF_CalculateCheckSum(cptr,len-2);
	DEBUGLINE
	// Fire away!
	MPFWrite( (char*) cptr, len);
	DEBUGLINE
	}; // mpf_send[i]?
      }; // is it time to send the message? 
    };//i
    
    counter++;
    if (counter == 100000) counter = 0;
  };
};


int MPFWrite(char *buffer, int buflen){
  unsigned int slen = sizeof(addr_mpf);
  sendto(fd_udp, buffer, buflen,0, (struct sockaddr *)&addr_mpf, slen);
  return 0;
};
