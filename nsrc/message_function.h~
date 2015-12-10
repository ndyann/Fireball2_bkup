#ifndef MESSAGE_FUNCTION_H
#define MESSAGE_FUNCTION_H

#define MPF_IP "127.0.0.1"
#define MPF_PORT 3025

#define TMTC_IP "127.0.0.1"


#define SERVICE_PORT 21234

#define UDPBUFSIZE 2048

//#define TMTC_PORT "/dev/ttyS0"
//#define TMTC_BAUDRATE B9600

void *message_function(void *message_function_data);
// establishes communication 
// compute periods
// set up loop.

short MPF_CaclulateCheckSum( unsigned char * ByteTab, int Size );
int MPFWrite(char *buffer, int buflen);

int ComSetup();
int ComStop();

#endif // MESSAGE_FUNCTION_H
