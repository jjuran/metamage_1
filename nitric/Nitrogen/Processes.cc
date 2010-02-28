// Nitrogen/Processes.cc
// ---------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2004-2006 by Joshua Juran and Marshall Clow.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#include "Nitrogen/Processes.hh"

// Nitrogen
#include "Nitrogen/Files.hh"
#include "Nitrogen/OSStatus.hh"


namespace Nitrogen
{
	
	// does nothing, but guarantees construction of theRegistration
	NUCLEUS_DEFINE_ERRORS_DEPENDENCY( ProcessManager )
	
	
	static void RegisterProcessManagerErrors();
	
	
#if NUCLEUS_RICH_ERRORCODES
#pragma force_active on
	
	class ProcessManagerErrorsRegistration
	{
		public:
			ProcessManagerErrorsRegistration()  { RegisterProcessManagerErrors(); }
	};
	
	static ProcessManagerErrorsRegistration theRegistration;
	
#pragma force_active reset
#endif
	
	
	ProcessSerialNumber GetCurrentProcess()
	{
		ProcessSerialNumber psn;
		ThrowOSStatus( ::GetCurrentProcess( &psn ) );
		
		return psn;
	}
	
	ProcessSerialNumber GetFrontProcess()
	{
		ProcessSerialNumber psn;
		ThrowOSStatus( ::GetFrontProcess( &psn ) );
		
		return psn;
	}
	
	bool SameProcess( const ProcessSerialNumber&  a, 
	                  const ProcessSerialNumber&  b )
	{
		::Boolean same;
		ThrowOSStatus( ::SameProcess( &a, &b, &same ) );
		
		return same;
	}
	
	void SetFrontProcess( const ProcessSerialNumber& psn )
	{
		ThrowOSStatus( ::SetFrontProcess( &psn ) );
	}
	
	ProcessSerialNumber LaunchApplication( const FSSpec&   file,
	                                       LaunchFlags     launchFlags,
	                                       AppParameters*  appParameters )
	{
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
		ThrowOSStatus( ::GetNextProcess( &process ) );
		
		return process;
	}
	
	ProcessInfoRec& GetProcessInformation( const ProcessSerialNumber&  process,
	                                       ProcessInfoRec&             processInfo )
	{
		ThrowOSStatus( ::GetProcessInformation( &process, &processInfo ) );
		
		return processInfo;
	}
	
	ProcessInfoRec GetProcessInformation( const ProcessSerialNumber& process )
	{
		ProcessInfoRec processInfo;
		
		return GetProcessInformation( process, Nucleus::Initialize< ProcessInfoRec >( processInfo ) );
	}
	
	FSRef GetProcessBundleLocation( const ProcessSerialNumber& psn )
	{
		FSRef location;
		ThrowOSStatus( ::GetProcessBundleLocation( &psn, &location ) );
		
		return location;
	}
	
	FSSpec GetProcessAppSpec( const ProcessSerialNumber& process )
	{
		ProcessInfoRec processInfo;
		FSSpec appSpec;
		
		// This may not be a good example of accessor use since we have the FSSpec sitting right there.
		return GetProcessInfoAppSpec
		(
			GetProcessInformation( process, 
		                           Nucleus::Initialize< ProcessInfoRec >( processInfo, 
		                                                                  &appSpec ) )
		);
	}
	
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
		
	#if TARGET_RT_MAC_MACHO
		
		RegisterOSStatus< -10661 >();
		
	#endif
	}
	
}

