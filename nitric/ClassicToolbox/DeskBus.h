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
#ifndef NUCLEUS_ID_H
#include "Nucleus/ID.h"
#endif

// Nitrogen
#ifndef NITROGEN_UPP_H
#include "Nitrogen/UPP.h"
#endif


namespace Nitrogen
{
	
	class ADBManagerErrorsRegistrationDependency
	{
		public:
			ADBManagerErrorsRegistrationDependency();
	};
	
	typedef Nucleus::ID< struct ADBAddress_Tag, ADBAddress >::Type ADBAddress;
	
	// ProcPtrs and UPPs
	
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
	
#if TARGET_CPU_68K && !TARGET_RT_MAC_CFM
	
	namespace Detail
	{
		
		#pragma parameter __D0 GetA0
		
		inline ::Ptr GetA0() = { 0x2008 };
		
		#pragma parameter __D0 GetA2
		
		inline ::Ptr GetA2() = { 0x200a };
		
		#pragma parameter __D0 GetD0
		
		inline long GetD0() = { 0x4e71 };
		
		
		typedef pascal void (*StackBased_ADBCompletionProcPtr)( ::Ptr buffer, ::Ptr refCon, long command );
		
		template < StackBased_ADBCompletionProcPtr proc >
		pascal void CallStackBasedADBCompletionProcPtr()
		{
			/*
			asm
			{
				MOVE.L   A0, -(SP) ;  // buffer
				MOVE.L   A2, -(SP) ;  // refCon
				MOVE.L   D0, -(SP) ;  // command
			}
			
			// I would have made the whole function asm,
			// but I didn't figure out how to get proc into a register.
			*/
			
			proc( GetA0(), GetA2(), GetD0() );
		}
	}
	
	template < Detail::StackBased_ADBCompletionProcPtr proc >
	struct ADBCompletionProcPtr_Traits
	{
		static ADBCompletionProcPtr GetProcPtr()
		{
			return &Detail::CallStackBasedADBCompletionProcPtr< proc >;
		}
	};
	
#else
	
	template < ADBCompletionProcPtr proc >
	struct ADBCompletionProcPtr_Traits
	{
		static ADBCompletionProcPtr GetProcPtr()
		{
			return proc;
		}
	};
	
#endif
	
	inline void DisposeADBCompletionUPP( Nucleus::Owned< ADBCompletionUPP > )  {}
	
#if !TARGET_CPU_68K || TARGET_RT_MAC_CFM
	
	inline void InvokeADBCompletionUPP( Ptr               buffer,
	                                    Ptr               refCon,
	                                    long              command,
	                                    ADBCompletionUPP  userUPP )
	{
		userUPP( buffer, refCon, command );
	}
	
#endif
	
}

#endif

