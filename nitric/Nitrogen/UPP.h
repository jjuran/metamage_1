// UPP.h

#ifndef NITROGEN_UPP_H
#define NITROGEN_UPP_H

#ifndef NITROGEN_OWNED_H
#include "Nitrogen/Owned.h"
#endif

namespace Nitrogen
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
         
         friend bool operator==( const BasicUPP& a, const BasicUPP& b )    { return a.Get() == b.Get(); }
         friend bool operator!=( const BasicUPP& a, const BasicUPP& b )    { return a.Get() != b.Get(); }

         friend bool operator==( const BasicUPP& a, const UPPType& b )     { return a.Get() == b; }
         friend bool operator==( const BasicUPP& a, const UPPType& b )     { return a.Get() == b; }
         
         friend bool operator!=( const UPPType& a, const BasicUPP& b )     { return a != b.Get(); }
         friend bool operator!=( const UPPType& a, const BasicUPP& b )     { return a != b.Get(); }
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


   template < class UnderlyingUPPType,
              class ProcPtrType,
              typename UPP_Traits< UnderlyingUPPType, ProcPtrType >::Creator create,
              typename UPP_Traits< UnderlyingUPPType, ProcPtrType >::Disposer dispose,
              typename UPP_Traits< UnderlyingUPPType, ProcPtrType >::Invoker invoke >
   struct UPP : public InvokableUPP< UnderlyingUPPType, ProcPtrType, invoke >
     {
      typedef UnderlyingUPPType UPPType;
      typedef ProcPtrType ProcPtr;
      
      UPP()                                          {}
      UPP( UPPType p )                               : InvokableUPP< UPPType, ProcPtr, invoke >( p )  {}
      
      static UPP Make( UPPType p )                   { return UPP( p ); }
      static Owned<UPP> New( ProcPtr function )      { return Owned<UPP>::Seize( create( function ) ); }
     };

   
   template < class NitrogenUPP >
   Owned< NitrogenUPP > NewUPP( typename NitrogenUPP::ProcPtr function )
     {
      return NitrogenUPP::New( function );
     }
   
   template < class UPPType,
              class ProcPtr,
              typename UPP_Traits< UPPType, ProcPtr >::Creator create,
              typename UPP_Traits< UPPType, ProcPtr >::Disposer dispose,
              typename UPP_Traits< UPPType, ProcPtr >::Invoker invoke >
   struct Disposer< UPP< UPPType, ProcPtr, create, dispose, invoke > >
            : public std::unary_function< UPP< UPPType, ProcPtr, create, dispose, invoke >, void >
     {
      typedef UPP< UPPType, ProcPtr, create, dispose, invoke > NitrogenUPP;
      
      void operator()( NitrogenUPP upp ) const
        {
         if ( upp != NitrogenUPP() )
            dispose( upp );
        }
     };
   
   template < class NitrogenUPP, typename NitrogenUPP::ProcPtr procPtr >
   NitrogenUPP StaticUPP()
     {
      static const Owned<NitrogenUPP> upp( NitrogenUPP::New( procPtr ) );
      return upp;
     }
  }

#endif
