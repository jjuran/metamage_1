/*	===============
 *	debug/assert.hh
 *	===============
 */

#ifndef DEBUG_ASSERT_HH
#define DEBUG_ASSERT_HH


#if CONFIG_DEBUGGING
	
	#if defined( __GNUC__ )  ||  defined(__MWERKS__)  &&  (__MWERKS__ >= 0x2400)
	#define DEBUG_CURRENT_FUNCTION  __PRETTY_FUNCTION__
	#else
	#define DEBUG_CURRENT_FUNCTION  "(unknown)"
	#endif
	
	#define ASSERT( cond )                                           \
		if ( !(cond) )                                               \
			debug::handle_failed_assertion( #cond,                   \
			                                DEBUG_CURRENT_FUNCTION,  \
			                                __FILE__,                \
			                                __LINE__,                \
			                                true );                  \
		else (void) 0
	
	#define EXPECT( cond )                                           \
		if ( !(cond) )                                               \
			debug::handle_failed_assertion( #cond,                   \
			                                DEBUG_CURRENT_FUNCTION,  \
			                                __FILE__,                \
			                                __LINE__,                \
			                                false );                 \
		else (void) 0
	
#else
	
	#define ASSERT( cond )  if ( 0 ) (void) !(cond); else (void) 0
	#define EXPECT( cond )  if ( 0 ) (void) !(cond); else (void) 0
	
#endif


namespace debug
{
	
	void handle_failed_assertion( const char*  text,
	                              const char*  func,
	                              const char*  file,
	                              unsigned     line,
	                              bool         fatal );
	
}

#endif
