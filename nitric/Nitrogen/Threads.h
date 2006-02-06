//	=========
//	Threads.h
//	=========

#ifndef NITROGEN_THREADS_H
#define NITROGEN_THREADS_H

// Universal Interfaces
#ifndef __THREADS__
#include <Threads.h>
#endif

// Nitrogen core
#ifndef NUCLEUS_OBJECTPARAMETERTRAITS_H
#include "Nucleus/ObjectParameterTraits.h"
#endif
#ifndef NUCLEUS_ONLYONCE_H
#include "Nucleus/OnlyOnce.h"
#endif

// Nitrogen Carbon support
#ifndef NITROGEN_OSSTATUS_H
#include "Nitrogen/OSStatus.h"
#endif
#ifndef NITROGEN_UPP_H
#include "Nitrogen/UPP.h"
#endif

#ifndef NUCLEUS_SELECTORTYPE_H
#include "Nucleus/SelectorType.h"
#endif

#ifndef NUCLEUS_IDTYPE_H
#include "Nucleus/IDType.h"
#endif

#ifndef NUCLEUS_FLAGTYPE_H
#include "Nucleus/FlagType.h"
#endif


namespace Nitrogen
{
	
	void RegisterThreadManagerErrors();
	
	class ThreadState_Tag {};
	typedef Nucleus::SelectorType< ThreadState_Tag, ::ThreadState, kReadyThreadState > ThreadState;
	
	using ::ThreadTaskRef;  // FIXME
	
	class ThreadStyle_Tag {};
	typedef Nucleus::SelectorType< ThreadStyle_Tag, ::ThreadStyle, kCooperativeThread > ThreadStyle;
	
	class ThreadID_Tag {};
	typedef Nucleus::IDType< ThreadID_Tag, ::ThreadID, kNoThreadID > ThreadID;
	
	class ThreadOptions_Tag {};
	typedef Nucleus::FlagType< ThreadOptions_Tag, ::ThreadOptions > ThreadOptions;
	
	using ::SchedulerInfoRec;
	
	// ...
	
  }

namespace Nucleus
  {
	template <>
	struct Disposer< Nitrogen::ThreadID > : public  std::unary_function< Nitrogen::ThreadID, void >,
	                                        private Nitrogen::DefaultDestructionOSStatusPolicy
	{
		void operator()( Nitrogen::ThreadID thread ) const
		{
			Nucleus::OnlyOnce< Nitrogen::RegisterThreadManagerErrors >();
			
			HandleDestructionOSStatus( ::DisposeThread( thread, NULL, false ) );
		}
	};
  }

namespace Nitrogen
  {
	
	struct ThreadEntryUPP_Details : Basic_UPP_Details< ::ThreadEntryUPP,
	                                                   ::ThreadEntryProcPtr,
	                                                   ::NewThreadEntryUPP,
	                                                   ::DisposeThreadEntryUPP,
	                                                   ::InvokeThreadEntryUPP >
	{};
	
	typedef UPP< ThreadEntryUPP_Details > ThreadEntryUPP;
	
	typedef ThreadEntryUPP ThreadEntryTPP;
	
	inline Nucleus::Owned< ThreadEntryUPP > NewThreadEntryUPP( ::ThreadEntryProcPtr p )
	{
		return NewUPP< ThreadEntryUPP >( p );
	}
	
	inline void DisposeThreadEntryUPP( Nucleus::Owned< ThreadEntryUPP > )  {}
	
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
			(void)Traits::InvokeWithResult( proc, param );
			
			return NULL;
		}
	};
	
	template < class Param,
	           class Result,
	           typename ThreadEntry_Traits< Param, Result >::ProcPtr threadEntry >
	struct ThreadEntry
	{
		static pascal void* Adapter( void* threadParam )
		{
			try
			{
				return ThreadEntry_Invoke_Traits< Param, Result >::Invoke( threadEntry, threadParam );
			}
			catch ( OSStatus err )
			{
				return NULL;  // FIXME:  What if the thread routine throws?
			}
		}
	};
	
	// Level 0, returns Nucleus::Owned< ThreadID >
	
	Nucleus::Owned< ThreadID > NewThread( ThreadStyle     threadStyle,
	                             ThreadEntryTPP  threadEntry,
	                             void*           threadParam,
	                             Size            stackSize,
	                             ThreadOptions   options,
	                             void**          threadResult = NULL );
	
	// Level 1, creates static UPP
	
	template < ::ThreadEntryProcPtr threadEntry >
	Nucleus::Owned< ThreadID > NewThread( ThreadStyle     threadStyle,
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
	inline Nucleus::Owned< ThreadID >
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
	inline Nucleus::Owned< ThreadID >
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
	inline Nucleus::Owned< ThreadID >
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
	inline Nucleus::Owned< ThreadID >
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
	
	void DisposeThread( Nucleus::Owned< ThreadID >  thread,
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

