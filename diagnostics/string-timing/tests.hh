/*
	tests.hh
	--------
*/

#if defined( I )  &&  I == 0
	
	// default construction
	//STRING s;  // this gets optimized away on modern compilers
	STRING strings[ 2 ];
	
#elif I == 1
	
	// pointer/length construction, small-string-optimizable
	STRING s( STR_LEN( "foo" ) );
	
#elif I == 2
	
	// pointer/length construction, allocated
	STRING s( STR_LEN( "0123456789abcdef0123456789abcdef" ) );
	
#elif I == 3
	
	// pointer/length construction, small-string-optimizable
	STRING a( STR_LEN( "123456789abcdef" ) );
	
	// copy construction (small string, no refcounting)
	STRING b = a;
	
#elif I == 4
	
	// pointer/length construction, allocated
	STRING a( STR_LEN( "0123456789abcdef0123456789abcdef" ) );
	
	// copy construction (refcounting)
	STRING b = a;
	
#elif I == 5
	#ifdef PLUS_STRING
	
	// pointer/length construction, static
	STRING a( STR_LEN( "0123456789abcdef0123456789abcdef" ), vxo::delete_never );
	
	// copy construction (static)
	STRING b = a;
	
	#else
	
	SKIP;
	
	#endif
#elif I == 6
	
	// pointer/length construction, allocated
	STRING a( STR_LEN( "0123456789abcdef0123456789abcdef" ) );
	
	// copy construction (refcounting)
	STRING b = a;
	
	// (possibly) immutable string iterator
	b.begin();
	
#elif I == 7
	
	// pointer/length construction, allocated
	STRING a( STR_LEN( "0123456789abcdef0123456789abcdef" ) );
	
	// copy construction (refcounting)
	VAR_STRING b = a;
	
	// mutable string iterator, copy on write
	b.begin();
	
#elif I == 8
	
	// pointer/length construction, allocated
	STRING a( STR_LEN( "0123456789abcdef0123456789abcdef" ) );
	
	// copy construction (refcounting)
	VAR_STRING b = a;
	
	// mutable string iterator, copy on write
	b.begin();
	
	// copy unsharable string
	STRING c = b;
	
#elif I == 9
	
	// pointer/length construction, allocated
	STRING a( STR_LEN( "0123456789abcdef0123456789abcdef" ) );
	
	// move string to var_string
	VAR_STRING b = move( a );
	
	// mutable string iterator, copy on write
	b.begin();
	
	// move var_string to string
	STRING c = move( b );
	
#elif I == 10
	
	// pointer/length construction, small-string-optimizable
	STRING a( STR_LEN( "123456789abcdef" ) );
	
	// move
	STRING b = move( a );
	
#elif I == 11
	
	// pointer/length construction, allocated
	STRING a( STR_LEN( "0123456789abcdef0123456789abcdef" ) );
	
	// move
	STRING b = move( a );
	
#elif I == 12
	
	// pointer/length construction, allocated
	STRING a( STR_LEN( "0123456789abcdef0123456789abcdef" ) );
	
	// move construction
	VAR_STRING b = move( a );
	
	// mutable string iterator
	b.begin();
	
#else
	
	#error Not a valid test number
	
#endif
