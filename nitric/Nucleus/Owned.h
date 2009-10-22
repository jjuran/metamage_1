// Nucleus/Owned.h
// ---------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2002-2007 by Lisa Lippincott and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


/*
      Owned: Ownership of a disposable resource.
     
      template < class Resource, class Disposer = OwnedDefaults<Resource>::DisposerType >
      class Owned;

         Resource: a type which can be disposed, deleted, removed, released, closed, or unlocked
         Disposer: a functor class specifying the manner of disposal
      
      An object of type Owned< Resource, Disposer > represents the ownership of, and responsibility
      for disposal of that aspect of Resource which is destroyed by Disposer.  In most cases, the
      aspect destroyed is the entire resource.
      
      The template Owned is similar to std::auto_ptr, but where auto_ptr uses delete to destroy
      objects created by new-expressions, Owned uses its Disposer class to destroy resources created
      by other means.
      
      So, for example, Owned<WindowRef> represents ownership of a window, and the consequent
      responsibility for calling DisposeWindow.  And Owned<CFStringRef> represents ownership of
      (a reference counting point of) a CFString, and a responsibility for calling CFRelease.
      
      The static member function Owned< Resource, Disposer >::Seize( const Resource& ) can be
      used to claim ownership of a resource.  Conversely, Owned< Resource, Disposer >::Release()
      is used to give up ownership.  These functions are intended to be used as an interface
      between code using Owned and code which represents ownership by other means, or which does
      not represent ownership at all.
      
      Owned< Resource, Disposer > provides pointer-like operators * and -> which can be used
      to access the owned resource.
      
      The destructor of Owned< Resource, Disposer > calls Disposer::operator() to dispose of the
      owned resource.  Since default-constructed objects of type Owned< Resource, Disposer >
      own an explicitly default-constructed Resource, it is important for Disposer::operator()
      to do little or nothing when passed this value.
      
      Copy construction, assignment, and conversion of Owned-types represent transfers of ownership:
      Given objects r1 and r2 of type Owned<R,D>, we have:
      
         Owned<R,D> r = r1; // r now holds the resource formerly in r1; r1 holds R().

         r = r2;            // the resource formerly in r is disposed;
                            // r now holds the resource formerly in r2; r2 holds R().

         Owned<S,D> s = r;  // s now holds the resource formerly in r, converted to type S;
                            // r holds R().
      
      Conversion from Owned<R,DR> to Owned<S,DS> is only allowed when R is implicitly convertable
      to S and DR is the same type as DS.
      
      These rules have implications for the use of Owned-types in function signatures.  A function
      parameter of type Owned<R,D> represents a transfer of ownership from the caller to the called
      function; a function result of type Owned<R,D> represents a transfer of ownership from the
      called function to the caller.
      
      It's possible to be caught off guard by an unexpected transfer of ownership out of an
      Owned object.  One may guard against this problem by declaring Owned objects const where
      transfer is not desired.  Const Owned objects hold ownership of their resources for
      their entire lifetime.
      
      Since most types are disposed of in only one sense, Owned uses a traits class

         template < class Resource > struct OwnedDefaults;
      
      to find the default disposer for each type Resource.  The default disposer is
      OwnedDefaults<Resource>::DisposerType; in the unspecialized OwnedDefaults template,
      this is a typedef name for Disposer<Resource>.
      
      The template 
      
         template < class Resource > struct Disposer;
      
      has no definition; instead, specializations provide the default disposers for various
      types.
      
      This two stage lookup for disposers allows default disposers to be specified in two ways.
      Most often, one specializes Disposer to provide the right disposer for a particular type:

         namespace Nucleus
           {      
            template <> struct Disposer<CFTypeRef>: public std::unary_function< CFTypeRef, void >
              {
               void operator()( CFTypeRef cf ) const
                 {
                  if ( cf != 0 )
                     ::CFRelease( cf );
                 }
              };
           }      
      
      But conversions are only allowed when two types share a disposer class.  To acheive this
      effect, specialize OwnedDefaults:
      
         namespace Nucleus
           {      
            template <> struct OwnedDefaults< CFStringRef >: OwnedDefaults< CFTypeRef > {};
           }
       
       With these declarations, OwnedDefaults<CFStringRef>::DisposerType and OwnedDefaults<CFTypeRef>::DisposerType
       are both names for Disposer<CFTypeRef>, and thus Owned<CFStringRef> may be converted to
       Owned<CFTypeRef>.
*/

#ifndef NUCLEUS_OWNED_H
#define NUCLEUS_OWNED_H

#ifndef NUCLEUS_CONVERT_H
#include "Nucleus/Convert.h"
#endif

#include <algorithm>

namespace Nucleus
  {
   template < class Resource > struct Disposer;
   
   template < class Resource >
   struct Disposer_Traits
     {
      typedef Disposer< Resource > Type;
     };

   template < class Resource >
   struct DefaultValue_Traits
     {
      static Resource DefaultValue()  { return Resource(); }
     };

   template < class Resource >
   struct OwnedDefaults : public DefaultValue_Traits< Resource >
     {
      typedef typename Disposer_Traits< Resource >::Type DisposerType;
     };

   class NondefaultValuesAreLive
     {
      public:
         NondefaultValuesAreLive( bool )           {}

         template < class Resource >
         static bool IsLive( const Resource& r )
         {
            return r != OwnedDefaults< Resource >::DefaultValue();
         }
     };

   template < class Resource, class DisposerType >
   struct LivelinessTraits
     {
      typedef NondefaultValuesAreLive LivelinessTest;
     };
   
   template < class Resource, class DisposerType >
   class DisposableResource: private DisposerType,
                             private Nucleus::LivelinessTraits< Resource, DisposerType >::LivelinessTest
     {
      private:
         typedef typename LivelinessTraits< Resource, DisposerType >::LivelinessTest TesterType;
         Resource resource;
      
         static Resource ImplicitlyConvert( const Resource& r )      { return r; }

         const TesterType& Tester() const      { return *this; }
         TesterType& Tester()                  { return *this; }
         
      public:
         DisposableResource()
           : DisposerType(),
             TesterType( false ),
             resource( OwnedDefaults< Resource >::DefaultValue() )
           {}
         
         DisposableResource( const Resource& r, bool seized )
           : DisposerType(),
             TesterType( seized ),
             resource( r )
           {}

         DisposableResource( const Resource& r, const DisposerType& disposer )
           : DisposerType( disposer ),
             TesterType( true ),
             resource( r )
           {}

         template< class R >
         DisposableResource( const DisposableResource<R,DisposerType>& r )
           : DisposerType( r.Disposer() ),
             TesterType( r.IsLive() ),
             resource( ImplicitlyConvert( r.Get() ) )
           {}
            
         void Swap( DisposableResource& r )
           {
            std::swap( Disposer(), r.Disposer() );
            std::swap( Tester(), r.Tester() );
            std::swap( resource,   r.resource   );
           }

         DisposerType const& Disposer() const      { return *this; }
         DisposerType      & Disposer()            { return *this; }
         
         bool IsLive() const                       { return Tester().IsLive( Get() ); }
         
         void Dispose() const                      { if ( IsLive() ) Disposer()( resource ); }

         const Resource& Get() const               { return resource; }
         operator const Resource&() const          { return resource; }
     };
   
   template < class Resource, class Disposer >
   class OwnershipTransfer
     {
      private:
         typedef DisposableResource< Resource, Disposer > Body;

         Body converted;
         
      public:
         explicit OwnershipTransfer( const Body& source )
           : converted( source )
           {}

         Body& operator*()       { return converted; }
     };
   
   template < class Resource, class Disposer = typename OwnedDefaults<Resource>::DisposerType >
   class Owned
     {
      private:
         typedef DisposableResource< Resource, Disposer > Body;
         typedef OwnershipTransfer< Resource, Disposer > Transfer;
         
         Body body;
         
         class Seizing {};
         explicit Owned( Seizing, const Resource& r ) : body( r, true ) {}
         explicit Owned( Seizing, const Body& r )     : body( r )       {}
         
      public:                                       
         Owned()                                      : body()    {}
         Owned( Owned& s )                            : body()    { Swap(  s ); }
         Owned( Transfer s )                          : body()    { std::swap( body, *s ); }
         
         template< class R >
         Owned( Owned<R,Disposer>& s )                : body( s.ReleaseWithDisposer() )  {}
         
         ~Owned()                                     { body.Dispose(); }
         
         Owned& operator=( Owned& s )                 { Owned old( *this ); Swap( s );       return *this; }
         Owned& operator=( Transfer s )               { Owned old( *this ); body.Swap( *s ); return *this; }
         
         template< class R > 
         Owned& operator=( Owned<R,Disposer>& s )     { Owned old( *this ); body = s.ReleaseWithDisposer(); return *this; }
         
         template < class R >
         operator Owned<R,Disposer>()                 { return Owned<R,Disposer>::Seize( ReleaseWithDisposer() ); }

         template < class R >
         operator OwnershipTransfer<R,Disposer>()     { return OwnershipTransfer<R,Disposer>( ReleaseWithDisposer() ); }
         
         void Swap( Owned& s )                        { body.Swap( s.body ); }
         
         const Resource& Get() const                  { return body.Get(); }
         operator const Resource&() const             { return body; }
         
         Disposer const& GetDisposer() const          { return body.Disposer(); }
         Disposer      & GetDisposer()                { return body.Disposer(); }
         
         static Owned Seize( const Body& r )          { return Owned( Seizing(), r ); }
         static Owned Seize( const Resource& r )      { return Owned( Seizing(), r ); }
         static Owned Seize( const Resource& r, const Disposer& d )      { return Seize( Body( r, d ) ); }
         
         Body ReleaseWithDisposer()
           {
            Body released;
            released.Swap( body );
            return released;
           }

         Resource Release()                           { return ReleaseWithDisposer().Get(); }
         
         void Reset()                                 { Owned empty;  Swap( empty ); }
         
         // For people who are used to the lowercase std::auto_ptr members:
            void swap( Owned& s )                     { Swap( s ); }
            const Resource& get() const               { return Get(); }
            Resource release()                        { return Release(); }
            void reset()                              { Reset(); }
     };

   template < class Resource, class Disposer >
   struct ConvertInputTraits< Owned< Resource, Disposer > >
     {
      typedef Resource ConverterInputType;
     };
   
   // This disposer makes Owned<Foo *> similar to std::auto_ptr<Foo>.  I generally
   // recommend std::auto_ptr<Foo> instead -- it helps you get along with the rest
   // of the world.  But sometimes uniformity is more important.
      struct DisposeWithDelete
        {
         template < class T >
         void operator()( T *p ) const
           {
            delete p;
           }
        };

   // This is for arrays.  The whole class is templated because array conversions are bad.
      template < class T >
      struct DisposeWithArrayDelete: public std::unary_function< T*, void >
        {
         void operator()( T *p ) const
           {
            delete[] p;
           }
        };
  }

#endif
