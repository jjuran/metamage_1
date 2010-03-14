// Nitrogen/Threads.hh
// -------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2004-2009 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef NITROGEN_THREADS_HH
#define NITROGEN_THREADS_HH

// Mac OS
#ifndef __THREADS__
#include <Threads.h>
#endif

// nucleus
#include "nucleus/enumeration_traits.hh"
#include "nucleus/flag_ops.hh"

// Nucleus
#ifndef NUCLEUS_OBJECTPARAMETERTRAITS_H
#include "Nucleus/ObjectParameterTraits.h"
#endif

// Nitrogen
#ifndef NITROGEN_MACTYPES_HH
#include "Nitrogen/MacTypes.hh"
#endif
#ifndef NITROGEN_OSSTATUS_HH
#include "Nitrogen/OSStatus.hh"
#endif
#ifndef NITROGEN_UPP_HH
#include "Nitrogen/UPP.hh"
#endif
#include "Nucleus/ErrorsRegistered.h"


namespace Nitrogen
{
	
	NUCLEUS_DECLARE_ERRORS_DEPENDENCY( ThreadManager );
	
	#pragma mark -
	#pragma mark ¥ Types and constants ¥
	
	enum ThreadState
	{
		kReadyThreadState   = ::kReadyThreadState,
		kStoppedThreadState = ::kStoppedThreadState,
		kRunningThreadState = ::kRunningThreadState,
		
		kThreadState_Max = nucleus::enumeration_traits< ::ThreadState >::max
	};
	
	using ::ThreadTaskRef;  // FIXME
	
	enum ThreadStyle
	{
		kCooperativeThread = ::kCooperativeThread,
		kPreemptiveThread  = ::kPreemptiveThread,
		
		kThreadStyle_Max = nucleus::enumeration_traits< ::ThreadStyle >::max
	};
	
	enum ThreadID
	{
		kNoThreadID          = ::kNoThreadID,
		kCurrentThreadID     = ::kCurrentThreadID,
		kApplicationThreadID = ::kApplicationThreadID,
		
		kThreadID_Max = nucleus::enumeration_traits< ::ThreadID >::max
	};
	
	enum ThreadOptions
	{
		kNewSuspend       = ::kNewSuspend,
		kUsePremadeThread = ::kUsePremadeThread,
		kCreateIfNeeded   = ::kCreateIfNeeded,
		kFPUNotNeeded     = ::kFPUNotNeeded,
		kExactMatchThread = ::kExactMatchThread,
		
		kThreadOptions_Max = nucleus::enumeration_traits< ::ThreadOptions >::max
	};
	
	NUCLEUS_DEFINE_FLAG_OPS( ThreadOptions )
	
	using ::SchedulerInfoRec;
	
	// ...
	
  }

namespace nucleus
  {
	template <>
	struct disposer< Nitrogen::ThreadID > : public std::unary_function< Nitrogen::ThreadID, void >
	{
		void operator()( Nitrogen::ThreadID thread ) const
		{
			NUCLEUS_REQUIRE_ERRORS( Nitrogen::ThreadManager );
			
			::Nitrogen::HandleDestructionOSStatus( ::DisposeThread( thread, NULL, false ) );
		}
	};
  }

namespace Nitrogen
  {
	
	namespace Detail
	{
		
		class ThreadDisposer : public std::unary_function< ThreadID, void >
		{
			private:
				bool recycleThread;
			
			public:
				ThreadDisposer( bool recycleThread = false ) : recycleThread( recycleThread )  {}
				
				void operator()( ThreadID thread ) const
				{
					NUCLEUS_REQUIRE_ERRORS( ThreadManager );
					
					HandleDestructionOSStatus( ::DisposeThread( thread, NULL, recycleThread ) );
				}
		};
		
	}
	
	#pragma mark -
	#pragma mark ¥ ThreadEntryUPP ¥
	
	struct ThreadEntryUPP_Details : Basic_UPP_Details< ::ThreadEntryUPP,
	                                                   ::ThreadEntryProcPtr,
	                                                   ::NewThreadEntryUPP,
	                                                   ::DisposeThreadEntryUPP,
	                                                   ::InvokeThreadEntryUPP >
	{};
	
	typedef UPP< ThreadEntryUPP_Details > ThreadEntryUPP;
	
	typedef ThreadEntryUPP ThreadEntryTPP;
	
	inline nucleus::owned< ThreadEntryUPP > NewThreadEntryUPP( ::ThreadEntryProcPtr p )
	{
		return NewUPP< ThreadEntryUPP >( p );
	}
	
	inline void DisposeThreadEntryUPP( nucleus::owned< ThreadEntryUPP > )  {}
	
	inline void InvokeThreadEntryUPP( void*           threadParam,
	                                  ThreadEntryUPP  userUPP )
	{
		userUPP( threadParam );
	}
	
	template < class Param, class Result >
	struct ThreadEntry_Traits
	{
		typedef Result ( *ProcPtr )( Param param );
		
		static Result InvokeWithResult( ProcPtr proc, void* param )
		{
			return proc( Nucleus::ObjectParameterTraits< Param  >::ConvertFromPointer( param ) );
		}
	};
	
	template < class Result >
	struct ThreadEntry_Traits< void, Result >
	{
		typedef Result ( *ProcPtr )();
		
		static Result InvokeWithResult( ProcPtr proc, void* )
		{
			return proc();
		}
	};
	
	template < class Param, class Result >
	struct ThreadEntry_Invoke_Traits
	{
		typedef typename ThreadEntry_Traits< Param, Result >::ProcPtr ProcPtr;
		
		typedef ThreadEntry_Traits< Param, Result > Traits;
		
		static void* Invoke( ProcPtr proc, void* param )
		{
			Result result = Traits::InvokeWithResult( proc, param );
			
			return const_cast< void* >( reinterpret_cast< const void* >( result ) );
		}
	};
	
	template < class Param >
	struct ThreadEntry_Invoke_Traits< Param, void >
	{
		typedef typename ThreadEntry_Traits< Param, void >::ProcPtr ProcPtr;
		
		typedef ThreadEntry_Traits< Param, void > Traits;
		
		static void* Invoke( ProcPtr proc, void* param )
		{
			(void) Traits::InvokeWithResult( proc, param );
			
			return NULL;
		}
	};
	
#if TARGET_CPU_PPC && TARGET_RT_MAC_CFM
	
	void Terminate_ThreadStack();
	
#else
	
	inline void Terminate_ThreadStack()
	{
	}
	
#endif
	
	template < class Param,
	           class Result,
	           typename ThreadEntry_Traits< Param, Result >::ProcPtr threadEntry >
	struct ThreadEntry
	{
		static pascal void* Adapter( void* threadParam )
		{
			if ( TARGET_CPU_PPC && TARGET_RT_MAC_CFM )
			{
				Terminate_ThreadStack();
			}
			
			try
			{
				return ThreadEntry_Invoke_Traits< Param, Result >::Invoke( threadEntry, threadParam );
			}
			catch ( ... )
			{
				return NULL;  // FIXME:  What if the thread routine throws?
			}
		}
	};
	
	#pragma mark -
	#pragma mark ¥ NewThread ¥
	
	// Level 0, returns nucleus::owned< ThreadID >
	
	nucleus::owned< ThreadID > NewThread( ThreadStyle     threadStyle,
	                                      ThreadEntryTPP  threadEntry,
	                                      void*           threadParam,
	                                      Size            stackSize,
	                                      ThreadOptions   options,
	                                      void**          threadResult = NULL );
	
	// Level 1, creates static UPP
	
	template < ::ThreadEntryProcPtr threadEntry >
	inline nucleus::owned< ThreadID > NewThread( ThreadStyle     threadStyle,
	                                             void*           threadParam,
	                                             Size            stackSize,
	                                             ThreadOptions   options,
	                                             void**          threadResult = NULL )
	{
		return NewThread( threadStyle,
		                  StaticUPP< ThreadEntryUPP, threadEntry >(),
		                  threadParam,
		                  stackSize,
		                  options,
		                  threadResult );
	}
	
	// Level 2, customizes param and result types
	
	template < class Param,
	           class Result,
	           typename ThreadEntry_Traits< Param, Result >::ProcPtr threadEntry >
	inline nucleus::owned< ThreadID >
	NewThread( ThreadStyle    threadStyle,
	           Param          param,
	           Size           stackSize = Size( 0 ),
	           ThreadOptions  options   = ThreadOptions(),
	           Result*        result    = NULL )
	{
		void*  threadParam  = const_cast< void* >( Nucleus::ObjectParameterTraits< Param  >::ConvertToPointer( param ) );
		void** threadResult = reinterpret_cast< void** >( result );
		
		return NewThread< ThreadEntry< Param,
		                               Result,
		                               threadEntry >::Adapter >( threadStyle,
		                                                         threadParam,
		                                                         stackSize,
		                                                         options,
		                                                         threadResult );
	}
	
	// No result
	template < class Param,
	           typename ThreadEntry_Traits< Param, void >::ProcPtr threadEntry >
	inline nucleus::owned< ThreadID >
	NewThread( ThreadStyle    threadStyle,
	           Param          param,
	           Size           stackSize = Size( 0 ),
	           ThreadOptions  options   = ThreadOptions() )
	{
		void*  threadParam  = const_cast< void* >( Nucleus::ObjectParameterTraits< Param  >::ConvertToPointer( param ) );
		
		return NewThread< ThreadEntry< Param,
		                               void,
		                               threadEntry >::Adapter >( threadStyle,
		                                                         threadParam,
		                                                         stackSize,
		                                                         options,
		                                                         NULL );
	}
	
	// No param
	template < class Result,
	           typename ThreadEntry_Traits< void, Result >::ProcPtr threadEntry >
	inline nucleus::owned< ThreadID >
	NewThread( ThreadStyle    threadStyle,
	           Size           stackSize = Size( 0 ),
	           ThreadOptions  options   = ThreadOptions(),
	           Result*        result    = NULL )
	{
		void** threadResult = reinterpret_cast< void** >( result );
		
		return NewThread< ThreadEntry< void,
		                               Result,
		                               threadEntry >::Adapter >( threadStyle,
		                                                         NULL,
		                                                         stackSize,
		                                                         options,
		                                                         threadResult );
	}
	
	// No result or param
	template < typename ThreadEntry_Traits< void, void >::ProcPtr threadEntry >
	inline nucleus::owned< ThreadID >
	NewThread( ThreadStyle    threadStyle,
	           Size           stackSize = Size( 0 ),
	           ThreadOptions  options   = ThreadOptions() )
	{
		return NewThread< ThreadEntry< void,
		                               void,
		                               threadEntry >::Adapter >( threadStyle,
		                                                         NULL,
		                                                         stackSize,
		                                                         options,
		                                                         NULL );
	}
	
	
	void SetThreadScheduler( ThreadSchedulerTPP threadScheduler );
	
	void SetThreadSwitcher( ThreadID         thread,
	                        ThreadSwitchTPP  threadSwitcher,
	                        void*            switchProcParam,
	                        bool             inVsOut );
	
	void SetThreadTerminator( ThreadID              thread,
	                          ThreadTerminationTPP  threadTerminator,
	                          void *                terminationProcParam );
	
	void SetDebuggerNotificationProcs( DebuggerNewThreadTPP        notifyNewThread,
	                                   DebuggerDisposeThreadTPP    notifyDisposeThread,
	                                   DebuggerThreadSchedulerTPP  notifyThreadScheduler );
	
	void CreateThreadPool( ThreadStyle  threadStyle,
	                       std::size_t  numToCreate,
	                       Size         stackSize );
	
	std::size_t GetFreeThreadCount( ThreadStyle threadStyle );
	
	std::size_t GetSpecificFreeThreadCount( ThreadStyle  threadStyle,
	                                        Size         stackSize );
	
	Size GetDefaultThreadStackSize( ThreadStyle threadStyle );
	
	std::size_t ThreadCurrentStackSpace( ThreadID thread );
	
	void DisposeThread( nucleus::owned< ThreadID >  thread,
	                    void*              threadResult,
	                    bool               recycleThread );
	
	void YieldToThread( ThreadID suggestedThread );
	
	void YieldToAnyThread();
	
	ThreadID MacGetCurrentThread();
	
	ThreadState GetThreadState( ThreadID thread );
	
	void SetThreadState( ThreadID     threadToSet,
	                     ThreadState  newState,
	                     ThreadID     suggestedThread = kNoThreadID );
	
	void SetThreadStateEndCritical( ThreadID     threadToSet,
	                                ThreadState  newState,
	                                ThreadID     suggestedThread = kNoThreadID );
	
	void ThreadBeginCritical();
	void ThreadEndCritical();
	
	ThreadTaskRef GetThreadCurrentTaskRef();
	
	ThreadState GetThreadStateGivenTaskRef( ThreadTaskRef  threadTaskRef,
	                                        ThreadID       threadToGet );
	
	void SetThreadReadyGivenTaskRef( ThreadTaskRef  threadTaskRef,
	                                 ThreadID       threadToSet );
	
}

#endif

