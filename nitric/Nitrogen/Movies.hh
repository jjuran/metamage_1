// Nitrogen/Movies.hh
// ------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2004-2009 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef NITROGEN_MOVIES_HH
#define NITROGEN_MOVIES_HH

#ifndef __MOVIES__
#include <Movies.h>
#endif

// nucleus
#include "nucleus/enumeration_traits.hh"
#include "nucleus/flag_ops.hh"

#ifndef NUCLEUS_OWNED_H
#include "Nucleus/Owned.h"
#endif

// Nitrogen
#ifndef NITROGEN_FILES_HH
#include "Nitrogen/Files.hh"
#endif
#ifndef NITROGEN_OSSTATUS_HH
#include "Nitrogen/OSStatus.hh"
#endif
#ifndef NITROGEN_RESOURCES_HH
#include "Nitrogen/Resources.hh"
#endif


namespace Nitrogen
{
	
	NUCLEUS_DECLARE_ERRORS_DEPENDENCY( QuickTime );
	
	using ::Movie;
	
	enum MovieFileRefNum
	{
		kMovieFileRefNum_Max = nucleus::enumeration_traits< SInt16 >::max
	};
	
	enum NewMovieFlags
	{
		kNewMovieFlags_Max = nucleus::enumeration_traits< SInt16 >::max
	};
	
	NUCLEUS_DEFINE_FLAG_OPS( NewMovieFlags )
	
	// Opaque pointer type
	typedef struct FullScreenContext* FullScreenContextPtr;
	
	enum FullScreenFlags
	{
		kFullScreenFlags_Max = nucleus::enumeration_traits< long >::max
	};
	
	NUCLEUS_DEFINE_FLAG_OPS( FullScreenFlags )
	
}

namespace Nucleus
{
	
	template <>
	struct Disposer< Nitrogen::Movie > : public std::unary_function< Nitrogen::Movie, void >
	{
		void operator()( Nitrogen::Movie movie ) const
		{
			::DisposeMovie( movie );
		}
	};
	
	template <>
	struct Disposer< Nitrogen::MovieFileRefNum > : public std::unary_function< Nitrogen::MovieFileRefNum, void >,
	                                               private Nitrogen::DefaultDestructionOSStatusPolicy
	{
		void operator()( Nitrogen::MovieFileRefNum refNum ) const
		{
			NUCLEUS_REQUIRE_ERRORS( Nitrogen::QuickTime );
			
			HandleDestructionOSStatus( ::CloseMovieFile( refNum ) );
		}
	};
	
	template <>
	struct Disposer< Nitrogen::FullScreenContextPtr > : public std::unary_function< Nitrogen::FullScreenContextPtr, void >,
	                                                    private Nitrogen::DefaultDestructionOSStatusPolicy
	{
		void operator()( Nitrogen::FullScreenContextPtr context ) const
		{
			NUCLEUS_REQUIRE_ERRORS( Nitrogen::QuickTime );
			
			HandleDestructionOSStatus( ::EndFullScreen( reinterpret_cast< ::Ptr >( context ), 0 ) );
		}
	};
	
}

namespace Nitrogen
{
	
	#pragma mark -
	#pragma mark ¥ Routines ¥
	
	inline void EnterMovies()  { ThrowOSStatus( ::EnterMovies() ); }
	
	using ::ExitMovies;
	
	inline void GetMoviesError()  { ThrowOSStatus( ::GetMoviesError() ); }
	
	// ...
	
	Nucleus::Owned< MovieFileRefNum > OpenMovieFile( const FSSpec&  file,
	                                                 FSIOPermssn    permission );
	
	// DeleteMovieFile
	
	Nucleus::Owned< Movie > NewMovieFromFile( MovieFileRefNum  refNum,
	                                          ResID            resID,
	                                          NewMovieFlags    flags );
	
	// ...
	
	Nucleus::Owned< FullScreenContextPtr > BeginFullScreen( GDHandle         whichGD,
	                                                        short            desiredWidth,
	                                                        short            desiredHeight,
	                                                        ::WindowRef*     newWindow,
	                                                        const RGBColor&  eraseColor,
	                                                        FullScreenFlags  flags      = FullScreenFlags() );
	
	Nucleus::Owned< FullScreenContextPtr > BeginFullScreen( GDHandle         whichGD,
	                                                        short            desiredWidth,
	                                                        short            desiredHeight,
	                                                        ::WindowRef*     newWindow  = NULL,
	                                                        FullScreenFlags  flags      = FullScreenFlags() );
	
	Nucleus::Owned< FullScreenContextPtr > BeginFullScreen( GDHandle         whichGD,
	                                                        short            desiredWidth,
	                                                        short            desiredHeight,
	                                                        const RGBColor&  eraseColor,
	                                                        FullScreenFlags  flags      = FullScreenFlags() );
	
	inline void EndFullScreen( Nucleus::Owned< FullScreenContextPtr > )  {}
	
}

#endif

