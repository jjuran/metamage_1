// Nitrogen/Processes.hh
// ---------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2004-2009 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef NITROGEN_PROCESSES_HH
#define NITROGEN_PROCESSES_HH

#ifndef __PROCESSES__
#include <Processes.h>
#endif

// nucleus
#include "nucleus/advance_until_error_sequence.hh"
#include "nucleus/enumeration_traits.hh"
#include "nucleus/flag_ops.hh"

#ifndef NITROGEN_CFSTRING_HH
//#include "Nitrogen/CFString.hh"
#endif
#ifndef NITROGEN_EVENTS_HH
//#include "Nitrogen/Events.hh"
#endif
#ifndef NITROGEN_FILES_HH
//#include "Nitrogen/Files.hh"
#endif
#ifndef NITROGEN_MACTYPES_HH
#include "Nitrogen/MacTypes.hh"
#endif
#ifndef NUCLEUS_MAKE_H
#include "Nucleus/Make.h"
#endif
#ifndef NUCLEUS_INITIALIZE_H
#include "Nucleus/Initialize.h"
#endif
#ifndef NITROGEN_TEXTCOMMON_HH
//#include "Nitrogen/TextCommon.hh"
#endif
#include "Nucleus/ErrorsRegistered.h"


namespace Nitrogen
{
	
	NUCLEUS_DECLARE_ERRORS_DEPENDENCY( ProcessManager );
	
	enum LaunchFlags
	{
		launchContinue    = ::launchContinue,
		launchNoFileFlags = ::launchNoFileFlags,
		
		kLaunchFlags_Max = nucleus::enumeration_traits< ::LaunchFlags >::max
	};
	
	NUCLEUS_DEFINE_FLAG_OPS( LaunchFlags )
	
	using ::ProcessSerialNumber;
	using ::ProcessInfoRec;
	
	template < unsigned long > struct LowLongOfPSN  {};
	
}

namespace Nucleus
{
	
	template <>
	struct Maker< Nitrogen::ProcessSerialNumber >
	{
		Nitrogen::ProcessSerialNumber operator()( unsigned long high, unsigned long low ) const
		{
			Nitrogen::ProcessSerialNumber result;
			result.highLongOfPSN = high;
			result.lowLongOfPSN  = low;
			return result;
		}

		template < unsigned long k >
		Nitrogen::ProcessSerialNumber operator()( Nitrogen::LowLongOfPSN< k > ) const
		{
			::ProcessSerialNumber result = { 0, k };
			return result;
		}
	};
	
}

namespace nucleus
{
	
	// Since ProcessSerialNumber is declared at global scope, namespace Nitrogen isn't checked.
	// We include "nucleus/operators.hh" below to make the operators available in nucleus::operators.
	inline bool operator==( const ::ProcessSerialNumber& a, const ::ProcessSerialNumber& b )
	{
		return a.highLongOfPSN == b.highLongOfPSN
			&& a.lowLongOfPSN  == b.lowLongOfPSN;
	}
	
	inline bool operator!=( const ::ProcessSerialNumber& a, const ::ProcessSerialNumber& b )
	{
		return !( a == b );
	}
	
}

namespace Nitrogen
{
	
	inline ProcessSerialNumber NoProcess()       { return Nucleus::Make< ProcessSerialNumber >( LowLongOfPSN< kNoProcess      >() ); }
	inline ProcessSerialNumber SystemProcess()   { return Nucleus::Make< ProcessSerialNumber >( LowLongOfPSN< kSystemProcess  >() ); }
	inline ProcessSerialNumber CurrentProcess()  { return Nucleus::Make< ProcessSerialNumber >( LowLongOfPSN< kCurrentProcess >() ); }
	
}

namespace Nucleus
{
	
	template <>
	struct Initializer< Nitrogen::ProcessInfoRec >
	{
		ProcessInfoRec& operator()( Nitrogen::ProcessInfoRec& processInfo, FSSpec* appSpec = NULL ) const
		{
			processInfo.processInfoLength = sizeof processInfo;
			processInfo.processName       = NULL;
			processInfo.processAppSpec    = appSpec;
			
			return processInfo;
		}
	};
	
	template <>
	struct Maker< Nitrogen::ProcessInfoRec >
	{
		Nitrogen::ProcessInfoRec operator()( FSSpec* appSpec = NULL ) const
		{
			Nitrogen::ProcessInfoRec result;
			
			return Initialize( result, appSpec );
		}
	};
	
}

namespace Nitrogen
{
	
	// Nitrogen accessors, since no Carbon accessors exist
	inline FSSpec GetProcessInfoAppSpec( const ProcessInfoRec& processInfo )
	{
		return *processInfo.processAppSpec;
	}
	
	ProcessSerialNumber GetCurrentProcess();
	
	ProcessSerialNumber GetFrontProcess();
	
	bool SameProcess( const ProcessSerialNumber& a, 
		              const ProcessSerialNumber& b );
	
	void SetFrontProcess( const ProcessSerialNumber& psn );
	
	ProcessSerialNumber LaunchApplication( const FSSpec&   file,
	                                       LaunchFlags     launchFlags   = LaunchFlags(),
	                                       AppParameters*  appParameters = NULL );
	
	ProcessSerialNumber GetNextProcess( ProcessSerialNumber process );
	
	ProcessInfoRec& GetProcessInformation( const ProcessSerialNumber& process, ProcessInfoRec& info);
	ProcessInfoRec  GetProcessInformation( const ProcessSerialNumber& process );
	
	// 425
	FSRef GetProcessBundleLocation( const ProcessSerialNumber& psn );
	
	FSSpec GetProcessAppSpec( const ProcessSerialNumber& psn );
	
	class Process_ContainerSpecifics : public OSStatus_EndOfEnumeration< procNotFound >
	{
		public:
			typedef ProcessSerialNumber value_type;
			typedef UInt32 size_type;
			typedef SInt32 difference_type;
			typedef value_type key_type;
			
			static key_type get_next_key( const key_type& value )
			{
				return GetNextProcess( value );
			}
			
			static const key_type* get_pointer( const key_type& value )  { return &value; }
			
			static key_type begin_key()  { return get_next_key( NoProcess() ); }
			static key_type end_key()    { return               NoProcess()  ; }
	};
	
	class Process_Container: public nucleus::advance_until_error_sequence< ::Nitrogen::Process_ContainerSpecifics >
	{
		friend Process_Container Processes();
		
		private:
			Process_Container()
			: nucleus::advance_until_error_sequence< ::Nitrogen::Process_ContainerSpecifics >( ::Nitrogen::Process_ContainerSpecifics() )
			{}
	};
	
	inline Process_Container Processes()
	{
		return Process_Container();
	}
	
}

// Necessary for operators of types declared at global scope, such as ProcessSerialNumber.
#include "nucleus/operators.hh"

#endif

