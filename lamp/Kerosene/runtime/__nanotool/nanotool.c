/*
	nanotool.c
	----------
	
	Joshua Juran
*/


#if defined( __MC68K__ )  &&  !defined( __CFM68K__ )
	#if !__option( far_data ) || !__option( far_strings ) || !__option( far_vtables )
		
		#define HAVE_A4_REFERENCES 1
		
		long SetCurrentA4( void );
		
	#endif
#endif

// Call main() and return
extern int _relix_main( void );

extern int main( int argc, char** argv );


int _relix_main( void )
{
#ifdef HAVE_A4_REFERENCES
	
	(void) SetCurrentA4();
	
#endif
	
	return main( 0, 0 );  // NULL
}
