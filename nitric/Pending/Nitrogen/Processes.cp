// Processes.cp

#ifndef NITROGEN_ONLYONCE_H
#include "Nitrogen/OnlyOnce.h"
#endif
#ifndef NITROGEN_OSSTATUS_H
#include "Nitrogen/OSStatus.h"
#endif
#ifndef NITROGEN_PROCESSES_H
#include "Nitrogen/Processes.h"
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
	
	bool SameProcess(
		const ProcessSerialNumber& one, 
		const ProcessSerialNumber& other)
	{
		OnlyOnce< RegisterProcessManagerErrors >();
		
		::Boolean same;
		ThrowOSStatus( ::SameProcess( &one, &other, &same ) );
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
		pb.launchControlFlags	= launchContinue | launchNoFileFlags | launchFlags;
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
	
	void GetProcessInformation( const ProcessSerialNumber& process, ProcessInfoRec& info)
	{
		OnlyOnce< RegisterProcessManagerErrors >();
		
		ThrowOSStatus( ::GetProcessInformation( &process, &info ) );
	}
	
	ProcessInfoRec GetProcessInformation( const ProcessSerialNumber& process )
	{
		ProcessInfoRec processInfo;
		
		processInfo.processInfoLength = sizeof processInfo;
		processInfo.processName = NULL;
		processInfo.processAppSpec = NULL;
		
		GetProcessInformation( process, processInfo );
		
		return processInfo;
	}
	
	std::size_t SizeOf_AppParameters( const AppParameters& appParameters )
	{
		return sizeof (AppParameters) + appParameters.messageLength;
	}
	
}

