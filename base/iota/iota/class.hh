/*
	iota/class.hh
	-------------
*/

#ifndef IOTA_CLASS_HH
#define IOTA_CLASS_HH

/*
	NON_COPYABLE() does exactly what it says and looks like it does.
*/

#define NON_COPYABLE( name )  \
private:                      \
	name           ( const name& );  \
	name& operator=( const name& );

/*
	A class with an explicitly defined destructor, when
	compiled with Metrowerks C++, will always include in
	its generated code a call to a deallocation function,
	even if no instance of the class is ever allocated with
	`new` or deallocated with `delete` (in which case the
	deallocation function is never called, but it still gets
	linked into the executable). By default, the deallocation
	function is ::operator delete(), which in turn depends
	on the exception throwing subsystem, linking *that* in.
	
	NO_NEW_DELETE suppresses the Metrowerks-generated call
	to ::operator delete() by overriding it with a no-op.
	It also makes using `new` or `delete` with the class
	a compile error, so you can't do so by mistake.
	(Unfortunately, this also prevents us from using
	placement `new`, even though it shouldn't, and even
	leaving `operator new` unreplaced doesn't help.)
	
	For a class that needs to be allocated dynamically only
	by some users, you can create a wrapper class that you
	allocate dynamically with a base class or a data member
	that's an instance of a class that's marked NO_NEW_DELETE.
	One example of this is vlib's class iterator_template
	(found in v/vlib/vlib/iterators/iterator_template.hh).
*/

#if defined( __GNUC__ )  &&  __GNUC__ < 4
	/*
		g++ 3.1, at least, gets confused by the operator new
		overload, so just skip the works and make it a no-op.
	*/
	#define NO_NEW_DELETE  /**/
#else
	/*
		This should have no codegen effect on non-Metrowerks
		compilers, but it will flag accidental misuse of new
		and delete that would trip up Metrowerks compile jobs.
	*/
	#define NO_NEW_DELETE  \
	private:               \
		void* operator new( unsigned long );  \
		void operator delete( void* )  {}
#endif

#endif
