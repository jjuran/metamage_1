// Nucleus/ErrorsRegistered.h
// --------------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2008 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef NUCLEUS_ERRORSREGISTERED_H
#define NUCLEUS_ERRORSREGISTERED_H


namespace Nucleus
{   
	
	#define NUCLEUS_DECLARE_ERRORS_DEPENDENCY( module )  struct module##ErrorsRegistrationDependency { module##ErrorsRegistrationDependency(); }
	
	#define NUCLEUS_DEFINE_ERRORS_DEPENDENCY( module )  module##ErrorsRegistrationDependency::module##ErrorsRegistrationDependency()  {}
	
#ifdef __MWERKS__
	
	// Metrowerks' linker schedules all static initialization code to run
	// before main(), so no tricks are needed here.
	#define NUCLEUS_REQUIRE_ERRORS( module )  /**/
	
	#define NUCLEUS_REQUIRE_ERRORS_STATIC( module )  /**/
	
#else
	
	#define NUCLEUS_REQUIRE_ERRORS( module )  module##ErrorsRegistrationDependency()
	
	#define NUCLEUS_REQUIRE_ERRORS_STATIC( module )  static module##ErrorsRegistrationDependency g##module##ErrorsRegistrationDependency;
	
#endif
	
}

#endif

