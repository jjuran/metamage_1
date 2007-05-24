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

// Nitrogen Extras / Templates
#include "Templates/FunctionalExtensions.h"
#include "Templates/PointerToFunction.h"


namespace NitrogenExtras
{
	
	namespace N = Nitrogen;
	namespace ext = N::STLExtensions;
	
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
		                                    ext::compose1( std::bind2nd( std::equal_to< N::OSType >(),
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
		                                    ext::compose1( std::bind2nd( N::PtrFun( N::FSCompareFSSpecs ),
		                                                                 appFile ),
		                                                   N::PtrFun( N::GetProcessAppSpec ) ) );
		
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
			return N::LaunchApplication( N::DTGetAPPL( signature ) );
		}
	}
	
}

