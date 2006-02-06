// QDOffscreen.h

#ifndef NITROGEN_QDOFFSCREEN_H
#define NITROGEN_QDOFFSCREEN_H

#ifndef NITROGEN_FRAMEWORKHEADER_H
#include "Nitrogen/FrameworkHeader.h"
#endif
#ifndef __QDOFFSCREEN__
#include FRAMEWORK_HEADER(QD,QDOffscreen.h)
#endif
#ifndef NITROGEN_MACERRORS_H
#include "Nitrogen/MacErrors.h"
#endif
#ifndef NITROGEN_QUICKDRAW_H
#include "Nitrogen/QuickDraw.h"
#endif

namespace Nitrogen
{
	
	class GWorldFlags_Tag {};
	typedef FlagType< GWorldFlags_Tag, ::GWorldFlags > GWorldFlags;
	
	typedef CGrafPtr GWorldPtr;
	
	struct GWorld_State
	{
		GWorldPtr port;
		GDHandle gdh;
	};
	
	typedef GWorld_State GetGWorld_Result;
	
	template <>
	struct Maker< GWorld_State >
	{
		GWorld_State operator()( GWorldPtr port, GDHandle gdh = NULL ) const
		{
			GWorld_State result;
			
			result.port = port;
			result.gdh  = gdh;
			
			return result;
		}
	};
	
	
	struct GWorldDisposer : public std::unary_function< GWorldPtr, void >
	{
		void operator()( GWorldPtr gWorld ) const
		{
			::DisposeGWorld( gWorld );
		}
	};
	
	Owned< GWorldPtr, GWorldDisposer > NewGWorld( short        pixelDepth,
	                                              const Rect&  boundsRect,
	                                              CTabHandle   cTable   = NULL,
	                                              GDHandle     aGDevice = NULL,
	                                              GWorldFlags  flags    = GWorldFlags() );
	
	Owned< GWorldPtr, GWorldDisposer > NewGWorld( short        pixelDepth,
	                                              const Rect&  boundsRect,
	                                              GWorldFlags  flags );
	
	struct LockPixels_Failed {};
	
	void LockPixels( PixMapHandle pm );
	
	using ::UnlockPixels;
	
	GWorldFlags UpdateGWorld( Owned< GWorldPtr, GWorldDisposer >&  offscreenGWorld,
	                          short                                pixelDepth,
	                          const Rect&                          boundsRect,
	                          CTabHandle                           cTable   = NULL,
	                          GDHandle                             aGDevice = NULL,
	                          GWorldFlags                          flags    = GWorldFlags() );
	
	inline void DisposeGWorld( Owned< GWorldPtr, GWorldDisposer > )  {}
	
	GWorld_State GetGWorld();
	
	void SetGWorld( const GWorld_State& state );
	void SetGWorld( GWorldPtr gWorld );
	
	GWorldFlags GetPixelsState( PixMapHandle pm );
	
	void SetPixelsState( PixMapHandle pm, GWorldFlags state );
	
	PixMapHandle GetGWorldPixMap( GWorldPtr offscreenGWorld );
	
	class GWorld_Value
	{
		public:
			typedef GWorld_State         Value;
			typedef GWorld_State         GetResult;
			typedef GWorld_State const&  SetParameter;
			
			static const bool hasSwap = false;
			
			GetResult Get() const                 { return GetGWorld(); }
			void Set( SetParameter state ) const  { SetGWorld( state ); }
	};
	
	class PixelsState_Value
	{
		private:
			PixMapHandle pm;
		
		public:
			typedef GWorldFlags Value;
			typedef GWorldFlags GetResult;
			typedef GWorldFlags SetParameter;
			
			static const bool hasSwap = false;
			
			PixelsState_Value( PixMapHandle pm ) : pm( pm )  {}
			
			GetResult Get() const                  { return Nitrogen::GetPixelsState( pm ); }
			void Set( SetParameter state ) const   { SetPixelsState( pm, state );           }
	};
	
	typedef GWorld_Value       GWorld_Details;
	typedef PixelsState_Value  PixelsState_Details;
	
	typedef Pseudoreference< GWorld_Details      > GWorld;
	typedef Pseudoreference< PixelsState_Details > PixelsState;
	
}

#endif
