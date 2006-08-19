// Flattener.h

#ifndef NUCLEUS_FLATTENER_H
#define NUCLEUS_FLATTENER_H
/*
   Some interfaces, like AppleEvent descriptors and Carbon Event parameters,
   move data around in a way that is untyped from a C++ perspective, instead
   treating type information as run-time data.

   The Nucleus scheme for building type-safe interfaces on top of these
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

// Standard C++
#include <memory>

// Nucleus
#include "Nucleus/Owned.h"


namespace Nucleus
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
      
      typedef Put_Parameter Parameter;
      typedef Get_Result    Result;
      
      static const bool hasStaticSize = true;
      typedef T Buffer;
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
         Get_Result result;
         result.resize( get.size() );
         
         typename T::value_type *begin = result.empty() ? 0 : &*result.begin();
         get( begin, begin + result.size() );
         return result;
        }
      
      typedef Put_Parameter Parameter;
      typedef Get_Result    Result;
      
      static const bool hasStaticSize = false;
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
         
         typedef Put_Parameter Parameter;
         typedef Get_Result    Result;
      
         static const bool hasStaticSize = BaseFlattener::hasStaticSize;
         
         typedef typename BaseFlattener::Buffer Buffer;
     };
   
	template < class Ownable >
	struct SeizingPODFlattener
	{
		typedef Ownable Put_Parameter;
		
		template < class Putter > void Put( Put_Parameter toPut, Putter put )
		{
			put( &toPut, &toPut + 1 );
		}
		
		typedef Nucleus::Owned< Ownable > Get_Result;
		
		template < class Getter > Get_Result Get( Getter get )
		{
			Get_Result result;
			
			get( &result, &result + 1 );
			
			return Nucleus::Owned< Ownable >::Seize( result );
		}
		
		static const bool hasStaticSize = true;
		
		typedef Ownable Buffer;
	};
	
	template < class T, std::size_t (*SizeOf)( const T& ) >
	struct VariableLengthPodFlattener
	{
		typedef const T& Put_Parameter;
		
		template < class Putter > void Put( Put_Parameter toPut, Putter put )
		{
			const T* begin = &toPut;
			const std::size_t size = SizeOf( toPut );
			
			put( begin, begin + size );
		}
		
		typedef std::auto_ptr< T > Get_Result;
		
		template < class Getter > Get_Result Get( Getter get )
		{
			const std::size_t size = get.size();
			
			Get_Result result = std::auto_ptr< T >( static_cast< T* >( ::operator new( size ) ) );
			
			T* begin = result.get();
			
			get( begin, begin + size );
			
			return result;
		}
		
		typedef Put_Parameter Parameter;
		typedef Get_Result    Result;
		
		static const bool hasStaticSize = false;
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
      
      typedef Put_Parameter Parameter;
      typedef Get_Result    Result;
      
      static const bool hasStaticSize = true;
     };
  }

#endif
