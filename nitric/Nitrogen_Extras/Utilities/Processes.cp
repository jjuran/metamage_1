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
#ifndef NITROGEN_OSSTATUS_H
#include "Nitrogen/OSStatus.h"
#endif

// MoreFunctional
#include "FunctionalExtensions.hh"
#include "PointerToFunction.hh"


namespace NitrogenExtras
{
	
	namespace N = Nitrogen;
	
	static N::OSType GetProcessInfoSignature( const ProcessInfoRec& processInfo )
	{
		return N::OSType( processInfo.processSignature );
	}
	
	static N::OSType GetProcessSignature( const ProcessSerialNumber process )
	{
		return GetProcessInfoSignature( N::GetProcessInformation( process ) );
	}
	
	ProcessSerialNumber FindProcess( N::OSType signature )
	{
		typedef N::Process_Container::const_iterator const_iterator;
		
		const_iterator proc = std::find_if( N::Processes().begin(),
		                                    N::Processes().end(),
		                                    more::compose1( std::bind2nd( std::equal_to< N::OSType >(),
		                                                                  signature ),
		                                                    std::ptr_fun( GetProcessSignature ) ) );
		
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
		                                    more::compose1( std::bind2nd( more::ptr_fun( N::FSCompareFSSpecs ),
		                                                                  appFile ),
		                                                    more::ptr_fun( N::GetProcessAppSpec ) ) );
		
		if ( proc == N::Processes().end() )
		{
			throw N::ProcNotFound();
		}
		
		return *proc;
	}
	
	ProcessSerialNumber LaunchApplication( N::OSType signature )
	{
		try
		{
			return FindProcess( signature );
		}
		catch ( const N::OSStatus& err )
		{
			if ( err.Get() != procNotFound )
			{
				throw;
			}
			
			return N::LaunchApplication( N::DTGetAPPL( signature ) );
		}
	}
	
}

