/*
	nanotool.c
	----------
	
	Joshua Juran
*/


// Call main() and return
extern int _relix_main( void );

extern int main( int argc, char** argv );


int _relix_main( void )
{
	return main( 0, 0 );  // NULL
}
