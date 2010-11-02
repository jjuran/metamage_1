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
#ifndef NUCLEUS_ENUMERATIONTRAITS_HH
#include "nucleus/enumeration_traits.hh"
#endif
#ifndef NUCLEUS_ERRORSREGISTERED_HH
#include "nucleus/errors_registered.hh"
#endif
#ifndef NUCLEUS_FLAGOPS_HH
#include "nucleus/flag_ops.hh"
#endif
#ifndef NUCLEUS_OWNED_HH
#include "nucleus/owned.hh"
#endif

// Nitrogen
#ifndef MAC_FILES_TYPES_FSIOPERM_HH
#include "Mac/Files/Types/FSIOPerm.hh"
#endif
#ifndef MAC_RESOURCES_TYPES_RESID_HH
#include "Mac/Resources/Types/ResID.hh"
#endif

#ifndef NITROGEN_OSSTATUS_HH
#include "Nitrogen/OSStatus.hh"
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

namespace nucleus
{
	
	template <>
	struct disposer< Movie >
	{
		typedef Movie  argument_type;
		typedef void   result_type;
		
		void operator()( Movie movie ) const
		{
			::DisposeMovie( movie );
		}
	};
	
	template <>
	struct disposer< Nitrogen::MovieFileRefNum >
	{
		typedef Nitrogen::MovieFileRefNum  argument_type;
		typedef void                       result_type;
		
		void operator()( Nitrogen::MovieFileRefNum refNum ) const
		{
			NUCLEUS_REQUIRE_ERRORS( Nitrogen::QuickTime );
			
			::Nitrogen::HandleDestructionOSStatus( ::CloseMovieFile( refNum ) );
		}
	};
	
	template <>
	struct disposer< Nitrogen::FullScreenContextPtr >
	{
		typedef Nitrogen::FullScreenContextPtr  argument_type;
		typedef void                            result_type;
		
		void operator()( Nitrogen::FullScreenContextPtr context ) const
		{
			NUCLEUS_REQUIRE_ERRORS( Nitrogen::QuickTime );
			
			::Nitrogen::HandleDestructionOSStatus( ::EndFullScreen( (::Ptr) context, 0 ) );
		}
	};
	
}

namespace Nitrogen
{
	
	inline void EnterMovies()  { ThrowOSStatus( ::EnterMovies() ); }
	
	using ::ExitMovies;
	
	inline void GetMoviesError()  { ThrowOSStatus( ::GetMoviesError() ); }
	
	// ...
	
	nucleus::owned< MovieFileRefNum > OpenMovieFile( const FSSpec&  file,
	                                                 Mac::FSIOPerm  permission );
	
	// DeleteMovieFile
	
	nucleus::owned< Movie > NewMovieFromFile( MovieFileRefNum  refNum,
	                                          Mac::ResID       resID,
	                                          NewMovieFlags    flags );
	
	// ...
	
	nucleus::owned< FullScreenContextPtr > BeginFullScreen( GDHandle         whichGD,
	                                                        short            desiredWidth,
	                                                        short            desiredHeight,
	                                                        ::WindowRef*     newWindow,
	                                                        const RGBColor&  eraseColor,
	                                                        FullScreenFlags  flags      = FullScreenFlags() );
	
	nucleus::owned< FullScreenContextPtr > BeginFullScreen( GDHandle         whichGD,
	                                                        short            desiredWidth,
	                                                        short            desiredHeight,
	                                                        ::WindowRef*     newWindow  = NULL,
	                                                        FullScreenFlags  flags      = FullScreenFlags() );
	
	nucleus::owned< FullScreenContextPtr > BeginFullScreen( GDHandle         whichGD,
	                                                        short            desiredWidth,
	                                                        short            desiredHeight,
	                                                        const RGBColor&  eraseColor,
	                                                        FullScreenFlags  flags      = FullScreenFlags() );
	
	inline void EndFullScreen( nucleus::owned< FullScreenContextPtr > )  {}
	
}

#endif

