/*	===============
 *	ScrollerBase.hh
 *	===============
 */

#ifndef GENIE_FILESYSTEM_SCROLLERBASE_HH
#define GENIE_FILESYSTEM_SCROLLERBASE_HH

// Pedestal
#include "Pedestal/Scroller_beta.hh"


namespace Genie
{
	
	class FSTree;
	
	struct ScrollerParameters
	{
		int  itsClientWidth;
		int  itsClientHeight;
		
		int  itsHOffset;
		int  itsVOffset;
		
		Rect  itsLastViewBounds;
		
		Pedestal::Scroller*  itsView;
		
		ScrollerParameters() : itsClientWidth ( 0 ),
		                       itsClientHeight( 0 ),
		                       itsHOffset( 0 ),
		                       itsVOffset( 0 ),
		                       itsView()
		{
		}
		
		static ScrollerParameters* Find( const FSTree* key );
		static ScrollerParameters& Get ( const FSTree* key );
		
		static void Erase( const FSTree* key );
		
		static int& Width ( const FSTree* view )  { return Get( view ).itsClientWidth;  }
		static int& Height( const FSTree* view )  { return Get( view ).itsClientHeight; }
		
		static int& HOffset( const FSTree* view )  { return Get( view ).itsHOffset; }
		static int& VOffset( const FSTree* view )  { return Get( view ).itsVOffset; }
		
		static Rect& ViewBounds( const FSTree* view )  { return Get( view ).itsLastViewBounds; }
	};
	
	short GetScrollerLastViewWidth ( const FSTree* scroller );
	short GetScrollerLastViewHeight( const FSTree* scroller );
	
	int GetScrollerClientWidth ( const FSTree* scroller );
	int GetScrollerClientHeight( const FSTree* scroller );
	
	int GetScrollerHOffset( const FSTree* scroller );
	int GetScrollerVOffset( const FSTree* scroller );
	
	void SetScrollerHOffset( const FSTree* scroller, int h );
	void SetScrollerVOffset( const FSTree* scroller, int v );
	
	Rect GetScrollerLastViewBounds( const FSTree* scroller );
	
	bool IsScrolledToBottom( const ScrollerParameters& params );
	
	
	class ScrollerProxy : public Pedestal::ScrollerAPI
	{
		private:
			typedef const FSTree* Key;
			
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
	
	
	class ScrollerBase : public Pedestal::Scroller
	{
		public:
			typedef const FSTree* Key;
		
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
			
			void Draw( const Rect& bounds, bool erasing );
	};
	
}

#endif

