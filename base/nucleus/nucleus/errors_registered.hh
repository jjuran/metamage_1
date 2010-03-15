// nucleus/errors_registered.hh
// ----------------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2008 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef NUCLEUS_ERRORSREGISTERED_HH
#define NUCLEUS_ERRORSREGISTERED_HH


namespace nucleus
{   
	
	#define NUCLEUS_DECLARE_ERRORS_DEPENDENCY( module )  struct module##_errors_registration_dependency { module##_errors_registration_dependency(); }
	
	#define NUCLEUS_DEFINE_ERRORS_DEPENDENCY( module )  module##_errors_registration_dependency::module##_errors_registration_dependency()  {}
	
#ifdef __MWERKS__
	
	// Metrowerks' linker schedules all static initialization code to run
	// before main(), so no tricks are needed here.
	#define NUCLEUS_REQUIRE_ERRORS( module )  /**/
	
	#define NUCLEUS_REQUIRE_ERRORS_STATIC( module )  /**/
	
#else
	
	#define NUCLEUS_REQUIRE_ERRORS( module )  module##_errors_registration_dependency()
	
	#define NUCLEUS_REQUIRE_ERRORS_STATIC( module )  static module##_errors_registration_dependency global_##module##_errors_registration_dependency;
	
#endif
	
}

#endif

