// Processes.h

#ifndef NITROGEN_PROCESSES_H
#define NITROGEN_PROCESSES_H

#ifndef NITROGEN_FRAMEWORKHEADER_H
#include "Nitrogen/FrameworkHeader.h"
#endif
#ifndef __PROCESSES__
#include FRAMEWORK_HEADER(HIServices,Processes.h)
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
	
	ProcessSerialNumber LaunchApplication( const FSSpec& file );
	
	ProcessSerialNumber GetNextProcess( ProcessSerialNumber process );
	
	void GetProcessInformation( const ProcessSerialNumber& process, ProcessInfoRec& info);
	
	class Process_Container
	{
		public:
			class const_iterator
			{
				public:
					typedef ProcessSerialNumber value_type;
					typedef void difference_type;
					typedef const value_type *pointer;
					typedef const value_type& reference;
					typedef std::forward_iterator_tag iterator_category;
				
				private:
					value_type value;
					
					void Increment()
					{
						try
						{
							value = GetNextProcess( value );
						}
						catch ( ErrorCode< OSStatus, procNotFound > )
						{
							value = NoProcess();
						}
					}
				
				public:
					const_iterator() : value( NoProcess() )  {}
					
					const_iterator& operator++()    { Increment();  return *this; }
					const_iterator operator++(int)  { const_iterator old = *this;  operator++();  return old; }
					
					reference operator*() const  { return value; }
					pointer operator->() const   { return &value; }
					
					friend bool operator==( const const_iterator& a, const const_iterator& b )    { return a.value == b.value; }
					friend bool operator!=( const const_iterator& a, const const_iterator& b )    { return !( a == b ); }
			};
		
			Process_Container()  {}
			
			const_iterator begin() const  { const_iterator first;  return ++first; }
			const_iterator end()   const  { return const_iterator(); }
	};
	
	inline Process_Container Processes()
	{
		return Process_Container();
	}
	
}

#include "Nitrogen/Operators.h"

#endif
