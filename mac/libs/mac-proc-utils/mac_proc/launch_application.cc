/*
	launch_application.cc
	---------------------
*/

#include "mac_proc/launch_application.hh"

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef __PROCESSES__
#include <Processes.h>
#endif


namespace mac  {
namespace proc {

#ifdef __LP64__

OSErr launch_application( ProcessSerialNumber*  psn,
                          const FSRef&          file,
                          AppParameters*        params )
{
	LaunchParamBlockRec pb;
	
	pb.reserved1			= 0;
	pb.reserved2			= 0;
	pb.launchBlockID 		= extendedBlock;
	pb.launchEPBLength 		= extendedBlockLen;
	pb.launchFileFlags 		= 0;
	pb.launchControlFlags	= launchContinue | launchNoFileFlags;
	pb.launchAppRef  		= const_cast< FSRef* >( &file );
	pb.launchAppParameters	= params;
	
	OSErr err = LaunchApplication( &pb );
	
	if ( psn )
	{
		*psn = pb.launchProcessSN;
	}
	
	return err;
}

#else

OSErr launch_application( ProcessSerialNumber*  psn,
                          const FSSpec&         file,
                          AppParameters*        params )
{
	LaunchParamBlockRec pb;
	
	pb.reserved1			= 0;
	pb.reserved2			= 0;
	pb.launchBlockID 		= extendedBlock;
	pb.launchEPBLength 		= extendedBlockLen;
	pb.launchFileFlags 		= 0;
	pb.launchControlFlags	= launchContinue | launchNoFileFlags;
	pb.launchAppSpec 		= const_cast< FSSpec* >( &file );
	pb.launchAppParameters	= params;
	
	OSErr err = LaunchApplication( &pb );
	
	if ( psn )
	{
		*psn = pb.launchProcessSN;
	}
	
	return err;
}

#endif

}
}
