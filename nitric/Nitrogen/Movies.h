// Nitrogen/Movies.h
// -----------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2004-2009 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef NITROGEN_MOVIES_H
#define NITROGEN_MOVIES_H

#ifndef NITROGEN_FRAMEWORKHEADER_H
#include "Nitrogen/FrameworkHeader.h"
#endif
#ifndef __MOVIES__
#include FRAMEWORK_HEADER(QuickTime,QuickTime.h)
#endif

#ifndef NUCLEUS_FLAG_H
#include "Nucleus/Flag.h"
#endif
#ifndef NUCLEUS_OWNED_H
#include "Nucleus/Owned.h"
#endif

// Nitrogen
#ifndef NITROGEN_FILES_H
#include "Nitrogen/Files.h"
#endif
#ifndef NITROGEN_OSSTATUS_H
#include "Nitrogen/OSStatus.h"
#endif
#ifndef NITROGEN_RESOURCES_H
#include "Nitrogen/Resources.h"
#endif


namespace Nitrogen
{
	
	NUCLEUS_DECLARE_ERRORS_DEPENDENCY( QuickTime );
	
	using ::Movie;
	
	enum MovieFileRefNum
	{
		kMovieFileRefNum_Max = Nucleus::Enumeration_Traits< SInt16 >::max
	};
	
	enum NewMovieFlags
	{
		kNewMovieFlags_Max = Nucleus::Enumeration_Traits< SInt16 >::max
	};
	
	NUCLEUS_DEFINE_FLAG_OPS( NewMovieFlags )
	
	// Opaque pointer type
	typedef struct FullScreenContext* FullScreenContextPtr;
	
	enum FullScreenFlags
	{
		kFullScreenFlags_Max = Nucleus::Enumeration_Traits< long >::max
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

