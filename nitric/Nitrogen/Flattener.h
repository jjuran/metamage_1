// Flattener.h

#ifndef NITROGEN_FLATTENER_H
#define NITROGEN_FLATTENER_H

/*
   Some interfaces, like AppleEvent descriptors and Carbon Event parameters,
   move data around in a way that is untyped from a C++ perspective, instead
   treating type information as run-time data.

   The Nitrogen scheme for building type-safe interfaces on top of these
   type-unsafe interfaces has three parts: flatteners, getters, and putters.
   
   Getters and putters are responsible for the mechanics of moving bytes,
   but not for relating the bytes to C++ types.  Getters and putters put
   uniform faces on functions that produce or consume untyped data, like
   AEGetParamPtr or SetDragItemFlavorData.
   
   A putter is just a functor that operates on a byte range:

   class Putter
     {
      void operator()( const void *begin, const void *end ) const;
     };
   
   And a getter is roughly the opposite: it fills in a byte range.
   It can also measure the number of bytes available.
   
   class Getter
     {
      std::size_t size() const;
      void operator()( void *begin, void *end ) const;
     };
   
   Flatteners are responsible for rendering typed data as a sequence of
   bytes, and, despite the name, also for rendering sequences of bytes
   into typed data.  They use getters and putters to move the raw bytes.
   
   class Flattener
     {
      typedef P Put_Parameter;
      
      template < class Putter > void Put( Put_Parameter, Putter );
      
      typedef R Get_Result;
      
      template < class Getter > Get_Result Get( Getter );
     };
*/

namespace Nitrogen
  {
   template < class T >
   struct PodFlattener
     {
      typedef const T& Put_Parameter;
      
      template < class Putter > void Put( Put_Parameter toPut, Putter put )
        {
         put( &toPut, &toPut + 1 );
        }
      
      typedef T Get_Result;
      
      template < class Getter > Get_Result Get( Getter get )
        {
         Get_Result result;
         get( &result, &result + 1 );
         return result;
        }
     };
   
   template < class T >
   struct PodVectorFlattener
     {
      typedef const T& Put_Parameter;
      
      template < class Putter > void Put( Put_Parameter toPut, Putter put )
        {
         const typename T::value_type *begin = toPut.empty() ? 0 : &*toPut.begin();
         put( begin, begin + toPut.size() );
        }
      
      typedef T Get_Result;
      
      template < class Getter > Get_Result Get( Getter get )
        {
         Get_Result result( get.size() );
         typename T::value_type *begin = result.empty() ? 0 : &*result.begin();
         get( begin, begin + result.size() );
         return result;
        }
     };
   
   template < class T >
   struct StringFlattener: public PodVectorFlattener<T>
     {};
   
   template < class Converted, class BaseFlattener >
   class ConvertingFlattener
     {
      private:
         BaseFlattener base;
      
      public:
         typedef const Converted& Put_Parameter;
         
         template < class Putter > void Put( Put_Parameter toPut, Putter put )
           {
            base.Put( toPut, put );
           }
         
         typedef Converted Get_Result;
         
         template < class Getter > Get_Result Get( Getter get )
           {
            return base.Get( get );
           }
     };
   
   struct FixedFlattener
     {
      typedef double Put_Parameter;
      
      template < class Putter > void Put( Put_Parameter toPut, Putter put )
        {
         const Fixed fixed = DoubleToFixed( toPut );
         put( &fixed, &fixed + 1 );
        }
      
      typedef double Get_Result;
      
      template < class Getter > Get_Result Get( Getter get )
        {
         Fixed fixed;
         get( &fixed, &fixed + 1 );
         return FixedToDouble( fixed );
        }
     };
   
   struct NoData {};
   
   struct NoDataFlattener
     {
      typedef NoData Put_Parameter;
      
      template < class Putter > void Put( Put_Parameter toPut, Putter put )
        {
         put( 0, 0 );
        }
      
      typedef NoData Get_Result;
      
      template < class Getter > Get_Result Get( Getter get )
        {
         get( 0, 0 );  // So it has a chance to throw
         return NoData();
        }
     };
  }

#endif
