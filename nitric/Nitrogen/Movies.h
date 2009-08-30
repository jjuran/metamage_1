/*	========
 *	Movies.h
 *	========
 */

#ifndef NITROGEN_MOVIES_H
#define NITROGEN_MOVIES_H

#include "Nitrogen/FrameworkHeader.h"

// Mac OS X Frameworks
#if NITROGEN_USE_FRAMEWORK_INCLUDES
	#include <QuickTime/QuickTime.h>
#endif

// Mac OS Universal Interfaces
#ifndef __MOVIES__
	#include <Movies.h>
#endif

// Nitrogen
#include "Nitrogen/Files.h"
#include "Nitrogen/FlagType.h"
#include "Nitrogen/IDType.h"
#include "Nitrogen/OSStatus.h"
#include "Nitrogen/Owned.h"
#include "Nitrogen/Resources.h"


namespace Nitrogen
{
	
	void RegisterQuickTimeErrors();
	
	using ::Movie;
	
	class MovieFileRefNum_Tag {};
	typedef IDType< MovieFileRefNum_Tag, SInt16, -1 > MovieFileRefNum;
	
	class NewMovieFlags_Tag {};
	typedef FlagType< NewMovieFlags_Tag, SInt16 > NewMovieFlags;
	
	// Opaque pointer type
	typedef struct FullScreenContext* FullScreenContextPtr;
	
	class FullScreenFlags_Tag {};
	typedef FlagType< FullScreenFlags_Tag, long > FullScreenFlags;
	
	template <>
	struct Disposer< Movie > : public std::unary_function< Movie, void >
	{
		void operator()( Movie movie ) const
		{
			::DisposeMovie( movie );
		}
	};
	
	template <>
	struct Disposer< MovieFileRefNum > : public std::unary_function< MovieFileRefNum, void >,
	                                     private DefaultDestructionOSStatusPolicy
	{
		void operator()( MovieFileRefNum refNum ) const
		{
			HandleDestructionOSStatus( ::CloseMovieFile( refNum ) );
		}
	};
	
	template <>
	struct Disposer< FullScreenContextPtr > : public std::unary_function< FullScreenContextPtr, void >,
	                                          private DefaultDestructionOSStatusPolicy
	{
		void operator()( FullScreenContextPtr context ) const
		{
			HandleDestructionOSStatus( ::EndFullScreen( reinterpret_cast< ::Ptr >( context ), 0 ) );
		}
	};
	
	#pragma mark -
	#pragma mark ¥ Routines ¥
	
	inline void EnterMovies()  { ThrowOSStatus( ::EnterMovies() ); }
	
	using ::ExitMovies;
	
	inline void GetMoviesError()  { ThrowOSStatus( ::GetMoviesError() ); }
	
	// ...
	
	Owned< MovieFileRefNum > OpenMovieFile( const FSSpec& file, FSIOPermssn permission );
	
	// DeleteMovieFile
	
	Owned< Movie > NewMovieFromFile( MovieFileRefNum  refNum,
	                                 ResID            resID,
	                                 NewMovieFlags    flags );
	
	// ...
	
	Owned< FullScreenContextPtr > BeginFullScreen( GDHandle         whichGD,
	                                               short            desiredWidth,
	                                               short            desiredHeight,
	                                               ::WindowRef*     newWindow,
	                                               const RGBColor&  eraseColor,
	                                               FullScreenFlags  flags      = FullScreenFlags() );
	
	Owned< FullScreenContextPtr > BeginFullScreen( GDHandle         whichGD,
	                                               short            desiredWidth,
	                                               short            desiredHeight,
	                                               ::WindowRef*     newWindow  = NULL,
	                                               FullScreenFlags  flags      = FullScreenFlags() );
	
	Owned< FullScreenContextPtr > BeginFullScreen( GDHandle         whichGD,
	                                               short            desiredWidth,
	                                               short            desiredHeight,
	                                               const RGBColor&  eraseColor,
	                                               FullScreenFlags  flags      = FullScreenFlags() );
	
	inline void EndFullScreen( Owned< FullScreenContextPtr > )  {}
	
}

#endif

