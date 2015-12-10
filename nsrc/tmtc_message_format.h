#ifndef TMTC_MESSAGE_FORMAT_H_
#define TMTC_MESSAGE_FORMAT_H_


#define TMTC_MESSAGES 6
#pragma pack(push,1)

// 000 is EXIT;
typedef struct tmtc_m000_t{
  unsigned short count;
  unsigned short length;
  unsigned char id;
  // message goes here
  short crc;
} tmtc_m000_t;

// 001 is Acquisition mode;
typedef struct tmtc_m001_t{
  unsigned short count;
  unsigned short length;
  unsigned char id;
  short crc;
} tmtc_m001_t;

// 002 is Trap bright star
typedef struct tmtc_m002_t{
  unsigned short count;
  unsigned short length;
  unsigned char id;
  short crc;
} tmtc_m002_t;

// 003 is guide on N stars
typedef struct tmtc_m003_t{
  unsigned short count;
  unsigned short length;
  unsigned char id;
  unsigned char nstars;
  short crc;
} tmtc_m003_t;

// 004 is switch display to full device
typedef struct tmtc_m004_t{
  unsigned short count;
  unsigned short length;
  unsigned char id;
  short crc;
} tmtc_m004_t;

// 005 is switch display to n boxes
typedef struct tmtc_m005_t{
  unsigned short count;
  unsigned short length;
  unsigned char id;
  unsigned char nboxes;
  short crc;
} tmtc_m005_t;


#pragma pack(pop)

#endif //TMTC_MESSAGE_FORMAT_H_
