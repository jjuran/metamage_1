/*	=========
 *	DeskBus.h
 *	=========
 */

#ifndef CLASSICTOOLBOX_DESKBUS_H
#define CLASSICTOOLBOX_DESKBUS_H

#if TARGET_API_MAC_CARBON
#error This module is for classic Toolbox only
#endif

// Mac OS Universal Interfaces
#ifndef __DESKBUS__
#include <DeskBus.h>
#endif

// Nucleus
#include "Nucleus/ErrorsRegistered.h"
#ifndef NUCLEUS_ID_H
#include "Nucleus/ID.h"
#endif
#ifndef NUCLEUS_INDEXEDCONTAINER_H
#include "Nucleus/IndexedContainer.h"
#endif

// Nitrogen
#ifndef NITROGEN_UPP_H
#include "Nitrogen/UPP.h"
#endif


namespace Nitrogen
{
	
	NUCLEUS_DECLARE_ERRORS_DEPENDENCY( ADBManager );
	
	
	typedef Nucleus::ID< struct ADBAddress_Tag, ADBAddress >::Type ADBAddress;
	
	// ProcPtrs and UPPs
	
	namespace Glue
	{
	#if TARGET_CPU_68K && !TARGET_RT_MAC_CFM
		
		typedef pascal void (*ADBCompletionProcPtr)( ::Ptr buffer, ::Ptr refCon, long command );
		
		template < ADBCompletionProcPtr proc >
		pascal void ADBCompletionProcPtr_Glue()
		{
			asm
			{
				MOVE.L   A0, -(SP) ;  // buffer
				MOVE.L   A2, -(SP) ;  // refCon
				MOVE.L   D0, -(SP) ;  // command
				
				JSR      proc      ;
			}
		}
		
	#else
		
		using ::ADBCompletionProcPtr;
		
	#endif
	}
	
	struct ADBCompletionUPP_Details : Basic_UPP_Details< ::ADBCompletionUPP,
	                                                     ::ADBCompletionProcPtr,
	                                                     ::NewADBCompletionUPP,
	                                                     ::DisposeADBCompletionUPP,
	                                                     ::InvokeADBCompletionUPP >
	{};
	
	typedef UPP< ADBCompletionUPP_Details > ADBCompletionUPP;
	
	using ::ADBDataBlock;
	using ::ADBSetInfoBlock;
	using ::ADBOpBlock;
	
	#pragma mark -
	#pragma mark ¥ Routines ¥
	
	using ::ADBReInit;
	
	void ADBOp( ::Ptr refCon, ADBCompletionUPP completion, ::Ptr buffer, short commandNum );
	
	template < Glue::ADBCompletionProcPtr proc >
	inline void ADBOp( ::Ptr refCon, ::Ptr buffer, short commandNum )
	{
		ADBCompletionUPP upp =
		
		#if TARGET_CPU_68K && !TARGET_RT_MAC_CFM
			
			Glue::ADBCompletionProcPtr_Glue< proc >;
			
		#else
			
			StaticUPP< ADBCompletionUPP, proc >();
			
		#endif
		
		Nitrogen::ADBOp( refCon, upp, buffer, commandNum );
	}
	
	using ::CountADBs;
	
	class GetIndADB_Failed {};
	
	struct GetIndADB_Result
	{
		ADBAddress    address;
		ADBDataBlock  data;
		
		operator ADBAddress() const  { return address; }
		
		operator const ADBDataBlock&() const  { return data; }
	};
	
	ADBAddress GetIndADB( ADBDataBlock& data, short index );
	
	inline GetIndADB_Result GetIndADB( short index )
	{
		GetIndADB_Result result;
		
		result.address = GetIndADB( result.data, index );
		
		return result;
	}
	
	ADBDataBlock GetADBInfo( ADBAddress adbAttr );
	
	void SetADBInfo( const ADBSetInfoBlock& info, ADBAddress adbAttr );
	
	// UPP management
	
	inline Nucleus::Owned< ADBCompletionUPP > NewADBCompletionUPP( ::ADBCompletionProcPtr p )
	{
		return NewUPP< ADBCompletionUPP >( p );
	}
	
	inline void DisposeADBCompletionUPP( Nucleus::Owned< ADBCompletionUPP > )  {}
	
	inline void InvokeADBCompletionUPP( Ptr               buffer,
	                                    Ptr               refCon,
	                                    long              command,
	                                    ADBCompletionUPP  userUPP )
	{
	#if TARGET_CPU_68K && !TARGET_RT_MAC_CFM
		
		asm
		{
			MOVE.L   buffer,  A0 ;
			MOVE.L   refCon,  A2 ;
			MOVE.L   command, D0 ;
		}
		
		userUPP();
		
	#else
		
		userUPP( buffer, refCon, command );
		
	#endif
	}
	
	
	struct ADBDevice_Container_Specifics
	{
		typedef UInt16             size_type;
		typedef SInt16             difference_type;
		typedef GetIndADB_Result   value_type;
		typedef const value_type&  const_reference;
		typedef const value_type*  const_pointer;
		
		class Nothing {};
		
		typedef Nothing ConstIteratorState;
		typedef Nothing ConstContainerState;
		
		
		static size_type Size( ConstContainerState )
		{
			return CountADBs();
		}
		
		static const_reference GetReference( ConstIteratorState state, size_type position )
		{
			return GetIndADB( position + 1 );
		}
		
		static const_pointer GetPointer( ConstIteratorState state, size_type position )
		{
			return &GetReference( state, position );
		}
	};
	
	class ADBDevice_Container : public Nucleus::ConstIndexedContainer< ADBDevice_Container_Specifics >
	{
		public:
			ADBDevice_Container() : ConstIndexedContainer( ADBDevice_Container_Specifics::Nothing() )  {}
	};
	
}

#endif

