/*	============
 *	Processes.cp
 *	============
 */

#ifndef UTILITIES_PROCESSES_H
#include "Utilities/Processes.h"
#endif

// STL extensions
#ifdef __MWERKS__
#include <functional_ext>
#else
#include <ext/functional>
#endif

// Nitrogen
#ifndef NITROGEN_MACERRORS_H
#include "Nitrogen/MacErrors.h"
#endif
#ifndef NITROGEN_OSSTATUS_H
#include "Nitrogen/OSStatus.h"
#endif

// Nitrogen Extras / Utilities
#ifndef UTILITIES_FILES_H
#include "Utilities/Files.h"
#endif


namespace NitrogenExtras
{
	
	namespace N = Nitrogen;
	
#ifdef __MWERKS__
	using std::compose1;
#else
	using __gnu_cxx::compose1;
#endif
	
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
		                                    compose1( std::bind2nd( std::equal_to< N::OSType >(),
		                                                            signature ),
		                                              std::ptr_fun( GetProcessSignature ) ) );
		
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
		catch ( N::ProcNotFound )
		{
			return N::LaunchApplication( DTGetAPPL( signature ) );
		}
	}
	
}

