/*
	pragma.cc
	---------
*/

#include "pragma.hh"

// Standard C++
#include <algorithm>

// mxcpp
#include "config.hh"
#include "exception.hh"
#include "pragma_once.hh"


namespace tool
{
	
	/*
		Pragma categories:
		
		* one-to-one option/pragma, no effect on preprocessing except via __option()
		* alias, e.g pointers_in_D0
		* read-only options (preprocess, precompile)
		* #pragma once
		
		DONE:
			recognize but ignore valid but insignificant pragmata (e.g. codegen, compiler diagnostics)
			track one-to-one option/pragma pairs (__option(foo) reflects #pragma foo)
			__option(little_endian) --> 0
			__option(preprocess) --> 1  // How could a source file *not* be preprocessed?
			__option(precompile)
			#pragma once
		
		PUNT:
			#pragma push/pop
			__option(linksym)  // seriously, WTF?
			ANSI_strict affects certain things
				line comments in C
				trailing garbage after #endif
			cplusplus affects cpp behavior
				operator aliases (bit_and, etc.)
				digraphs
				line comments
			trigraphs affects cpp behavior
			mpwc_newline might affect evaluation of character constants
				e.g. '\n' == 10
		
		ALSO:
			pragma and option names are case-sensitive
			direct_to_SOM is an error; it's spelled direct_to_som.
			d0_pointers takes on|off|reset, in spite of being repeatedly
				misdocumented as taking nothing.
	*/
	
	/*
		Non-pragma options
		
			linksym
			little_endian
			precompile
			preprocess
			
		Misc, no options, special ones are marked
			
			options align=(power|native|packed|reset)
			mark <comment>
		*	once [ on | off ]
		*	push
		*	pop
			precompile_target    <filename>
			segment              <name>
			unused( <var>, <var>, ... )
		
	68K codegen, no options
		
		68K	far_code
		68K	near_code
		68K	smart_code
		68K	pointers_in_A0  ->  d0_pointers off
		68K	pointers_in_D0  ->  d0_pointers on
		68K	parameter            <return-reg> <function-name>(<param-regs>)
		
	PPC codegen, no options
		
		PPC	optimization_level   1 | 2 | 3 | 4
		PPC	scheduling           601 | 603 | 604 | on | off | reset
		
	SOM stuff, no options
		
		C++	SOMCallStyle         OIDL | IDL
		C++	SOMClassVersion( <class>, <major>, <minor> )
		C++	SOMMetaClass( <class>, <metaclass> )
		C++	SOMReleaseOrder( <f1>, <f2>, ... <fN> )
		
	Windows cruft (no options)
		
		W32	code_seg(<name>)
		W32	data_seg(<name>)
		W32	function( <f1>, <f2>, ... )
		W32	init_seg( compiler | lib | user | "<name>" )
		W32	inline_depth(<n>)
		W32	pack( [ <n> | push, <n> | pop )
		W32	warning( <spec> : <list> )
		
	CFM codegen, matching options
		
		CFM	export               on | off | reset | list <names>
		CFM	import               on | off | reset | list <names>
		CFM	internal             on | off | reset | list <names>
		
	General, matching options -- special ones are marked
		
		68K	a6frames             on | off | reset
			align_array_members  on | off | reset
	*		ANSI_strict          on | off | reset
			ARM_conform          on | off | reset
			auto_inline          on | off | reset
		C++	bool                 on | off | reset
		pch	check_header_flags   on | off | reset
		68K	code68020            on | off | reset
		Mgc	code68349            on | off | reset
		68K	code68881            on | off | reset
	*		cplusplus            on | off | reset
			cpp_extensions       on | off | reset
		68K	d0_pointers          on | off | reset
		C++	direct_destruction   on | off | reset
		C++	direct_to_som        on | off | reset
		PPC	disable_registers    on | off | reset
			dont_inline          on | off | reset
			dont_reuse_strings   on | off | reset
			enumsalwaysint       on | off | reset
		C++	exceptions           on | off | reset
			extended_errorcheck  on | off | reset
		68K	far_data             on | off | reset
		68K	far_strings          on | off | reset
		68K	far_vtables          on | off | reset
		68K	force_active         on | off | reset
		68K	fourbyteints         on | off | reset
		PPC	fp_contract          on | off | reset
		PPC	global_optimizer     on | off | reset
		68K	IEEEdoubles          on | off | reset
			ignore_oldstyle      on | off | reset
		CFM	lib_export           on | off | reset
			longlong             on | off | reset
			longlong_enums       on | off | reset
			macsbug              on | off | reset
			oldstyle_symbols     on | off | reset
		68K	mpwc                 on | off | reset
	*		mpwc_newline         on | off | reset
			mpwc_relax           on | off | reset
		68K	no_register_coloring on | off | reset
			only_std_keywords    on | off | reset
			optimize_for_size    on | off | reset
		68K	pcrelstrings         on | off | reset
		PPC	peephole             on | off | reset
			pool_strings         on | off | reset
			profile              on | off | reset
		PPC	readonly_strings     on | off | reset
			require_prototypes   on | off | reset
			RTTI                 on | off | reset
			side_effects         on | off | reset
		C++	SOMCallOptimization  on | off | reset
		C++	SOMCheckEnvironment  on | off | reset
			static_inlines       on | off | reset
			sym                  on | off | reset
		PPC	toc_data             on | off | reset
	*		trigraphs            on | off | reset
		PPC	traceback            on | off | reset
			unsigned_char        on | off | reset
			warn_emptydecl       on | off | reset
			warning_errors       on | off | reset
			warn_extracomma      on | off | reset
			warn_hidevirtual     on | off | reset
			warn_illpragma       on | off | reset
			warn_possunwant      on | off | reset
			warn_unusedarg       on | off | reset
			warn_unusedvar       on | off | reset
			
	*/
	
	static inline long undef_stack()
	{
		return 0;
	}
	
	static inline long empty_stack()
	{
		return 1;
	}
	
	static inline bool stack_defined( long stack )
	{
		return stack != 0;
	}
	
	static inline bool stack_undefined( long stack )
	{
		return stack == 0;
	}
	
	static inline bool stack_nonnull( long stack )
	{
		return stack & -2L;
	}
	
	static inline bool stack_null( long stack )
	{
		return !stack_nonnull( stack );
	}
	
	static inline bool stack_empty( long stack )
	{
		return stack == 1;
	}
	
	static inline bool stack_full( long stack )
	{
		return stack < 0;
	}
	
	static inline bool stack_top( long stack )
	{
		return stack & 1;
	}
	
	static int stack_height( long stack )
	{
		int height = -1;
		
		while ( stack )
		{
			stack >>= 1;
			
			++height;
		}
		
		return height;
	}
	
	static int stack_height_0( long stack )
	{
		int height = stack_height( stack );
		
		return height < 0 ? 0 : height;
	}
	
	static inline long popped_stack( long stack )
	{
		return stack >> 1;
	}
	
	static inline void pop_stack( long& stack )
	{
		stack >>= 1;
	}
	
	static inline long pushed_stack( long stack, bool boolean )
	{
		return stack << 1 | boolean;
	}
	
	static inline void push_stack( long& stack, bool boolean )
	{
		stack = pushed_stack( stack, boolean );
	}
	
	
	typedef void (*pragma_handler)( const plus::string& param );
	
	enum
	{
		only_68k          = 1,
		only_cplusplus    = 2,
		only_precompiling = 4,
	};
	
	static long global_config = 0;
	
	struct option_t
	{
		const char*  name;
		long         initial;
		bool*        dependency;
		long         stack;
	};
	
	static inline bool operator==( const option_t& o, const plus::string& s )
	{
		return &o.name[1] == s;
	}
	
	static option_t the_options[] =
	{
		{ " ANSI_strict",       false                            },
		{ " IEEEdoubles",       true,  &global_config_68k        },
		{ " bool",              true,  &global_config_cplusplus  },
		{ " code68020",         false, &global_config_68k        },
		{ " code68881",         false, &global_config_68k        },
		{ "@cplusplus",         false, &global_config_cplusplus  },
		{ ";direct_destruction"                                  },
		{ " enumsalwaysint",    true                             },
		{ " exceptions",        true,  &global_config_cplusplus  },
		{ " export",            false                            },
		{ " far_data",          true,  &global_config_68k        },
		{ " far_strings",       true,  &global_config_68k        },
		{ " far_vtables",       true,  &global_config_68k        },
		{ " force_active",      false                            },
		{ " fourbyteints",      true,  &global_config_68k        },
		{ " import",            false                            },
		{ " internal",          false                            },
		{ "=little_endian",     false                            },
		{ " longlong",          true                             },
		{ ";mark",                                               },
		{ "&once",              (long) &pragma_once              },
		{ ";options",                                            },
		{ " optimize_for_size", false                            },
		{ ";parameter",                                          },
		{ ";pointers_in_A0",                                     },
		{ ";pointers_in_D0",                                     },
		{ ";pop",                                                },
		{ "=precompile",        true,  &global_config_precompile },
		{ "=preprocess",        true                             },
		{ ";push",                                               },
	};
	
	static option_t* find_option( const plus::string& token )
	{
		option_t* begin = the_options;
		option_t* end   = begin + sizeof the_options / sizeof the_options[0];
		
		option_t* it = std::find( begin, end, token );
		
		return it != end ? it : NULL;
	}
	
	static bool missing_prerequisite( const option_t& option )
	{
		const bool* prerequisite = option.dependency;
		
		return prerequisite != NULL  &&  !*prerequisite;
	}
	
	static bool get_boolean_option( const option_t& option )
	{
		return   missing_prerequisite( option ) ? false
		       : stack_null( option.stack )     ? option.initial
		       :                                  stack_top( option.stack );
	}
	
	static void set_boolean_option( option_t& option, bool new_value )
	{
		if ( stack_undefined( option.stack ) )
		{
			option.stack = pushed_stack( empty_stack(), option.initial );
		}
		
		push_stack( option.stack, new_value );
	}
	
	static void reset_boolean_option( option_t& option )
	{
		if ( !stack_null( option.stack ) )
		{
			pop_stack( option.stack );
		}
	}
	
	bool check_option( const plus::string& name )
	{
		if ( const option_t* option = find_option( name ) )
		{
			const char code = option->name[0];
			
			if ( code == ' '  ||  code == '=' )
			{
				return get_boolean_option( *option );
			}
			else if ( code == '@' )
			{
				return *option->dependency;
			}
		}
		
		return false;
	}
	
	static void modify_boolean_option( option_t& option, const plus::string& param )
	{
		if ( param == "reset" )
		{
			reset_boolean_option( option );
		}
		else if ( param == "on" )
		{
			set_boolean_option( option, true );
		}
		else if ( param == "off" )
		{
			set_boolean_option( option, false );
		}
		else
		{
			throw exception( param );
		}
	}
	
	static void update_variable_from_option( const option_t& option )
	{
		if ( bool* prerequisite = option.dependency )
		{
			*prerequisite = stack_top( option.stack );
		}
	}
	
	bool pragma( const plus::string& name, const plus::string& param )
	{
		if ( option_t* option = find_option( name ) )
		{
			switch ( option->name[0] )
			{
				case '&':
					{
						pragma_handler handler = (pragma_handler) option->initial;
						
						handler( param );
					}
					
					// Currently the only handler is #pragma once, which we omit
					return false;
				
				case ' ':
					if ( missing_prerequisite( *option ) )
					{
						throw exception( name );
					}
					
					modify_boolean_option( *option, param );
					break;
				
				case '@':
					modify_boolean_option( *option, param );
					
					update_variable_from_option( *option );
				
				case ';':
					break;
				
				case '=':  // read-only option -- not a pragma
				default:
					throw exception( name );
			}
			
			return true;
		}
		
		throw exception( name );
	}
	
}
