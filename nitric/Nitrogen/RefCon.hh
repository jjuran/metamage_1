// Nitrogen/RefCon.hh
// ------------------

// Part of the Nitrogen project.
//
// Written 2002-2003 by Lisa Lippincott.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef NITROGEN_REFCON_HH
#define NITROGEN_REFCON_HH

namespace Nitrogen
  {
   template < class Desired > struct RefCon_Traits;
   
   template <> struct RefCon_Traits< long >
     {
      static long Get( const void *value )       { return reinterpret_cast<long>( value ); }
      static const void *Make( long value )      { return reinterpret_cast<const void *>( value ); }
     };
   
   template < class T > struct RefCon_Traits< const T* >
     {
      static const T* Get( const void *value )   { return static_cast<const T*>( value ); }
      static const void *Make( const T *value )  { return value; }
     };

   template < class T > struct RefCon_Traits< T* >
     {
      static T* Get( const void *value )         { return const_cast<T*>( static_cast<const T*>( value ) ); }
      static const void *Make( const T *value )  { return value; }
     };
   
   template < class T > struct RefCon_Traits< const T& >
     {
      static const T& Get( const void *value )   { return *static_cast<const T*>( value ); }
      static const void *Make( const T& value )  { return &value; }
     };

   template < class T > struct RefCon_Traits< T& >
     {
      static T& Get( const void *value )         { return *const_cast<T*>( static_cast<const T*>( value ) ); }
      static const void *Make( const T& value )  { return &value; }
     };
   
     
   class RefCon
     {
      private:
         const void *value;
      
      public:
         RefCon()                         : value( 0 )  {}
         RefCon( const void *r )          : value( r )  {}
         RefCon( long r )                 : value( reinterpret_cast<const void *>( r ) )  {}
 
         template < class T >
         T Get() const                    { return RefCon_Traits<T>::Get( value ); }
         
         template < class T >
         static RefCon Make( T t )        { return RefCon_Traits<T>::Make( t ); }
         
         operator long() const            { return Get<long>(); }

         template < class T >
         operator const T*() const        { return static_cast<const T*>( value ); }

         template < class T >
         operator T*() const              { return static_cast<T*>( const_cast<void*>( value ) ); }
         
         bool operator==( const RefCon& b ) const  { return value == b.value; }
         bool operator!=( const RefCon& b ) const  { return value != b.value; }
     };
  }

#endif
