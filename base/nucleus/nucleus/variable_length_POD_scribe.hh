/*
	nucleus/variable_length_POD_scribe.hh
	-------------------------------------
*/

#ifndef NUCLEUS_VARIABLELENGTHPODSCRIBE_HH
#define NUCLEUS_VARIABLELENGTHPODSCRIBE_HH

// Standard C++
#include <memory>


namespace nucleus
{
	
	template < class T, std::size_t (*size_of)( const T& ) >
	struct variable_length_POD_scribe
	{
		typedef const T&            argument_type;
		typedef std::auto_ptr< T >  result_type;
		
		template < class Putter >
		static void Put( argument_type param, const Putter& putter )
		{
			const T* begin = &param;
			
			const std::size_t size = size_of( param );
			
			putter( begin, begin + size );
		}
		
		template < class Getter >
		static result_type Get( const Getter& getter )
		{
			const std::size_t size = getter.size();
			
			result_type result = std::auto_ptr< T >( static_cast< T* >( ::operator new( size ) ) );
			
			T* begin = result.get();
			
			getter( begin, begin + size );
			
			return result;
		}
		
		static const std::size_t static_size = 0;
	};
	
}

#endif

