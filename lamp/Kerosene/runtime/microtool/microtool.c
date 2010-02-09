/*
	microtool.c
	-----------
	
	Joshua Juran
*/


extern void _set_dispatcher( void *address );

extern void* _initialize_environ( char **envp );

// Call InitProc() to set references to cleanup proc and errno
extern void InitializeCallbacks();

// Call main() and return
extern int _lamp_main( int argc, char **argv, char **envp, void *dispatcher );
extern int       main( int argc, char **argv );


#pragma force_active on

int _lamp_main( int argc, char **argv, char **envp, void *dispatcher )
{
	_set_dispatcher( dispatcher );
	
	(void) _initialize_environ( envp );
	
	InitializeCallbacks();
	
	return main( argc, argv );
}

#pragma force_active reset

