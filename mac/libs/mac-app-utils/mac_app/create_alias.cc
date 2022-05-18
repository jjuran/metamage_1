/*
	create_alias.hh
	---------------
*/

#include "mac_app/create_alias.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __ALIASES__
#include <Aliases.h>
#endif
#ifndef __FILES__
#include <Files.h>
#endif
#ifndef __RESOURCES__
#include <Resources.h>
#endif
#ifndef __SCRIPT__
#include <Script.h>
#endif

// mac-sys-utils
#include "mac_sys/res_error.hh"


namespace mac {
namespace app {
	
#if __LP64__
	
	int dummy;
	
#else
	
	short create_alias( const FSSpec& alias_file, const FSSpec& target )
	{
		OSErr err;
		CInfoPBRec pb;
		
		pb.hFileInfo.ioNamePtr = (StringPtr) target.name;
		pb.hFileInfo.ioVRefNum = target.vRefNum;
		pb.hFileInfo.ioDirID   = target.parID;
		pb.hFileInfo.ioFDirIndex = 0;
		
		if (( err = PBGetCatInfoSync( &pb ) ))
		{
			return err;
		}
		
		OSType creator = 'MACS';
		OSType type    = kContainerFolderAliasType;
		
		if ( ! (pb.hFileInfo.ioFlAttrib & kioFlAttribDirMask) )
		{
			creator = pb.hFileInfo.ioFlFndrInfo.fdCreator;
			type    = pb.hFileInfo.ioFlFndrInfo.fdType;
		}
		
		FSpCreateResFile( &alias_file, creator, type, smSystemScript );
		
		if (( err = mac::sys::res_error() ))
		{
			return err;
		}
		
		ResFileRefNum res = FSpOpenResFile( &alias_file, fsRdWrPerm );
		
		if ( res == -1 )
		{
			return mac::sys::res_error();
		}
		
		AliasHandle h;
		
		if (( err = NewAlias( &alias_file, &target, &h ) ))
		{
			goto cleanup;
		}
		
		AddResource( (Handle) h, rAliasType, 0, "\p" );
		
		if (( err = mac::sys::res_error() ))
		{
			DisposeHandle( (Handle) h );
			
			goto cleanup;
		}
		
		if (( err = FSpGetFInfo( &alias_file, &pb.hFileInfo.ioFlFndrInfo ) ))
		{
			goto cleanup;
		}
		
		pb.hFileInfo.ioFlFndrInfo.fdFlags |= kIsAlias;
		
		if (( err = FSpSetFInfo( &alias_file, &pb.hFileInfo.ioFlFndrInfo ) ))
		{
			goto cleanup;
		}
		
	cleanup:
		CloseResFile( res );
		
		return err;
	}
	
#endif  // #if __LP64__
	
}
}
