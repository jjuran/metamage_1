/*	==========
 *	DeskBus.hh
 *	==========
 */

#ifndef CLASSICTOOLBOX_DESKBUS_HH
#define CLASSICTOOLBOX_DESKBUS_HH

#if TARGET_API_MAC_CARBON
#error This module is for classic Toolbox only
#endif

// Mac OS
#ifndef __DESKBUS__
#include <DeskBus.h>
#endif

// nucleus
#ifndef NUCLEUS_ENUMERATIONTRAITS_HH
#include "nucleus/enumeration_traits.hh"
#endif
#ifndef NUCLEUS_ERRORSREGISTERED_HH
#include "nucleus/errors_registered.hh"
#endif
#ifndef NUCLEUS_INDEXEDSEQUENCE_HH
#include "nucleus/indexed_sequence.hh"
#endif

// Nitrogen
#ifndef NITROGEN_UPP_HH
#include "Nitrogen/UPP.hh"
#endif


namespace Nitrogen
{
	
	NUCLEUS_DECLARE_ERRORS_DEPENDENCY( ADBManager );
	
	
	enum ADBAddress
	{
		kADBAddress_Max = nucleus::enumeration_traits< ::ADBAddress >::max
	};
	
	// ProcPtrs and UPPs
	
	typedef pascal void (*ADBCompletionProcPtr)( ::Ptr buffer, ::Ptr refCon, long command );
	
#if TARGET_CPU_68K && !TARGET_RT_MAC_CFM
	
	template < class ProcPtr, ProcPtr procPtr >
	inline pascal void ADBCompletion_Glue()
	{
		asm
		{
			MOVE.L  A0, -(SP) ;  // buffer
			MOVE.L  A2, -(SP) ;  // refCon
			MOVE.L  D0, -(SP) ;  // command
			
			JSR     procPtr
		}
	}
	
	struct ADBCompletionUPP_Details
	{
		typedef ::ADBCompletionUPP UPPType;
		
		// This is the stack-based function signature
		typedef ADBCompletionProcPtr ProcPtr;
		
		template < ProcPtr procPtr >
		static pascal void Glue()
		{
			ADBCompletion_Glue< ProcPtr, procPtr >();
		}
	};
	
	typedef GlueUPP< ADBCompletionUPP_Details > ADBCompletionUPP;
	
#else
	
	struct ADBCompletionUPP_Details : Basic_UPP_Details< ::ADBCompletionUPP,
	                                                     ::ADBCompletionProcPtr,
	                                                     ::NewADBCompletionUPP,
	                                                     ::DisposeADBCompletionUPP,
	                                                     ::InvokeADBCompletionUPP >
	{
	};
	
	typedef UPP< ADBCompletionUPP_Details > ADBCompletionUPP;
	
#endif
	
	using ::ADBDataBlock;
	using ::ADBSetInfoBlock;
	using ::ADBOpBlock;
	
	#pragma mark -
	#pragma mark ¥ Routines ¥
	
	using ::ADBReInit;
	
	void ADBOp( ::Ptr refCon, ADBCompletionUPP completion, ::Ptr buffer, short commandNum );
	
	template < ADBCompletionProcPtr proc >
	inline void ADBOp( ::Ptr refCon, ::Ptr buffer, short commandNum )
	{
		ADBCompletionUPP upp = StaticUPP< ADBCompletionUPP, proc >();
			
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
	
	inline nucleus::owned< ADBCompletionUPP > NewADBCompletionUPP( ADBCompletionProcPtr p )
	{
		return NewUPP< ADBCompletionUPP >( p );
	}
	
	inline void DisposeADBCompletionUPP( nucleus::owned< ADBCompletionUPP > )  {}
	
	inline void InvokeADBCompletionUPP( Ptr               buffer,
	                                    Ptr               refCon,
	                                    long              command,
	                                    ADBCompletionUPP  userUPP )
	{
		::InvokeADBCompletionUPP( buffer, refCon, command, userUPP.Get() );
	}
	
	
	struct ADBDevice_Container_Specifics
	{
		typedef UInt16             size_type;
		typedef SInt16             difference_type;
		typedef GetIndADB_Result   value_type;
		typedef const value_type&  const_reference;
		typedef const value_type*  const_pointer;
		
		class Nothing {};
		
		typedef Nothing context_type;
		
		
		static size_type Size( context_type )
		{
			return CountADBs();
		}
		
		static const_reference get_reference( context_type state, size_type position )
		{
			return GetIndADB( position + 1 );
		}
		
		static const_pointer get_pointer( context_type state, size_type position )
		{
			return &get_reference( state, position );
		}
	};
	
	class ADBDevice_Container : public nucleus::const_indexed_sequence< ADBDevice_Container_Specifics >
	{
		public:
			ADBDevice_Container() : const_indexed_sequence( ADBDevice_Container_Specifics::Nothing() )  {}
	};
	
}

#endif

