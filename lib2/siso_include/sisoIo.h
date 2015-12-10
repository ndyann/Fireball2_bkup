//+-------------------------------------------------------------------
//
//  MicroEnable Io Library
//  Copyright (C) Silicon Software GmbH, 1997-2010
//
// This includes code from:
// LIBTIFF, Version 3.7.4
//    Copyright (c) 1988-1996 Sam Leffler
//    Copyright (c) 1991-1996 Silicon Graphics, Inc.
//
//--------------------------------------------------------------------

#ifndef _SISOIO_H
#define _SISOIO_H

#include "os_type.h"

#ifdef _WIN32
#include <stdio.h>
#include <vfw.h>
#else
typedef char* LPCWSTR;
#endif

#define	    COMPRESSION_NONE		1	// dump mode 
#define	    COMPRESSION_CCITTRLE	2	// CCITT modified Huffman RLE 
#define	    COMPRESSION_CCITTFAX3	3	// CCITT Group 3 fax encoding 
#define     COMPRESSION_CCITT_T4    3       // CCITT T.4 (TIFF 6 name) 
#define	    COMPRESSION_CCITTFAX4	4	// CCITT Group 4 fax encoding 
#define     COMPRESSION_CCITT_T6    4       // CCITT T.6 (TIFF 6 name) 
#define	    COMPRESSION_LZW			5       // Lempel-Ziv  & Welch 
#define	    COMPRESSION_OJPEG		6	// !6.0 JPEG 
#define	    COMPRESSION_JPEG		7	// %JPEG DCT compression 
#define	    COMPRESSION_NEXT		32766	// NeXT 2-bit RLE 
#define	    COMPRESSION_CCITTRLEW	32771	// #1 w/ word alignment 
#define	    COMPRESSION_PACKBITS	32773	// Macintosh RLE 
#define	    COMPRESSION_THUNDERSCAN	32809	// ThunderScan RLE 


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef IMP_DLL
#define dlldir __declspec( dllimport)
#else
#ifndef dlldir
#define dlldir
#endif
#endif

dlldir int IoCreateAVIGray(void **AviRef,char *filename,int width,int height,double fps);
dlldir int IoCreateAVIGrayW(void **AviRef,LPCWSTR filename,int width,int height,double fps);
dlldir int IoCreateAVIColor(void **AviRef,char *filename,int width,int height,double fps);
dlldir int IoCreateAVIColorW(void **AviRef,LPCWSTR filename,int width,int height,double fps);
dlldir int IoWriteAVIPicture(void *AviRef,int PicNr,unsigned char *buffer);
dlldir int IoReadAVIPicture(void *AviRef,int PicNr,unsigned char *buffer);
dlldir int IoCloseAVI(void *AviRef);
dlldir int IoOpenAVI(void **AviRef,char *fileName,int* width,int* height,int *bitDepth);

dlldir void *IoReadBmp(const char *name,unsigned char **data,int *width,int *height,int *bits);
dlldir int IoCloseBmp(void *bmpHandle);
dlldir int IoWriteBmp(char *name,unsigned char *data,int width,int height,int bits);
dlldir int IoWriteBmpW(LPCWSTR filename,unsigned char *data,int width,int height,int bits);

dlldir int IoWriteRaw(char *name,unsigned char *data,int width,int height,int bits);
dlldir int IoWriteRawW(LPCWSTR filename,unsigned char *data,int width,int height,int bits);
dlldir int IoReadRaw(char *name,unsigned char**data,int *width,int *height,int *bits);
dlldir int IoFreeRaw(unsigned char *data);

// create a image sequence file
dlldir int IoCreateSeq(void **SeqRef,char *pFilename,int width,int height,int bitdepth,int format);
// put the given image at the given sequence index
dlldir int IoWriteSeqPicture(void *SeqRef,int PicNr,unsigned char *buffer);
// close image sequence file
dlldir int IoCloseSeq(void *SeqRef);

// Open file with image sequence
dlldir int IoOpenSeq(void **SeqRef,char *pFilename,int* width,int* height,int* bitdepth,int mode);
// get information about image sequence file
dlldir int IoGetSeqInfo(void *SeqRef,int *nrOfImages,int *nrOfLostImages);
// load image with given sequence number
dlldir int IoReadSeqPicture(void *SeqRef,int PicNr,unsigned char *buffer);
// load next picture in sequence
dlldir int IoReadNextSeqPicture(void *SeqRef,int* PicNr,unsigned char *buffer);
// create an AVI file from the given range of images
dlldir int IoSeqCreateAvi(void *SeqRef,char* aviFilename,int StartPicNr,int StopPicNr);


dlldir int IoWriteTiff(char *name,unsigned char *data,int width,int height,int bitPerSample,int samplePerPixel);
dlldir int IoWriteTiffW(LPCWSTR filename,unsigned char *data,int width,int height,int bitPerSample,int samplePerPixel);
dlldir void *IoReadTiff(char *name,unsigned char**data,int *width,int *height,int *bitPerSample,int *samplePerPixel);
dlldir void *IoReadTiffW(LPCWSTR filename,unsigned char**data,int *width,int *height,int *bitPerSample,int *samplePerPixel);
dlldir int IoFreeTiff(unsigned char *data);
dlldir int IoWriteTiffCompression(char *name,unsigned char *data,int width,int height,int bitPerSample,int samplePerPixel, int compressionFlag);

// -------------------------------------------------------------------
dlldir void*	IoOpenImage					(char *name);
dlldir void*	IoOpenImageExt				(char *name,unsigned char **data,int *width,int *height,int *bits);
dlldir int	IoFreeImage					(void *handle);
dlldir int	IoGetWidth					(void *handle);
dlldir int	IoGetHeight					(void *handle);
dlldir int	IoGetBitsPerPixel			(void *handle);
dlldir int	IoGetBitsPerComponent		(void *handle) ;
dlldir int	IoGetNrOfComponents			(void *handle);
dlldir int	IoGetImageData				(void *handle);
dlldir int	IoSaveImageExt				(char *name,unsigned char *data,int width,int height,int bits);

// -------------------------------------------------------------------

dlldir void*	AIoOpenImage				(const char *name,int bits,int comp);
dlldir int	AIoFreeImage				(void *handle);
dlldir int	AIoGetWidth					(void *handle);
dlldir int	AIoGetHeight				(void *handle);
dlldir int	AIoGetBitsPerPixel			(void *handle);
dlldir int	AIoGetBitsPerComponent		(void *handle) ;
dlldir int	AIoGetNrOfComponents		(void *handle);
dlldir unsigned char*	AIoGetImageData				(void *handle);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _SISOIO_H */
