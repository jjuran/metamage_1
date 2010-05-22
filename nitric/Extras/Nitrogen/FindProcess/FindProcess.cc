/*
	FindProcess.cc
	--------------
	
	Copyright 2009, Joshua Juran
*/

#include "FindProcess.hh"

// Nitrogen
#include "Nitrogen/Files.hh"
#include "Nitrogen/Processes.hh"


namespace NitrogenExtras
{
	
	namespace N = Nitrogen;
	
	
	class Process_HasSignature
	{
		private:
			Mac::OSType itsSignature;
		
		public:
			Process_HasSignature( Mac::OSType signature )
			:
				itsSignature( signature )
			{
			}
			
			bool operator()( const ProcessSerialNumber& psn ) const
			{
				ProcessInfoRec processInfo = N::GetProcessInformation( psn );
				
				return processInfo.processSignature == itsSignature;
			}
	};
	
	class Process_HasAppSpec
	{
		private:
			FSSpec itsAppSpec;
		
		public:
			Process_HasAppSpec( const FSSpec& appSpec )
			:
				itsAppSpec( appSpec )
			{
			}
			
			bool operator()( const ProcessSerialNumber& psn ) const
			{
				FSSpec appSpec = N::GetProcessAppSpec( psn );
				
				return N::FSCompareFSSpecs( appSpec, itsAppSpec );
			}
	};
	
	template < class Predicate >
	static inline ProcessSerialNumber FindProcess( Predicate predicate )
	{
		ProcessSerialNumber psn = N::NoProcess();
		
		while ( true )
		{
			psn = N::GetNextProcess( psn );
			
			if ( predicate( psn ) )
			{
				break;
			}
		}
		
		return psn;
	}
	
	ProcessSerialNumber FindProcess( Mac::OSType signature )
	{
		return FindProcess( Process_HasSignature( signature ) );
	}
	
	ProcessSerialNumber FindProcess( const FSSpec& appFile )
	{
		return FindProcess( Process_HasAppSpec( appFile ) );
	}
	
}

