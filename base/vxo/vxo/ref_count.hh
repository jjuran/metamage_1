/*
	ref_count.hh
	------------
*/

#ifndef VXO_REFCOUNT_HH
#define VXO_REFCOUNT_HH

#ifndef __RELIX__
#include <boost/atomic.hpp>
#define GCC_ASM_IS_DEFECTIVE 0  // begin optimistic
#endif

#if defined( __APPLE__ )  &&  ! defined( __LP64__ )
#  ifdef __GNUC__
#    if __GNUC__ < 4  ||  (__GNUC__ == 4  &&  __GNUC_MINOR__ == 0 )
#      undef  GCC_ASM_IS_DEFECTIVE
#      define GCC_ASM_IS_DEFECTIVE 1  // "error: impossible constraint in 'asm'"
#      include <CoreServices/CoreServices.h>
#      undef check
#    endif
#  endif
#endif

namespace vxo
{
	
#ifdef __RELIX__
	
	// MacRelix threading is cooperative and doesn't need atomic types.
	typedef unsigned long reference_count_t;
	
#elif GCC_ASM_IS_DEFECTIVE
	
	class OTAtomicInt32
	{
		private:
			typedef unsigned long Int;
			
			SInt32 its_x;
		
		public:
			OTAtomicInt32( Int x = 0 ) : its_x( x )
			{
			}
			
			operator Int() const  { return its_x; }
			
			Int operator++()
			{
				return OTAtomicAdd32( 1, &its_x );
			}
			
			Int operator--()
			{
				return OTAtomicAdd32( -1, &its_x );
			}
	};
	
	typedef OTAtomicInt32 reference_count_t;
	
#else
	
	typedef boost::atomic< unsigned long > reference_count_t;
	
#endif
	
	template < class T > struct destroyer
	{
		static void apply( T* x )
		{
			delete x;
		}
	};
	
	template < class T >
	inline void destroy( T* x )
	{
		destroyer< T >::apply( x );
	}
	
	class ref_count_base
	{
		private:
			mutable reference_count_t its_n;
			
			// Non-copyable
			ref_count_base           ( const ref_count_base& );
			ref_count_base& operator=( const ref_count_base& );
			
		protected:
			// Protected constructor to prevent slicing
			ref_count_base() : its_n( 0 )
			{
			}
			
			unsigned long release() const
			{
				return --its_n;
			}
		
		public:
			friend unsigned long intrusive_ptr_ref_count( const ref_count_base* count )
			{
				return count->its_n;
			}
		
		private:
			friend void intrusive_ptr_add_ref( const ref_count_base* count )
			{
				++count->its_n;
			}
	};
	
	template < class Derived >
	class ref_count : public ref_count_base
	{
		private:
			// Hide the protected release() from view
			unsigned long release() const
			{
				return ref_count_base::release();
			}
			
			friend void intrusive_ptr_release( const Derived* derived )
			{
				if ( derived->release() == 0 )
				{
					destroy( derived );
				}
			}
		
		protected:
			// Protected destructor to prevent slicing
			~ref_count()
			{
			}
	};
	
}

#undef GCC_ASM_IS_DEFECTIVE

#endif
