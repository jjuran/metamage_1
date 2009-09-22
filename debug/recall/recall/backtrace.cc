/*	============
 *	backtrace.cc
 *	============
 */

#include "recall/backtrace.hh"

// Standard C++
#include <algorithm>
#include <functional>
#include <string>

// Standard C/C++
#include <cstdio>

// Standard C
#include <string.h>

#ifdef __GNUC__
#include <cxxabi.h>
#endif

// Iota
#include "iota/decimal.hh"
#include "iota/hexidecimal.hh"
#include "iota/strings.hh"

// Recall
#include "recall/demangle.hh"
#include "recall/mach_o.hh"
#include "recall/macsbug_symbols.hh"
#include "recall/name_filter.hh"
#include "recall/traceback_tables.hh"


#ifndef TARGET_CPU_PPC
	#ifdef __POWERPC__
		#define TARGET_CPU_PPC 1
	#else
		#define TARGET_CPU_PPC 0
	#endif
#endif


namespace recall
{
	
	template < class ReturnAddr > struct demangler_traits;
	
#ifdef __MACOS__
	
	template <> struct demangler_traits< return_address_68k >
	{
		static std::string demangle( const std::string& name )
		{
			return demangle_MWC68K( name );
		}
	};
	
	template <> struct demangler_traits< return_address_cfm >
	{
		static std::string demangle( const std::string& name )
		{
			return demangle_MWCPPC( name );
		}
	};
	
#endif
	
#ifdef __GNUC__
	
	template <> struct demangler_traits< return_address_native >
	{
		static std::string demangle( const std::string& name )
		{
			std::string result = name;
			
			// s/:.*//;
			result.resize( std::find( result.begin(), result.end(), ':' ) - result.begin() );
			
			const char* mangled_name = result.c_str();
			
			if ( std::equal( mangled_name, mangled_name + 3, "__Z" ) )
			{
				++mangled_name;
			}
			
			if ( char* unmangled = abi::__cxa_demangle( mangled_name, NULL, NULL, NULL ) )
			{
				result = unmangled;
				
				free( unmangled );
				
				return result;
			}
			
			return result;
		}
	};
	
#endif
	
#ifdef __ELF__
	
	static const char* find_symbol_name( return_address_native )
	{
		return NULL;
	}
	
#endif
	
	template < class SymbolPtr >
	static inline std::string get_name_from_symbol_pointer( SymbolPtr symbol )
	{
		return symbol != NULL ? get_symbol_string( symbol ) : "???";
	}
	
	template < class ReturnAddr >
	static inline std::string get_symbol_name( ReturnAddr addr )
	{
		return get_name_from_symbol_pointer( find_symbol_name( addr ) );
	}
	
	template < class ReturnAddr >
	static std::string get_demangled_symbol_name( ReturnAddr addr )
	{
		std::string name = get_symbol_name( addr );
		
		try
		{
			return demangler_traits< ReturnAddr >::demangle( name );
		}
		catch ( ... )
		{
			return name;
		}
	}
	
	
	struct call_info
	{
		const void*  frame_pointer;
		const void*  return_address;
		const char*  arch;
		std::string  demangled_name;
	};
	
	static call_info get_call_info_from_return_address( const frame_data& call )
	{
		call_info result;
		
		result.frame_pointer  = call.frame_pointer;
		result.return_address = call.addr_native;
		
	#ifdef __MACOS__
		
		result.arch           = call.is_cfm ? "ppc" : "68k";
		result.demangled_name = call.is_cfm ? get_demangled_symbol_name( call.addr_cfm    )
		                                    : get_demangled_symbol_name( call.addr_native );
		
		filter_symbol( result.demangled_name );
		
	#else
		
		result.arch           = TARGET_CPU_PPC ? "ppc" : "x86";
		result.demangled_name = get_demangled_symbol_name( call.addr_native );
		
	#endif
		
		return result;
	}
	
	static void make_report_for_call( const call_info&  info,
	                                  std::string&      result )
	{
		const size_t old_size = result.size();
		
		result.append( STR_LEN( ": [0x12345678 <0x12345678|xyz>] " ) );
		
		char* frame_buf  = &result[ old_size + STRLEN( ": [0x"             ) ];
		char* return_buf = &result[ old_size + STRLEN( ": [0x12345678 <0x" ) ];
		
		iota::inscribe_n_hex_digits( frame_buf,  (long) info.frame_pointer,  8 );
		iota::inscribe_n_hex_digits( return_buf, (long) info.return_address, 8 );
		
		strncpy( &*result.end() - STRLEN( "xyz>] " ), info.arch, 3 );
		
		result += info.demangled_name;
		result += "\n";
	}
	
	static std::string make_report_from_call_chain( std::vector< call_info >::const_iterator  begin,
	                                                std::vector< call_info >::const_iterator  end )
	{
		const unsigned n_frames = end - begin;
		
		const unsigned nth_offset = n_frames - 1;
		
		const char* spaces = "         ";
		//                    123456789  // 9 spaces ought to be enough
		
		const unsigned nth_magnitude = iota::decimal_magnitude( nth_offset );
		
		unsigned offset = 0;
		
		std::string result;
		
		// It's important to use < instead of != if we might skip past the end
		for ( std::vector< call_info >::const_iterator it = begin;  it < end;  ++it, ++offset )
		{
			const unsigned magnitude = iota::decimal_magnitude( offset );
			
			const unsigned n_spaces = nth_magnitude - magnitude;
			
			result.append( spaces, n_spaces );
			
			result.resize( result.size() + magnitude );
			
			iota::inscribe_unsigned_decimal_backwards( offset, &*result.end() );
			
			const call_info& info = *it;
			
			make_report_for_call( info, result );
		}
		
		result += "\n";
		
		return result;
	}
	
	std::string make_report_from_stack_crawl( std::vector< frame_data >::const_iterator  begin,
	                                          std::vector< frame_data >::const_iterator  end )
	{
		std::vector< call_info > call_chain;
		
		call_chain.resize( end - begin );
		
		std::transform( begin,
		                end,
		                call_chain.begin(),
		                std::ptr_fun( get_call_info_from_return_address ) );
		
		return make_report_from_call_chain( call_chain.begin(), call_chain.end() );
	}
	
	debugging_context::debugging_context()
	:
		its_stack_crawl( make_stack_crawl() )
	{
	}
	
}

