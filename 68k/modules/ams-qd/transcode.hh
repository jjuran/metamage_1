/*
	transcode.hh
	------------
*/

#ifndef TRANSCODE_HH
#define TRANSCODE_HH

typedef unsigned char Byte;
typedef unsigned long UInt32;

Byte* downsample_8_to_1( Byte*       dst : __A0,
                         const Byte* src : __A1, UInt32 n_src_bytes );

#endif
