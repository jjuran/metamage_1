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
		typedef const T& Put_Parameter;
		
		template < class Putter >
		static void Put( Put_Parameter param, const Putter& putter )
		{
			const T* begin = &param;
			
			const std::size_t size = size_of( param );
			
			putter( begin, begin + size );
		}
		
		typedef std::auto_ptr< T > Get_Result;
		
		template < class Getter >
		static Get_Result Get( const Getter& getter )
		{
			const std::size_t size = getter.size();
			
			Get_Result result = std::auto_ptr< T >( static_cast< T* >( ::operator new( size ) ) );
			
			T* begin = result.get();
			
			getter( begin, begin + size );
			
			return result;
		}
		
		typedef Put_Parameter Parameter;
		typedef Get_Result  Result;
		
		static const bool hasStaticSize = false;
		struct Buffer {};
	};
	
}

#endif

