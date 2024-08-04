/*	============
 *	backtrace.cc
 *	============
 */

#include "recall/backtrace.hh"

// Standard C++
#include <algorithm>

// Standard C
#include <string.h>

#ifdef __GNUC__
#include <cxxabi.h>
#endif

// Iota
#include "iota/strings.hh"

// gear
#include "gear/hexadecimal.hh"
#include "gear/inscribe_decimal.hh"

// plus
#include "plus/var_string.hh"

// Recall
#include "recall/demangle.hh"
#include "recall/mach_o.hh"
#include "recall/macsbug_symbols.hh"
#include "recall/name_filter.hh"
#include "recall/symbol_locator.hh"
#include "recall/traceback_tables.hh"


#if defined( __i386__ )  ||  defined( __x86_64__ )
#define CONFIG_ODD_ADDRESSES  1
#else
#define CONFIG_ODD_ADDRESSES  0
#endif

#ifndef TARGET_CPU_PPC
	#ifdef __POWERPC__
		#define TARGET_CPU_PPC 1
	#else
		#define TARGET_CPU_PPC 0
	#endif
#endif

#ifndef CONFIG_DEMANGLING
	#ifdef __MC68K__
		#define CONFIG_DEMANGLING  CONFIG_DEBUGGING
	#else
		#define CONFIG_DEMANGLING  1
	#endif
#endif


namespace recall
{
	
	template < class ReturnAddr > struct demangler_traits;
	
#ifdef __MACOS__
	
	template <> struct demangler_traits< return_address_68k >
	{
		static void demangle( enough_string& result, const char* name )
		{
			demangle_MWC68K( result, name );
		}
	};
	
	template <> struct demangler_traits< return_address_cfm >
	{
		static void demangle( enough_string& result, const char* name )
		{
			demangle_MWCPPC( result, name );
		}
	};
	
#endif
	
#ifdef __GNUC__
	
	template <> struct demangler_traits< return_address_native >
	{
		static void demangle( plus::var_string& result, const char* name )
		{
			result = name;
			
			// s/:.*//;
			result.resize( std::find( result.begin(), result.end(), ':' ) - result.begin() );
			
			const char* mangled_name = result.c_str();
			
			if ( std::equal( mangled_name, mangled_name + 3, "__Z" ) )
			{
				++mangled_name;
			}
			
			if ( char* unmangled = abi::__cxa_demangle( mangled_name, NULL, NULL, NULL ) )
			{
				result.assign( unmangled, strlen( unmangled ), vxo::delete_free );
			}
		}
	};
	
#endif
	
#if defined( __ELF__ )  ||  defined( __CYGWIN__ )  ||  defined( __INTERIX )
	
	static const char* find_symbol_name( return_address_native )
	{
		return NULL;
	}
	
#endif
	
	template < class SymbolPtr >
	static inline plus::string get_name_from_symbol_pointer( SymbolPtr symbol )
	{
		if ( symbol )
		{
			return get_symbol_string( symbol );
		}
		
		return "???";
	}
	
	template < class ReturnAddr >
	static inline plus::string get_symbol_name( ReturnAddr addr )
	{
		return get_name_from_symbol_pointer( find_symbol_name( addr ) );
	}
	
	static const char* locate_symbol_name( const void* address )
	{
		if ( const symbol_locator locate = get_symbol_locator() )
		{
			return locate( address );
		}
		
		return NULL;
	}
	
	template < class ReturnAddr >
	static plus::string get_demangled_symbol_name( ReturnAddr addr )
	{
		if ( ! CONFIG_ODD_ADDRESSES  &&  (long) addr & 1 )
		{
			return "<<odd address>>";
		}
		
		const char* located_name = locate_symbol_name( addr );
		
		plus::string name = located_name ? plus::string( located_name )
		                                 : get_symbol_name( addr );
		
		if ( CONFIG_DEMANGLING )
		{
		#ifdef __GNUC__
			
			plus::var_string result;
			
		#else
			
			char buffer[ 512 ];
			
			enough_string result( buffer, sizeof buffer );
			
		#endif
			
			try
			{
				demangler_traits< ReturnAddr >::demangle( result, name.c_str() );
				
			#ifdef __GNUC__
				
				return move( result );
				
			#else
				
				return plus::string( result.data(), result.size() );
				
			#endif
			}
			catch ( ... )
			{
			}
		}
		
		return name;
	}
	
	
	struct call_info
	{
		const void*   frame_pointer;
		const void*   return_address;
		const char*   arch;
		plus::string  demangled_name;
	};
	
	static call_info get_call_info_from_return_address( const frame_data& call )
	{
		call_info result;
		
		result.frame_pointer  = call.frame_pointer;
		result.return_address = call.addr_native;
		
	#ifdef __MACOS__
		
		const long is_cfm = (long) result.frame_pointer & 0x1;
		
		if ( is_cfm )
		{
			--*(long*) &result.frame_pointer;
		}
		
		result.arch           = is_cfm ? "ppc" : "68k";
		result.demangled_name = is_cfm ? get_demangled_symbol_name( call.addr_cfm    )
		                               : get_demangled_symbol_name( call.addr_native );
		
		plus::var_string demangled_name;
		
		result.demangled_name.swap( demangled_name );
		
		filter_symbol( demangled_name );
		
		result.demangled_name.swap( demangled_name );
		
	#else
		
		result.arch           = TARGET_CPU_PPC ? "ppc" : "x86";
		result.demangled_name = get_demangled_symbol_name( call.addr_native );
		
	#endif
		
		locate_symbol_name( NULL );  // signal that we're done with the string
		
		return result;
	}
	
	static void make_report_for_call( plus::var_string&  result,
	                                  const call_info&   info )
	{
		const size_t old_size = result.size();
		
		result.append( STR_LEN( ": [0x12345678 <0x12345678|xyz>] " ) );
		
		char* frame_buf  = &result[ old_size + STRLEN( ": [0x"             ) ];
		char* return_buf = &result[ old_size + STRLEN( ": [0x12345678 <0x" ) ];
		
		gear::encode_32_bit_hex( (long) info.frame_pointer,  frame_buf  );
		gear::encode_32_bit_hex( (long) info.return_address, return_buf );
		
		strncpy( &*result.end() - STRLEN( "xyz>] " ), info.arch, 3 );
		
		result += info.demangled_name;
		result += "\n";
	}
	
	void make_report_from_stack_crawl( plus::var_string&  result,
	                                   const frame_data*  begin,
	                                   const frame_data*  end )
	{
		const unsigned n_frames = end - begin;
		
		const unsigned nth_offset = n_frames - 1;
		
		const char* spaces = "         ";
		//                    123456789  // 9 spaces ought to be enough
		
		const unsigned nth_magnitude = gear::decimal_magnitude( nth_offset );
		
		unsigned offset = 0;
		
		// It's important to use < instead of != if we might skip past the end
		for ( const frame_data* it = begin;  it < end;  ++it, ++offset )
		{
			const unsigned magnitude = gear::decimal_magnitude( offset );
			
			const unsigned n_spaces = nth_magnitude - magnitude;
			
			result.append( spaces, n_spaces );
			
			const std::size_t size = result.size();
			
			result.resize( size + magnitude );
			
			gear::fill_unsigned_decimal( offset, &result[ size ], magnitude );
			
			const call_info info = get_call_info_from_return_address( *it );
			
			make_report_for_call( result, info );
		}
		
		result += "\n";
	}
	
	debugging_context::debugging_context()
	{
		its_n_frames = make_stack_crawl( its_stack_crawl, frame_capacity );
	}
	
}
