/*	============
 *	Processes.cp
 *	============
 */

#ifndef UTILITIES_PROCESSES_H
#include "Utilities/Processes.h"
#endif

// Nitrogen
#include "Nitrogen/Files.h"
#ifndef NITROGEN_MACERRORS_H
#include "Nitrogen/MacErrors.h"
#endif


namespace NitrogenExtras
{
	
	namespace N = Nitrogen;
	
	
	class Process_HasSignature
	{
		private:
			N::OSType itsSignature;
		
		public:
			Process_HasSignature( N::OSType signature )
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
	
	ProcessSerialNumber FindProcess( N::OSType signature )
	{
		typedef N::Process_Container::const_iterator const_iterator;
		
		const_iterator proc = std::find_if( N::Processes().begin(),
		                                    N::Processes().end(),
		                                    Process_HasSignature( signature ) );
		
		if ( proc == N::Processes().end() )
		{
			throw N::ProcNotFound();
		}
		
		return *proc;
	}
	
	ProcessSerialNumber FindProcess( const FSSpec& appFile )
	{
		typedef N::Process_Container::const_iterator const_iterator;
		
		const_iterator proc = std::find_if( N::Processes().begin(),
		                                    N::Processes().end(),
		                                    Process_HasAppSpec( appFile ) );
		
		if ( proc == N::Processes().end() )
		{
			throw N::ProcNotFound();
		}
		
		return *proc;
	}
	
}

