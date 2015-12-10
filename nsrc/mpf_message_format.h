#ifndef MPF_MESSAGE_FORMAT_H
#define MPF_MESSAGE_FORMAT_H

#define MPF_MIN_TOTAL_LENGTH 8
#define MPF_MIN_MESS_LENGTH 1
#define MPF_MESSAGES 5

#pragma pack (push,1)

typedef struct mpf_m000_t{
  unsigned short count;
  unsigned short length;
  unsigned char id;
  unsigned char status;
  short crc;
} mpf_m000_t;

// not currently implemented
typedef struct mpf_m001_t{
  unsigned short count;
  unsigned short length;
  unsigned char id;
  unsigned char byt[50];
  short crc;
} mpf_m001_t;

typedef struct mpf_m002_t{
  unsigned short count;
  unsigned short length;
  unsigned char id;
  double timestamp;
  float azimuth;
  float elevation;
  float rotation;
  unsigned char valid;
  short crc;
} mpf_m002_t;

typedef struct mpf_m003_t{
  unsigned short count;
  unsigned short length;
  unsigned char id;
  double timestamp;
  float alpha;
  float delta;
  float rotation;
  unsigned char valid;
  short crc;
} mpf_m003_t;

typedef struct mpf_m004_t{
  unsigned short count;
  unsigned short length;
  unsigned char id;
  double timestamp;
  float roterror;
  unsigned char valid;
  short crc;
} mpf_m004_t;

#pragma pack (pop)

#endif

