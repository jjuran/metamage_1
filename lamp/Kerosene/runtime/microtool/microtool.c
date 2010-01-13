/*
	microtool.c
	-----------
	
	Joshua Juran
*/


extern void _set_dispatcher( void *address );

// Call main() and return
extern int _lamp_main( int argc, char **argv, char **envp, void *dispatcher );
extern int       main( int argc, char **argv );


#pragma force_active on

int _lamp_main( int argc, char **argv, char **envp, void *dispatcher )
{
	_set_dispatcher( dispatcher );
	
	return main( argc, argv );
}

#pragma force_active reset

