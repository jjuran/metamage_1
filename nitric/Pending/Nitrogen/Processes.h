// Processes.h

#ifndef NITROGEN_PROCESSES_H
#define NITROGEN_PROCESSES_H

#ifndef NITROGEN_FRAMEWORKHEADER_H
#include "Nitrogen/FrameworkHeader.h"
#endif
#ifndef __PROCESSES__
#include FRAMEWORK_HEADER(HIServices,Processes.h)
#endif

#ifndef NITROGEN_ADVANCEUNTILFAILURECONTAINER_H
#include "Nitrogen/AdvanceUntilFailureContainer.h"
#endif
#ifndef NITROGEN_FLAGTYPE_H
#include "Nitrogen/FlagType.h"
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
#ifndef NITROGEN_MAKE_H
#include "Nitrogen/Make.h"
#endif
#ifndef NITROGEN_TEXTCOMMON_H
//#include "Nitrogen/TextCommon.h"
#endif

namespace Nitrogen {
	
	void RegisterProcessManagerErrors();
	
	struct LaunchFlags_Tag {};
	typedef FlagType< LaunchFlags_Tag, ::LaunchFlags, 0 > LaunchFlags;
	
	static const LaunchFlags launchContinue    = LaunchFlags::Make( ::launchContinue    );
	static const LaunchFlags launchNoFileFlags = LaunchFlags::Make( ::launchNoFileFlags );
	
	using ::ProcessSerialNumber;
	using ::ProcessInfoRec;
	
	template < unsigned long > struct PSN_Constant  {};
	
	static const PSN_Constant< kNoProcess >      kNoProcess      = PSN_Constant< ::kNoProcess >();
	static const PSN_Constant< kSystemProcess >  kSystemProcess  = PSN_Constant< ::kSystemProcess >();
	static const PSN_Constant< kCurrentProcess > kCurrentProcess = PSN_Constant< ::kCurrentProcess >();
	
	template <>
	struct Maker< ProcessSerialNumber >
	{
		ProcessSerialNumber operator()( unsigned long high, unsigned long low ) const
		{
			ProcessSerialNumber result;
			result.highLongOfPSN = high;
			result.lowLongOfPSN  = low;
			return result;
		}
		
		template < unsigned long k >
		ProcessSerialNumber operator()( PSN_Constant< k > ) const
		{
			ProcessSerialNumber result = { 0, k };
			return result;
		}
	};
	
	inline
	bool
	operator==( const ProcessSerialNumber& a, const ProcessSerialNumber& b )
	{
		return a.highLongOfPSN == b.highLongOfPSN
			&& a.lowLongOfPSN  == b.lowLongOfPSN;
	}
	
	inline
	bool
	operator!=( const ProcessSerialNumber& one, const ProcessSerialNumber& other )
	{
		return !( one == other );
	}
	
	inline ProcessSerialNumber NoProcess()       { return Make< ProcessSerialNumber >( kNoProcess ); }
	inline ProcessSerialNumber SystemProcess()   { return Make< ProcessSerialNumber >( kSystemProcess ); }
	inline ProcessSerialNumber CurrentProcess()  { return Make< ProcessSerialNumber >( kCurrentProcess ); }
	
	ProcessSerialNumber GetCurrentProcess();
	
	ProcessSerialNumber GetFrontProcess();
	
	bool SameProcess(
		const ProcessSerialNumber& one, 
		const ProcessSerialNumber& other
	);
	
	void SetFrontProcess( const ProcessSerialNumber& psn );
	
	ProcessSerialNumber LaunchApplication( const FSSpec& file, LaunchFlags launchFlags = LaunchFlags(), AppParameters* appParameters = NULL );
	
	ProcessSerialNumber GetNextProcess( ProcessSerialNumber process );
	
	void GetProcessInformation( const ProcessSerialNumber& process, ProcessInfoRec& info);
	ProcessInfoRec GetProcessInformation( const ProcessSerialNumber& process );
	
	// 425
	FSRef GetProcessBundleLocation( const ProcessSerialNumber& psn );
	
	std::size_t SizeOf_AppParameters( const AppParameters& appParameters );
	
	class Process_ContainerSpecifics
	{
		public:
			typedef ProcessSerialNumber value_type;
			typedef UInt32 size_type;
			typedef SInt32 difference_type;
			
			typedef ErrorCode< OSStatus, procNotFound > EndOfEnumeration;
			
			value_type GetNextValue( const value_type& value )
			{
				return GetNextProcess( value );
			}
			
			static value_type begin_value()  { return NoProcess(); }
			static value_type end_value()    { return NoProcess(); }
	};
	
	class Process_Container: public AdvanceUntilFailureContainer< Process_ContainerSpecifics >
	{
		friend Process_Container Processes();
		
		private:
			Process_Container()
			: AdvanceUntilFailureContainer< Process_ContainerSpecifics >( Process_ContainerSpecifics() )
			{}
	};
	
	inline Process_Container Processes()
	{
		return Process_Container();
	}
	
}

#include "Nitrogen/Operators.h"

#endif
