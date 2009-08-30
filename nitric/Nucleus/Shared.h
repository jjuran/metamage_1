// Nucleus/Shared.h
// ----------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2002-2007 by Lisa Lippincott and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NUCLEUS_SHARED_H
#define NUCLEUS_SHARED_H

#ifndef NUCLEUS_OWNED_H
#include "Nucleus/Owned.h"
#endif

#include <algorithm>

namespace Nucleus
  {
   class Share
     {
      private:
         mutable const Share *previous;
         mutable const Share *next;
         
      public:
         Share()
           : previous( this ),
             next( this )
           {}

         Share( const Share& b )
           : previous( &b ),
             next( b.next )
           {
            previous->next = this;
            next->previous = this;
           }
         
         ~Share()
           {
            previous->next = next;
            next->previous = previous;
           }
         
         Share& operator=( const Share& b )
           {
            previous->next = next;
            next->previous = previous;
            next = b.next;
            previous = next->previous;
            previous->next = this;
            next->previous = this;
            return *this;
           }
         
         bool Sole() const
           {
            return previous == this;
           }
     };
   
   template < class Resource, class Disposer >
   class Shared;
   
   /*
      CodeWarrior 7 has bugs preventing a Shared<R1,D> from accessing
      the members of Shared<R2,D>.  This hack gets around those bugs.
   */
   
   class SharedAccessHack
     {
      protected:
         template < class R, class D >
         const DisposableResource<R,D>& GetBody( const Shared<R,D>& s )
           {
            return s.body;
           }

         template < class R, class D >
         const Share& GetShare( const Shared<R,D>& s )
           {
            return s.share;
           }
     };

   template < class Resource, class Disposer = typename OwnedDefaults<Resource>::DisposerType >
   class Shared: private SharedAccessHack
     {
      private:
         friend class SharedAccessHack;
         
         typedef DisposableResource< Resource, Disposer > Body;

         Body body;
         Share share;
         
      public:
         Shared()                                           : body()                            {}

         template < class R >
         Shared( Owned<R,Disposer> s )                      : body( s.ReleaseWithDisposer() )   {}

         template < class R >
         Shared( const Shared<R,Disposer>& s )              : body( GetBody(s) ), share( GetShare(s) )  {}
         
         // Plus the implicit copy constructor
         
         ~Shared()                                          { if ( share.Sole() ) body.Dispose(); }

         Shared& operator=( const Shared& s )               { Shared empty; Swap( empty ); body = GetBody(s); share = GetShare(s); return *this; }
         
         template < class R >
         Shared& operator=( const Shared<R,Disposer>& s )   { Shared empty; Swap( empty ); body = GetBody(s); share = GetShare(s); return *this; }
         
         void Swap( Shared& s )                             { std::swap( body, s.body ); std::swap( share, s.share ); }

         const Resource& Get() const                        { return body.Get(); }
         operator const Resource&() const                   { return body; }

         bool Sole() const                                  { return share.Sole(); }

         Owned<Resource,Disposer> Unshare();
         
         void Reset()                                       { Shared empty;  Swap( empty ); }
         
         // For people who are used to the lowercase std::auto_ptr members:
            void swap( Shared& s )                          { Swap( s ); }
            const Resource& get() const                     { return Get(); }
            void reset()                                    { Reset(); }
     };

   class Unshare_Failed {};

   template < class Resource, class Disposer >
   Owned<Resource, Disposer> Shared<Resource, Disposer>::Unshare()
     {
      if ( !Sole() )
         throw Unshare_Failed();

      Body released;
      released.Swap( body );

      return Owned<Resource, Disposer>::Seize( released );
     }
  }

#endif
