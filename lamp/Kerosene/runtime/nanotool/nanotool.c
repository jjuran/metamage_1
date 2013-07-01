/*
	nanotool.c
	----------
	
	Joshua Juran
*/


// Call main() and return
extern int _lamp_main( void );

extern int main( int argc, char** argv );


int _lamp_main( void )
{
	return main( 0, 0 );  // NULL
}

