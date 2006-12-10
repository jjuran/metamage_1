/*	========
 *	Movies.h
 *	========
 */

#ifndef NITROGEN_MOVIES_H
#define NITROGEN_MOVIES_H

#ifndef NITROGEN_FRAMEWORKHEADER_H
#include "Nitrogen/FrameworkHeader.h"
#endif
#ifndef __MOVIES__
#include FRAMEWORK_HEADER(QuickTime,QuickTime.h)
#endif

#ifndef NUCLEUS_FLAGTYPE_H
#include "Nucleus/FlagType.h"
#endif
#ifndef NUCLEUS_IDTYPE_H
#include "Nucleus/IDType.h"
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
	
	class QuickTimeErrorsRegistrationDependency
	{
		public:
			QuickTimeErrorsRegistrationDependency();
	};
	
	using ::Movie;
	
	class MovieFileRefNum_Tag {};
	typedef Nucleus::IDType< MovieFileRefNum_Tag, SInt16, -1 > MovieFileRefNum;
	
	class NewMovieFlags_Tag {};
	typedef Nucleus::FlagType< NewMovieFlags_Tag, SInt16 > NewMovieFlags;
	
	// Opaque pointer type
	typedef struct FullScreenContext* FullScreenContextPtr;
	
	class FullScreenFlags_Tag {};
	typedef Nucleus::FlagType< FullScreenFlags_Tag, long > FullScreenFlags;
	
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
			(void) Nitrogen::QuickTimeErrorsRegistrationDependency();
			
			HandleDestructionOSStatus( ::CloseMovieFile( refNum ) );
		}
	};
	
	template <>
	struct Disposer< Nitrogen::FullScreenContextPtr > : public std::unary_function< Nitrogen::FullScreenContextPtr, void >,
	                                                    private Nitrogen::DefaultDestructionOSStatusPolicy
	{
		void operator()( Nitrogen::FullScreenContextPtr context ) const
		{
			(void) Nitrogen::QuickTimeErrorsRegistrationDependency();
			
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

