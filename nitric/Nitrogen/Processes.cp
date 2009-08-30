// Processes.cp

#ifndef NITROGEN_PROCESSES_H
#include "Nitrogen/Processes.h"
#endif

#ifndef NITROGEN_ONLYONCE_H
#include "Nitrogen/OnlyOnce.h"
#endif
#ifndef NITROGEN_OSSTATUS_H
#include "Nitrogen/OSStatus.h"
#endif
#ifndef NITROGEN_FILES_H
#include "Nitrogen/Files.h"
#endif

namespace Nitrogen {
	
	void RegisterProcessManagerErrors()
	{
		RegisterOSStatus< paramErr      >();
		RegisterOSStatus< memFullErr    >();
		RegisterOSStatus< resNotFound   >();
		RegisterOSStatus< procNotFound  >();
		RegisterOSStatus< memFragErr    >();
		RegisterOSStatus< appModeErr    >();
		RegisterOSStatus< appMemFullErr >();
		RegisterOSStatus< appIsDaemon   >();
	}
	
	ProcessSerialNumber GetCurrentProcess()
	{
		OnlyOnce< RegisterProcessManagerErrors >();
		
		ProcessSerialNumber psn;
		ThrowOSStatus( ::GetCurrentProcess( &psn ) );
		return psn;
	}
	
	ProcessSerialNumber GetFrontProcess()
	{
		OnlyOnce< RegisterProcessManagerErrors >();
		
		ProcessSerialNumber psn;
		ThrowOSStatus( ::GetFrontProcess( &psn ) );
		return psn;
	}
	
	bool SameProcess( const ProcessSerialNumber& a, 
	                  const ProcessSerialNumber& b )
	{
		OnlyOnce< RegisterProcessManagerErrors >();
		
		::Boolean same;
		ThrowOSStatus( ::SameProcess( &a, &b, &same ) );
		return same;
	}
	
	void SetFrontProcess( const ProcessSerialNumber& psn )
	{
		OnlyOnce< RegisterProcessManagerErrors >();
		
		ThrowOSStatus( ::SetFrontProcess( &psn ) );
	}
	
	ProcessSerialNumber LaunchApplication( const FSSpec& file, LaunchFlags launchFlags, AppParameters* appParameters )
	{
		OnlyOnce< RegisterProcessManagerErrors >();
		
		LaunchParamBlockRec 	pb;
		
		pb.reserved1			= 0;
		pb.reserved2			= 0;
		pb.launchBlockID 		= extendedBlock;
		pb.launchEPBLength 		= extendedBlockLen;
		pb.launchFileFlags 		= 0;
		pb.launchControlFlags	= launchFlags | launchContinue | launchNoFileFlags;
		pb.launchAppSpec 		= const_cast< FSSpec* >( &file );
		pb.launchAppParameters	= appParameters;
		
		ThrowOSStatus( ::LaunchApplication( &pb ) );
		
		return pb.launchProcessSN;
	}
	
	ProcessSerialNumber GetNextProcess( ProcessSerialNumber process )
	{
		OnlyOnce< RegisterProcessManagerErrors >();
		
		ThrowOSStatus( ::GetNextProcess( &process ) );
		return process;
	}
	
	ProcessInfoRec& GetProcessInformation( const ProcessSerialNumber& process, ProcessInfoRec& processInfo )
	{
		OnlyOnce< RegisterProcessManagerErrors >();
		
		ThrowOSStatus( ::GetProcessInformation( &process, &processInfo ) );
		
		return processInfo;
	}
	
	ProcessInfoRec GetProcessInformation( const ProcessSerialNumber& process )
	{
		ProcessInfoRec processInfo;
		
		return GetProcessInformation( process, Initialize< ProcessInfoRec >( processInfo ) );
	}
	
	FSRef GetProcessBundleLocation( const ProcessSerialNumber& psn )
	{
	#if TARGET_API_MAC_CARBON
		FSRef location;
		ThrowOSStatus( ::GetProcessBundleLocation( &psn, &location ) );
		return location;
	#else
		return Convert< FSRef >( GetProcessAppSpec( psn ) );
	#endif
	}
	
	FSSpec GetProcessAppSpec( const ProcessSerialNumber& process )
	{
		ProcessInfoRec processInfo;
		FSSpec appSpec;
		
		// This may not be a good example of accessor use since we have the FSSpec sitting right there.
		return GetProcessInfoAppSpec
		(
			GetProcessInformation( process, 
		                           Initialize< ProcessInfoRec >( processInfo, 
		                                                         &appSpec ) )
		);
	}
	
}

