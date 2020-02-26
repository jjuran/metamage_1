/*
	macbinary.hh
	------------
*/

#ifndef MACBINARY_HH
#define MACBINARY_HH

// Standard
#include <stdint.h>


typedef uint8_t Str63[ 64 ];

typedef uint8_t uint16_x[ 2 ];
typedef uint8_t uint32_x[ 4 ];

typedef uint32_x OSType_x, Point_x;


namespace macbinary
{
	
#pragma options align=packed
	
	struct header
	{
		uint8_t   extensions;           // 1 for MacBinary+, 0 otherwise
		Str63     filename;
		OSType_x  fileType;
		OSType_x  fileCreator;
		uint8_t   fndrFlagsHigh;        // Finder flags, bits 8-15
		/*
			Bit 7 - isAlias
			Bit 6 - isInvisible
			Bit 5 - hasBundle
			Bit 4 - nameLocked
			Bit 3 - isStationery
			Bit 2 - hasCustomIcon
			Bit 1 - reserved
			Bit 0 - hasBeenInited
		*/
		uint8_t   zeroByte74;
		Point_x   iconPos;
		uint16_x  folderID;
		uint8_t   protectFlag;          // "Protected" flag (in low order bit)
		uint8_t   zeroByte82;
		uint32_x  dataForkLen;
		uint32_x  rsrcForkLen;
		uint32_x  crDate;
		uint32_x  mdDate;
		uint16_x  commentLen;
		uint8_t   fndrFlagsLow;         // Finder flags, bits 0-7
		/*
			Bit 7 - hasNoInits 
			Bit 6 - isShared 
			Bit 5 - requiresSwitchLaunch 
			Bit 4 - ColorReserved 
			Bits 1-3 - color 
			Bit 0 - isOnDesk
		*/
		uint32_t  formatSig;            // word-, but not long-aligned
		uint8_t   fdScript;
		uint8_t   fdXFlags;
		uint8_t   unused108[ 8 ];
		uint32_t  totalFilesSize;       // Total size of files (0 for one file)
		uint16_t  extHeaderLen;         // secondary header length
		uint8_t   srcFormatVers;
		uint8_t   minFormatVers;
		uint16_t  crc;
		uint16_t  reserved126;
	};
	
	typedef header hdr;
	
#pragma options align=reset
	
	int8_t version( const header& h );
	
}

#endif
