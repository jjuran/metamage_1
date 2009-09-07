/*	===============
 *	debug/assert.hh
 *	===============
 */

#ifndef DEBUG_ASSERT_HH
#define DEBUG_ASSERT_HH


#if TARGET_CONFIG_DEBUGGING
	
	#if defined(__MWERKS__) && (__MWERKS__ < 0x0300)
	#define DEBUG_CURRENT_FUNCTION  __func__
	#else
	#define DEBUG_CURRENT_FUNCTION  __PRETTY_FUNCTION__
	#endif
	
	#define ASSERT( cond )                                           \
		if ( !(cond) )                                               \
			debug::handle_failed_assertion( #cond,                   \
			                                DEBUG_CURRENT_FUNCTION,  \
			                                __FILE__,                \
			                                __LINE__ );              \
		else 0
	
#else
	
	#define ASSERT( cond )  if ( 0 ) (void) !(cond); else 0
	
#endif


namespace debug
{
	
	void handle_failed_assertion( const char*  text,
	                              const char*  func,
	                              const char*  file,
	                              unsigned     line );
	
}

#endif

