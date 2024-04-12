/*
	gui/new/progress.cc
	-------------------
*/

#include "Genie/FS/gui/new/progress.hh"

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// missing-macos
#ifdef MAC_OS_X_VERSION_10_7
#ifndef MISSING_QUICKDRAW_H
#include "missing/Quickdraw.h"
#endif
#endif

// mac-config
#include "mac_config/color-quickdraw.hh"

// mac-sys-utils
#include "mac_sys/has/ColorQuickDraw.hh"

// plus
#include "plus/serialize.hh"
#include "plus/string.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/node/types/property_file.hh"

// Pedestal
#include "Pedestal/View.hh"

// Genie
#include "Genie/FS/Views.hh"


static inline
bool has_color_quickdraw()
{
	return CONFIG_COLOR_QUICKDRAW_GRANTED  ||  mac::sys::has_ColorQuickDraw();
}

namespace Genie
{
	
	namespace Ped = Pedestal;
	
	
	enum
	{
		kStdProgressBarHeight = 12,
		kSideMargin           = 12,
	};
	
	static const RGBColor gDarkGrey = { 0x4444, 0x4444, 0x4444 };
	static const RGBColor gSkyBlue  = { 0xcccc, 0xcccc, 0xffff };
	
	
	struct progress_extra : vfs::fixed_dir_extra
	{
		int value;  // permyriad, i.e. units of 1/10000
	};
	
	const size_t progress_annex_size = sizeof (progress_extra) - sizeof (vfs::fixed_dir_extra);
	
	
	class ProgressBar : public Ped::View
	{
		private:
			typedef const vfs::node* Key;
			
			Key  its_key;
		
		public:
			ProgressBar( Key key ) : its_key( key )
			{
			}
			
			int Value() const;
			
			void Draw( const Rect& bounds, bool erasing );
	};
	
	int ProgressBar::Value() const
	{
		progress_extra& extra = *(progress_extra*) its_key->extra();
		
		return extra.value;
	}
	
	static
	void PaintRect_In_Color( const Rect& bounds, const RGBColor& color )
	{
		RGBForeColor( &color );
		
		PaintRect( &bounds );
		
		ForeColor( blackColor );
	}
	
	static
	void PaintProgress( const Rect& insetBounds )
	{
		if ( CONFIG_COLOR_QUICKDRAW  &&  has_color_quickdraw() )
		{
			PaintRect_In_Color( insetBounds, gDarkGrey );
		}
		else
		{
			PaintRect( &insetBounds );
		}
	}
	
	static
	void EraseProgress( const Rect& insetBounds )
	{
		if ( CONFIG_COLOR_QUICKDRAW  &&  has_color_quickdraw() )
		{
			PaintRect_In_Color( insetBounds, gSkyBlue );
		}
		else
		{
			EraseRect( &insetBounds );
		}
	}
	
	static
	void DrawProgress( Rect insetBounds, int value )
	{
		if ( value < 0 )
		{
			value = 0;
		}
		
		if ( value > 10000 )
		{
			value = 10000;
		}
		
		const short left = insetBounds.left;
		
		const int boundsWidth = insetBounds.right - insetBounds.left;
		
		int progressWidth = value * boundsWidth / 10000;
		
		insetBounds.left += short( progressWidth );
		
		EraseProgress( insetBounds );
		
		insetBounds.right = insetBounds.left;
		insetBounds.left  = left;
		
		PaintProgress( insetBounds );
	}
	
	void ProgressBar::Draw( const Rect& bounds, bool erasing )
	{
		FrameRect( &bounds );
		
		Rect insetBounds = bounds;
		
		InsetRect( &insetBounds, 1, 1 );
		
		DrawProgress( insetBounds, Value() );
	}
	
	static boost::intrusive_ptr< Ped::View > CreateView( const vfs::node* delegate )
	{
		progress_extra& extra = *(progress_extra*) delegate->extra();
		
		extra.value = 0;
		
		return new ProgressBar( delegate );
	}
	
	
	typedef plus::serialize_int< int > serialize;
	
	static
	void get( plus::var_string& result, const vfs::node* view, bool binary )
	{
		progress_extra& extra = *(progress_extra*) view->extra();
		
		serialize::deconstruct::apply( result, extra.value, binary );
	}
	
	static
	void set( const vfs::node* view, const char* begin, const char* end, bool binary )
	{
		progress_extra& extra = *(progress_extra*) view->extra();
		
		extra.value = serialize::reconstruct::apply( begin, end, binary );
		
		InvalidateWindowForView( view );
	}
	
	static const vfs::property_params value_params =
	{
		serialize::fixed_size,
		&get,
		&set,
	};
	
	#define PROPERTY( prop )  &vfs::new_property, &prop##_params
	
	static const vfs::fixed_mapping local_mappings[] =
	{
		{ "value", PROPERTY( value ) },
		
		{ NULL, NULL }
	};
	
	vfs::node_ptr New_new_progress( const vfs::node*     parent,
	                                const plus::string&  name,
	                                const void*          args )
	{
		return New_new_view( parent,
		                     name,
		                     &CreateView,
		                     local_mappings,
		                     NULL,
		                     progress_annex_size );
	}
	
}
