/*	===============
 *	ScrollerBase.hh
 *	===============
 */

#ifndef GENIE_FILESYSTEM_SCROLLERBASE_HH
#define GENIE_FILESYSTEM_SCROLLERBASE_HH

// Pedestal
#ifndef PEDESTAL_SCROLLERBETA_HH
#include "Pedestal/Scroller_beta.hh"
#endif

// vfs
#include "vfs/node_fwd.hh"


namespace vfs
{
	
	struct property_params;
	
}

namespace Genie
{
	
	class Scroller : public Pedestal::ScrollerAPI
	{
		public:
			virtual void Scroll( int dh, int dv ) = 0;
	};
	
	struct ScrollerParameters
	{
		int  itsClientWidth;
		int  itsClientHeight;
		
		int  itsHOffset;
		int  itsVOffset;
		
		Rect  itsLastViewBounds;
		
		Scroller*  itsView;
		
		ScrollerParameters() : itsClientWidth ( 0 ),
		                       itsClientHeight( 0 ),
		                       itsHOffset( 0 ),
		                       itsVOffset( 0 ),
		                       itsView()
		{
		}
		
		static ScrollerParameters* Find( const vfs::node* key );
		static ScrollerParameters& Get ( const vfs::node* key );
		
		static void Erase( const vfs::node* key );
		
		static int& Width ( const vfs::node* view )  { return Get( view ).itsClientWidth;  }
		static int& Height( const vfs::node* view )  { return Get( view ).itsClientHeight; }
		
		static int& HOffset( const vfs::node* view )  { return Get( view ).itsHOffset; }
		static int& VOffset( const vfs::node* view )  { return Get( view ).itsVOffset; }
		
		static Rect& ViewBounds( const vfs::node* view )  { return Get( view ).itsLastViewBounds; }
	};
	
	short GetScrollerLastViewWidth ( const vfs::node* scroller );
	short GetScrollerLastViewHeight( const vfs::node* scroller );
	
	int GetScrollerClientWidth ( const vfs::node* scroller );
	int GetScrollerClientHeight( const vfs::node* scroller );
	
	int GetScrollerHOffset( const vfs::node* scroller );
	int GetScrollerVOffset( const vfs::node* scroller );
	
	void SetScrollerHOffset( const vfs::node* scroller, int h );
	void SetScrollerVOffset( const vfs::node* scroller, int v );
	
	Rect GetScrollerLastViewBounds( const vfs::node* scroller );
	
	bool IsScrolledToBottom( const ScrollerParameters& params );
	
	
	class ScrollerProxy : public Pedestal::ScrollerAPI
	{
		private:
			typedef const vfs::node* Key;
			
			Key  itsKey;
		
		public:
			ScrollerProxy() : itsKey()
			{
			}
			
			ScrollerProxy( Key key ) : itsKey( key )
			{
			}
			
			Key Get() const  { return itsKey; }
			
			short ViewWidth () const;
			short ViewHeight() const;
			
			int ClientWidth () const  { return GetScrollerClientWidth ( itsKey ); }
			int ClientHeight() const  { return GetScrollerClientHeight( itsKey ); }
			
			int GetHOffset() const;
			int GetVOffset() const;
			
			void SetHOffset( int h );
			void SetVOffset( int v );
	};
	
	
	class ScrollerBase : public Pedestal::Superview, public Scroller
	{
		public:
			typedef const vfs::node* Key;
		
		private:
			Key  itsKey;
		
		public:
			ScrollerBase( Key key ) : itsKey( key )
			{
			}
			
			void Install( const Rect& bounds );
			void Uninstall();
			
			Key GetKey() const  { return itsKey; }
			
			short ViewWidth () const  { return GetScrollerLastViewWidth ( itsKey ); }
			short ViewHeight() const  { return GetScrollerLastViewHeight( itsKey ); }
			
			int ClientWidth () const  { return GetScrollerClientWidth ( itsKey ); }
			int ClientHeight() const  { return GetScrollerClientHeight( itsKey ); }
			
			int GetHOffset() const  { return GetScrollerHOffset( itsKey ); }
			int GetVOffset() const  { return GetScrollerVOffset( itsKey ); }
			
			void SetHOffset( int h )  { SetScrollerHOffset( itsKey, h ); }
			void SetVOffset( int v )  { SetScrollerVOffset( itsKey, v ); }
			
			bool KeyDown( const EventRecord& event );
			
			void Draw( const Rect& bounds, bool erasing );
	};
	
	extern const vfs::property_params scroller_setting_params;
	
}

#endif
