//	=========
//	Threads.h
//	=========

#ifndef NITROGEN_THREADS_H
#define NITROGEN_THREADS_H

// Universal Interfaces
#ifndef __THREADS__
#include <Threads.h>
#endif

// Nucleus
#ifndef NUCLEUS_OBJECTPARAMETERTRAITS_H
#include "Nucleus/ObjectParameterTraits.h"
#endif
#ifndef NUCLEUS_FLAG_H
#include "Nucleus/Flag.h"
#endif
#ifndef NUCLEUS_ID_H
#include "Nucleus/ID.h"
#endif
#ifndef NUCLEUS_SELECTOR_H
#include "Nucleus/Selector.h"
#endif

// Nitrogen
#ifndef NITROGEN_MACTYPES_H
#include "Nitrogen/MacTypes.h"
#endif
#ifndef NITROGEN_OSSTATUS_H
#include "Nitrogen/OSStatus.h"
#endif
#ifndef NITROGEN_UPP_H
#include "Nitrogen/UPP.h"
#endif


namespace Nitrogen
{
	
	class ThreadManagerErrorsRegistrationDependency
	{
		public:
			ThreadManagerErrorsRegistrationDependency();
	};
	
	#pragma mark -
	#pragma mark ¥ Types and constants ¥
	
	typedef Nucleus::Selector< class ThreadState_Tag, ::ThreadState >::Type ThreadState;
	
	static const ThreadState kReadyThreadState   = ThreadState( ::kReadyThreadState   );
	static const ThreadState kStoppedThreadState = ThreadState( ::kStoppedThreadState );
	static const ThreadState kRunningThreadState = ThreadState( ::kRunningThreadState );
	
	using ::ThreadTaskRef;  // FIXME
	
	typedef Nucleus::Selector< class ThreadStyle_Tag, ::ThreadStyle >::Type ThreadStyle;
	
	static const ThreadStyle kCooperativeThread = ThreadStyle( ::kCooperativeThread );
	static const ThreadStyle kPreemptiveThread  = ThreadStyle( ::kPreemptiveThread  );
	
	typedef Nucleus::ID< class ThreadID_Tag, ::ThreadID >::Type ThreadID;
	
	static const ThreadID kNoThreadID          = ThreadID( ::kNoThreadID          );
	static const ThreadID kCurrentThreadID     = ThreadID( ::kCurrentThreadID     );
	static const ThreadID kApplicationThreadID = ThreadID( ::kApplicationThreadID );
	
	typedef Nucleus::Flag< class ThreadOptions_Tag, ::ThreadOptions >::Type ThreadOptions;
	
	NUCLEUS_DEFINE_FLAG_OPS( ThreadOptions )
	
	static const ThreadOptions kNewSuspend       = ThreadOptions( ::kNewSuspend       );
	static const ThreadOptions kUsePremadeThread = ThreadOptions( ::kUsePremadeThread );
	static const ThreadOptions kCreateIfNeeded   = ThreadOptions( ::kCreateIfNeeded   );
	static const ThreadOptions kFPUNotNeeded     = ThreadOptions( ::kFPUNotNeeded     );
	static const ThreadOptions kExactMatchThread = ThreadOptions( ::kExactMatchThread );
	
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
			(void) Nitrogen::ThreadManagerErrorsRegistrationDependency();
			
			HandleDestructionOSStatus( ::DisposeThread( thread, NULL, false ) );
		}
	};
  }

namespace Nitrogen
  {
	
	namespace Detail
	{
		
		class ThreadDisposer : public std::unary_function< ThreadID, void >,
	                           private DefaultDestructionOSStatusPolicy
		{
			private:
				bool recycleThread;
			
			public:
				ThreadDisposer( bool recycleThread = false ) : recycleThread( recycleThread )  {}
				
				void operator()( ThreadID thread ) const
				{
					(void) ThreadManagerErrorsRegistrationDependency();
					
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
			(void) Traits::InvokeWithResult( proc, param );
			
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
			catch ( ... )
			{
				return NULL;  // FIXME:  What if the thread routine throws?
			}
		}
	};
	
	#pragma mark -
	#pragma mark ¥ NewThread ¥
	
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

