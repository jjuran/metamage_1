/*
	Mac/MixedMode/Types/UPP.hh
	--------------------------
	
	Written 2002-2009 by Lisa Lippincott and Joshua Juran.
	
	This code was written entirely by the above contributors, who place it
	in the public domain.
*/

#ifndef MAC_MIXEDMODE_TYPES_UPP_HH
#define MAC_MIXEDMODE_TYPES_UPP_HH

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __CONDITIONALMACROS__
#include <ConditionalMacros.h>
#endif

// nucleus
#ifndef NUCLEUS_OWNED_HH
#include "nucleus/owned.hh"
#endif


namespace Mac
{
	
	template < class UPP, class ProcPtr > struct UPP_Traits;
	
	template < class UPP, class R >
	struct UPP_Traits< UPP, R(*)() >
	{
		typedef UPP UPPType;
		typedef pascal R (*ProcPtr)();
		typedef UPP (*Creator)( ProcPtr );
		typedef void (*Disposer)( UPP );
		typedef R (*Invoker)( UPP );
	};
	
	template < class UPP, class R, class P0 >
	struct UPP_Traits< UPP, R(*)( P0 ) >
	{
		typedef UPP UPPType;
		typedef pascal R (*ProcPtr)( P0 );
		typedef UPP (*Creator)( ProcPtr );
		typedef void (*Disposer)( UPP );
		typedef R (*Invoker)( P0, UPP );
	};
	
	template < class UPP, class R, class P0, class P1 >
	struct UPP_Traits< UPP, R(*)( P0, P1 ) >
	{
		typedef UPP UPPType;
		typedef pascal R (*ProcPtr)( P0, P1 );
		typedef UPP (*Creator)( ProcPtr );
		typedef void (*Disposer)( UPP );
		typedef R (*Invoker)( P0, P1, UPP );
	};
	
	template < class UPP, class R, class P0, class P1, class P2 >
	struct UPP_Traits< UPP, R(*)( P0, P1, P2 ) >
	{
		typedef UPP UPPType;
		typedef pascal R (*ProcPtr)( P0, P1, P2 );
		typedef UPP (*Creator)( ProcPtr );
		typedef void (*Disposer)( UPP );
		typedef R (*Invoker)( P0, P1, P2, UPP );
	};
	
	template < class UPP, class R, class P0, class P1, class P2, class P3 >
	struct UPP_Traits< UPP, R(*)( P0, P1, P2, P3 ) >
	{
		typedef UPP UPPType;
		typedef pascal R (*ProcPtr)( P0, P1, P2, P3 );
		typedef UPP (*Creator)( ProcPtr );
		typedef void (*Disposer)( UPP );
		typedef R (*Invoker)( P0, P1, P2, P3, UPP );
	};
	
	template < class UPP, class R, class P0, class P1, class P2, class P3, class P4 >
	struct UPP_Traits< UPP, R(*)( P0, P1, P2, P3, P4 ) >
	{
		typedef UPP UPPType;
		typedef pascal R (*ProcPtr)( P0, P1, P2, P3, P4 );
		typedef UPP (*Creator)( ProcPtr );
		typedef void (*Disposer)( UPP );
		typedef R (*Invoker)( P0, P1, P2, P3, P4, UPP );
	};
	
	template < class UPP, class R, class P0, class P1, class P2, class P3, class P4, class P5 >
	struct UPP_Traits< UPP, R(*)( P0, P1, P2, P3, P4, P5 ) >
	{
		typedef UPP UPPType;
		typedef pascal R (*ProcPtr)( P0, P1, P2, P3, P4, P5 );
		typedef UPP (*Creator)( ProcPtr );
		typedef void (*Disposer)( UPP );
		typedef R (*Invoker)( P0, P1, P2, P3, P4, P5, UPP );
	};
	
	template < class UPP, class R, class P0, class P1, class P2, class P3, class P4, class P5, class P6 >
	struct UPP_Traits< UPP, R(*)( P0, P1, P2, P3, P4, P5, P6 ) >
	{
		typedef UPP UPPType;
		typedef pascal R (*ProcPtr)( P0, P1, P2, P3, P4, P5, P6 );
		typedef UPP (*Creator)( ProcPtr );
		typedef void (*Disposer)( UPP );
		typedef R (*Invoker)( P0, P1, P2, P3, P4, P5, P6, UPP );
	};
	
	template < class UPP, class R, class P0, class P1, class P2, class P3, class P4, class P5, class P6, class P7 >
	struct UPP_Traits< UPP, R(*)( P0, P1, P2, P3, P4, P5, P6, P7 ) >
	{
		typedef UPP UPPType;
		typedef pascal R (*ProcPtr)( P0, P1, P2, P3, P4, P5, P6, P7 );
		typedef UPP (*Creator)( ProcPtr );
		typedef void (*Disposer)( UPP );
		typedef R (*Invoker)( P0, P1, P2, P3, P4, P5, P6, P7, UPP );
	};
	
	template < class UPP, class R, class P0, class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8 >
	struct UPP_Traits< UPP, R(*)( P0, P1, P2, P3, P4, P5, P6, P7, P8 ) >
	{
		typedef UPP UPPType;
		typedef pascal R (*ProcPtr)( P0, P1, P2, P3, P4, P5, P6, P7, P8 );
		typedef UPP (*Creator)( ProcPtr );
		typedef void (*Disposer)( UPP );
		typedef R (*Invoker)( P0, P1, P2, P3, P4, P5, P6, P7, P8, UPP );
	};
	
	
	template < class UPPType >
	struct BasicUPP
	{
		private:
			UPPType upp;
		
		public:
			BasicUPP()                   : upp( 0 )     {}
			BasicUPP( UPPType p )        : upp( p )     {}
			
			operator UPPType() const     { return upp; }
			UPPType Get() const          { return upp; }
	};
	
	
	template < class UPPType,
	           class FunctionType,
	           typename UPP_Traits< UPPType, FunctionType >::Invoker invoke >
	struct InvokableUPP;
	
	template < class UPPType,
	           class R,
	           typename UPP_Traits< UPPType, R(*)() >::Invoker invoke >
	struct InvokableUPP< UPPType, R(*)(), invoke >: public BasicUPP< UPPType >
	{
		InvokableUPP()                        {}
		InvokableUPP( UPPType p )             : BasicUPP< UPPType >( p )     {}
		
		R operator()() const
		{
			return invoke( BasicUPP< UPPType >::Get() );
		}
	};
	
	template < class UPPType,
	           class R, class P0,
	           typename UPP_Traits< UPPType, R(*)( P0 ) >::Invoker invoke >
	struct InvokableUPP< UPPType, R(*)( P0 ),
	                     invoke >
	           : public BasicUPP< UPPType >
	{
		InvokableUPP()                        {}
		InvokableUPP( UPPType p )             : BasicUPP< UPPType >( p )     {}
		
		R operator()( P0 p0 ) const
		{
			return invoke( p0, BasicUPP< UPPType >::Get() );
		}
	};
	
	template < class UPPType,
	           class R, class P0, class P1,
	           typename UPP_Traits< UPPType, R(*)( P0, P1 ) >::Invoker invoke >
	struct InvokableUPP< UPPType, R(*)( P0, P1 ),
	                     invoke >
	           : public BasicUPP< UPPType >
	{
		InvokableUPP()                        {}
		InvokableUPP( UPPType p )             : BasicUPP< UPPType >( p )     {}
		
		R operator()( P0 p0, P1 p1 ) const
		{
			return invoke( p0, p1, BasicUPP< UPPType >::Get() );
		}
	};
	
	template < class UPPType,
	           class R, class P0, class P1, class P2,
	           typename UPP_Traits< UPPType, R(*)( P0, P1, P2 ) >::Invoker invoke >
	struct InvokableUPP< UPPType, R(*)( P0, P1, P2 ),
	                     invoke >
	           : public BasicUPP< UPPType >
	{
		InvokableUPP()                        {}
		InvokableUPP( UPPType p )             : BasicUPP< UPPType >( p )     {}
		
		R operator()( P0 p0, P1 p1, P2 p2 ) const
		{
			return invoke( p0, p1, p2, BasicUPP< UPPType >::Get() );
		}
	};
	
	template < class UPPType,
	           class R, class P0, class P1, class P2, class P3,
	           typename UPP_Traits< UPPType, R(*)( P0, P1, P2, P3 ) >::Invoker invoke >
	struct InvokableUPP< UPPType, R(*)( P0, P1, P2, P3 ),
	                     invoke >
	           : public BasicUPP< UPPType >
	{
		InvokableUPP()                        {}
		InvokableUPP( UPPType p )             : BasicUPP< UPPType >( p )     {}
		
		R operator()( P0 p0, P1 p1, P2 p2, P3 p3 ) const
		{
			return invoke( p0, p1, p2, p3, BasicUPP< UPPType >::Get() );
		}
	};
	
	template < class UPPType,
	           class R, class P0, class P1, class P2, class P3, class P4,
	           typename UPP_Traits< UPPType, R(*)( P0, P1, P2, P3, P4 ) >::Invoker invoke >
	struct InvokableUPP< UPPType, R(*)( P0, P1, P2, P3, P4 ),
	                     invoke >
	           : public BasicUPP< UPPType >
	{
		InvokableUPP()                        {}
		InvokableUPP( UPPType p )             : BasicUPP< UPPType >( p )     {}
		
		R operator()( P0 p0, P1 p1, P2 p2, P3 p3, P4 p4 ) const
		{
			return invoke( p0, p1, p2, p3, p4, BasicUPP< UPPType >::Get() );
		}
	};
	
	template < class UPPType,
	           class R, class P0, class P1, class P2, class P3, class P4, class P5,
	           typename UPP_Traits< UPPType, R(*)( P0, P1, P2, P3, P4, P5 ) >::Invoker invoke >
	struct InvokableUPP< UPPType, R(*)( P0, P1, P2, P3, P4, P5 ),
	                     invoke >
	           : public BasicUPP< UPPType >
	{
		InvokableUPP()                        {}
		InvokableUPP( UPPType p )             : BasicUPP< UPPType >( p )     {}
		
		R operator()( P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5 ) const
		{
			return invoke( p0, p1, p2, p3, p4, p5, BasicUPP< UPPType >::Get() );
		}
	};
	
	template < class UPPType,
	           class R, class P0, class P1, class P2, class P3, class P4, class P5, class P6,
	           typename UPP_Traits< UPPType, R(*)( P0, P1, P2, P3, P4, P5, P6 ) >::Invoker invoke >
	struct InvokableUPP< UPPType, R(*)( P0, P1, P2, P3, P4, P5, P6 ),
	                     invoke >
	           : public BasicUPP< UPPType >
	{
		InvokableUPP()                        {}
		InvokableUPP( UPPType p )             : BasicUPP< UPPType >( p )     {}
		
		R operator()( P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6 ) const
		{
			return invoke( p0, p1, p2, p3, p4, p5, p6, BasicUPP< UPPType >::Get() );
		}
	};
	
	template < class UPPType,
	           class R, class P0, class P1, class P2, class P3, class P4, class P5, class P6, class P7,
	           typename UPP_Traits< UPPType, R(*)( P0, P1, P2, P3, P4, P5, P6, P7 ) >::Invoker invoke >
	struct InvokableUPP< UPPType, R(*)( P0, P1, P2, P3, P4, P5, P6, P7 ),
	                     invoke >
	           : public BasicUPP< UPPType >
	{
		InvokableUPP()                        {}
		InvokableUPP( UPPType p )             : BasicUPP< UPPType >( p )     {}
		
		R operator()( P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7 ) const
		{
			return invoke( p0, p1, p2, p3, p4, p5, p6, p7, BasicUPP< UPPType >::Get() );
		}
	};
	
	template < class UPPType,
	           class R, class P0, class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8,
	           typename UPP_Traits< UPPType, R(*)( P0, P1, P2, P3, P4, P5, P6, P7, P8 ) >::Invoker invoke >
	struct InvokableUPP< UPPType, R(*)( P0, P1, P2, P3, P4, P5, P6, P7, P8 ),
	                     invoke >
	           : public BasicUPP< UPPType >
	{
		InvokableUPP()                        {}
		InvokableUPP( UPPType p )             : BasicUPP< UPPType >( p )     {}
		
		R operator()( P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8 ) const
		{
			return invoke( p0, p1, p2, p3, p4, p5, p6, p7, p8, BasicUPP< UPPType >::Get() );
		}
	};
	
	/*
		We bundle the parameter list for a UPP into a structure, so that we can have
		shorter mangled names for UPPs.  (Such enourmous mangled names make error
		messages difficult to read.)  To make a UPP with a short name, define a class
		(not a typedef name) inheriting from Basic_UPP_Details, and use that class as
		the parameter to UPP, like this:
		
		struct EventHandlerUPP_Details: Basic_UPP_Details< ::EventHandlerUPP,
		                                                   ::EventHandlerProcPtr,
		                                                   ::NewEventHandlerUPP,
		                                                   ::DisposeEventHandlerUPP,
		                                                   ::InvokeEventHandlerUPP >
		{};
		
		typedef UPP< EventHandlerUPP_Details > EventHandlerUPP;
	*/
	
	template < class UnderlyingUPPType,
	           class ProcPtrType,
	           typename UPP_Traits< UnderlyingUPPType, ProcPtrType >::Creator create,
	           typename UPP_Traits< UnderlyingUPPType, ProcPtrType >::Disposer dispose,
	           typename UPP_Traits< UnderlyingUPPType, ProcPtrType >::Invoker invoke >
	struct Basic_UPP_Details
	{
		typedef UnderlyingUPPType UPPType;
		typedef ProcPtrType ProcPtr;
		static UPPType Create( ProcPtr function )      { return create( function );  }
		static void Dispose( UPPType upp )             { dispose( upp ); }
		typedef InvokableUPP< UnderlyingUPPType, ProcPtrType, invoke > InvokableUPPType;
	};
	
	template < class NitrogenUPP >
	nucleus::owned< NitrogenUPP > NewUPP( typename NitrogenUPP::ProcPtr function )
	{
		return nucleus::owned<NitrogenUPP>::seize( NitrogenUPP::Details::Create( function ) );
	}
	
	template < class UPP_Details >
	struct UPP : public UPP_Details::InvokableUPPType
	{
		typedef typename UPP_Details::UPPType UPPType;
		typedef typename UPP_Details::ProcPtr ProcPtr;
		typedef UPP_Details Details;
		
		UPP()                                          {}
		UPP( UPPType p )                               : UPP_Details::InvokableUPPType( p )  {}
		
		static UPP Make( UPPType p )                   { return UPP( p ); }
		
		template < ProcPtr procPtr >
		static UPP Static()
		{
			static const UPPType upp = procPtr != NULL ? Details::Create( procPtr )
			                                           : NULL;
			return upp;
		}
	};
	
}

namespace nucleus
{
	
	template < class UPP_Details >
	struct disposer< Mac::UPP< UPP_Details > >
	{
		typedef Mac::UPP< UPP_Details >  argument_type;
		typedef void                     result_type;
		
		void operator()( Mac::UPP< UPP_Details > upp ) const
		{
			UPP_Details::Dispose( upp );
		}
	};
	
}

namespace Mac
{
	
	template < class NitrogenUPP, typename NitrogenUPP::ProcPtr procPtr >
	NitrogenUPP StaticUPP()
	{
		return NitrogenUPP::template Static< procPtr >();
	}
	
	// This is a workaround for a CodeWarrior Pro 9 bug: it runs into an internal
	// compiler error when a class template member is used as a template argument
	// matching a template parameter of dependent type for a function template.
	// But not for a class template.  So calling indirectly through a class template
	// member gets us around it.
	template < class NitrogenUPP, typename NitrogenUPP::ProcPtr procPtr >
	struct StaticUPPWorkaround
	{
		static NitrogenUPP StaticUPP()
		{
			return ::Mac::StaticUPP< NitrogenUPP, procPtr >();
		}
	};
	
}

#endif
