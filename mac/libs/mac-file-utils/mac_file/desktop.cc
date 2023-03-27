/*
	desktop.cc
	----------
*/

#include "mac_file/desktop.hh"

// Standard C
#include <string.h>

// mac-sys-utils
#include "mac_sys/unit_table.hh"


#pragma exceptions off


#define PSTR_LEN( s )  "\p" s, sizeof s


namespace mac  {
namespace file {

#if ! __LP64__

OSErr get_desktop_APPL( FSSpec&  result,
                        short    vRefNum,
                        OSType   signature,
                        short    index )
{
	OSErr err;
	DTPBRec pb;
	
	pb.ioVRefNum = vRefNum;
	pb.ioNamePtr = NULL;
	
	err = PBDTGetPath( &pb );
	
	if ( err == noErr )
	{
		pb.ioNamePtr     = result.name;
		pb.ioIndex       = index;
		pb.ioFileCreator = signature;
		
		err = PBDTGetAPPLSync( &pb );
		
		if ( err == noErr )
		{
			result.vRefNum = vRefNum;
			result.parID   = pb.ioAPPLParID;
		}
	}
	
	return err;
}

OSErr get_desktop_APPL( FSSpec&  result,
                        OSType   signature )
{
	HParamBlockRec pb;
	
	pb.volumeParam.ioNamePtr = NULL;
	
	OSErr err;
	short index = 0;
	
	do
	{
		pb.volumeParam.ioVRefNum  = 0;
		pb.volumeParam.ioVolIndex = ++index;
		
		err = PBHGetVInfoSync( &pb );
		
		if ( err == noErr )
		{
			short vRefNum = pb.volumeParam.ioVRefNum;
			
			err = get_desktop_APPL( result, vRefNum, signature, 0 );
		}
		
		if ( err == noErr )
		{
			FInfo info;
			err = HGetFInfo( result.vRefNum, result.parID, result.name, &info );
			
			if ( err == noErr )
			{
				return err;  // noErr
			}
		}
	}
	while ( err != nsvErr );
	
	return afpItemNotFound;
}

static inline
bool is_RAM_disk_driver_name( ConstStr255Param name )
{
	return memcmp( name, PSTR_LEN( ".EDisk" ) ) == 0;
}

static inline
bool is_RAM_disk_driver( mac::types::AuxDCE** dceHandle )
{
	return is_RAM_disk_driver_name( mac::sys::get_driver_name( dceHandle ) );
}

static inline
bool is_RAM_disk_driver_refnum( short dRefNum )
{
	mac::types::AuxDCE*** const unit_table = mac::sys::get_unit_table_base();
	
	return dRefNum != 0  &&  is_RAM_disk_driver( unit_table[ ~dRefNum ] );
}

OSErr get_desktop_APPL_on_RAM_disk( FSSpec&  result,
                                    OSType   signature )
{
	if ( TARGET_API_MAC_CARBON )
	{
		return afpItemNotFound;
	}
	
	HParamBlockRec pb;
	
	pb.volumeParam.ioNamePtr = NULL;
	
	OSErr err;
	short index = 0;
	
	do
	{
		pb.volumeParam.ioVRefNum  = 0;
		pb.volumeParam.ioVolIndex = ++index;
		
		err = PBHGetVInfoSync( &pb );
		
		if ( err == noErr )
		{
			short dRefNum = pb.volumeParam.ioVDRefNum;
			short vRefNum = pb.volumeParam.ioVRefNum;
			
			if ( ! is_RAM_disk_driver_refnum( dRefNum ) )
			{
				continue;
			}
			
			err = get_desktop_APPL( result, vRefNum, signature, 0 );
		}
		
		if ( err == noErr )
		{
			FInfo info;
			err = HGetFInfo( result.vRefNum, result.parID, result.name, &info );
			
			if ( err == noErr )
			{
				return err;  // noErr
			}
		}
	}
	while ( err != nsvErr );
	
	return afpItemNotFound;
}

long get_desktop_comment( short            vRefNum,
                          long             dirID,
                          ConstStr63Param  name,
                          char*            buffer,
                          UInt32           length )
{
	OSErr err;
	DTPBRec pb;
	
	pb.ioVRefNum = vRefNum;
	pb.ioNamePtr = NULL;
	
	err = PBDTGetPath( &pb );
	
	if ( err == noErr )
	{
		pb.ioNamePtr = const_cast< unsigned char* >( name );
		pb.ioDirID   = dirID;
		
		pb.ioDTBuffer   = buffer;
		pb.ioDTReqCount = length;
		
		err = PBDTGetCommentSync( &pb );
	}
	
	return err ? err : pb.ioDTActCount;
}

OSErr set_desktop_comment( short            vRefNum,
                           long             dirID,
                           ConstStr63Param  name,
                           const char*      buffer,
                           UInt32           length )
{
	OSErr err;
	DTPBRec pb;
	
	pb.ioVRefNum = vRefNum;
	pb.ioNamePtr = NULL;
	
	err = PBDTGetPath( &pb );
	
	if ( err == noErr )
	{
		pb.ioNamePtr = const_cast< unsigned char* >( name );
		pb.ioDirID   = dirID;
		
		pb.ioDTBuffer   = const_cast< char* >( buffer );
		pb.ioDTReqCount = length;
		
		err = PBDTSetCommentSync( &pb );
	}
	
	return err;
}

#endif  // #if ! __LP64__

}
}
