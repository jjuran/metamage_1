// Processes.h

#ifndef NITROGEN_PROCESSES_H
#define NITROGEN_PROCESSES_H

#ifndef NITROGEN_FRAMEWORKHEADER_H
#include "Nitrogen/FrameworkHeader.h"
#endif
#ifndef __PROCESSES__
#include FRAMEWORK_HEADER(HIServices,Processes.h)
#endif

#ifndef NUCLEUS_ADVANCEUNTILFAILURECONTAINER_H
#include "Nucleus/AdvanceUntilFailureContainer.h"
#endif
#ifndef NUCLEUS_FLAGTYPE_H
#include "Nucleus/FlagType.h"
#endif
#ifndef NITROGEN_CFSTRING_H
//#include "Nitrogen/CFString.h"
#endif
#ifndef NITROGEN_EVENTS_H
//#include "Nitrogen/Events.h"
#endif
#ifndef NITROGEN_FILES_H
//#include "Nitrogen/Files.h"
#endif
#ifndef NITROGEN_MACTYPES_H
#include "Nitrogen/MacTypes.h"
#endif
#ifndef NUCLEUS_MAKE_H
#include "Nucleus/Make.h"
#endif
#ifndef NUCLEUS_INITIALIZE_H
#include "Nucleus/Initialize.h"
#endif
#ifndef NITROGEN_TEXTCOMMON_H
//#include "Nitrogen/TextCommon.h"
#endif

namespace Nitrogen
{
	
	class ProcessManagerErrorsRegistrationDependency
	{
		public:
			ProcessManagerErrorsRegistrationDependency();
	};
	
	struct LaunchFlags_Tag {};
	typedef Nucleus::FlagType< LaunchFlags_Tag, ::LaunchFlags > LaunchFlags;
	
	inline LaunchFlags LaunchContinue   ()  { return LaunchFlags::Make( launchContinue    ); }
	inline LaunchFlags LaunchNoFileFlags()  { return LaunchFlags::Make( launchNoFileFlags ); }
	
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
	
	// Since ProcessSerialNumber is declared at global scope, namespace Nitrogen isn't checked.
	// We include "Nucleus/Operators.h" below to make the operators available in Nitrogen::Operators.
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
	
	class Process_ContainerSpecifics
	{
		public:
			typedef ProcessSerialNumber value_type;
			typedef UInt32 size_type;
			typedef SInt32 difference_type;
			
			typedef Nucleus::ErrorCode< OSStatus, procNotFound > EndOfEnumeration;
			
			value_type GetNextValue( const value_type& value )
			{
				return GetNextProcess( value );
			}
			
			static value_type begin_value()  { return NoProcess(); }
			static value_type end_value()    { return NoProcess(); }
	};
	
	class Process_Container: public Nucleus::AdvanceUntilFailureContainer< ::Nitrogen::Process_ContainerSpecifics >
	{
		friend Process_Container Processes();
		
		private:
			Process_Container()
			: Nucleus::AdvanceUntilFailureContainer< ::Nitrogen::Process_ContainerSpecifics >( ::Nitrogen::Process_ContainerSpecifics() )
			{}
	};
	
	inline Process_Container Processes()
	{
		return Process_Container();
	}
	
}

// Necessary for operators of types declared at global scope, such as ProcessSerialNumber.
#include "Nucleus/Operators.h"

#endif

