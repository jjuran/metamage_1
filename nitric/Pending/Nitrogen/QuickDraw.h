// QuickDraw.h

#ifndef NITROGEN_QUICKDRAW_H
#define NITROGEN_QUICKDRAW_H

#ifndef NITROGEN_FRAMEWORKHEADER_H
#include "Nitrogen/FrameworkHeader.h"
#endif
#ifndef __QUICKDRAW__
#include FRAMEWORK_HEADER(QD,QuickDraw.h)
#endif
#ifndef NITROGEN_PSEUDOREFERENCE_H
#include "Nitrogen/Pseudoreference.h"
#endif

#ifndef NITROGEN_OWNED_H
#include "Nitrogen/Owned.h"
#endif

#ifndef NITROGEN_OWNED_H
#include "Nitrogen/OSStatus.h"
#endif

namespace Nitrogen
  {
	void RegisterQuickDrawErrors ();

#if OPAQUE_TOOLBOX_STRUCTS
	
	using ::GrafPtr;
	
#else
	
	class GrafPtr
	{
		private:
			typedef ::GrafPtr UnderlyingType;
			UnderlyingType value;
			
			::GrafPtr  GetGrafPtr()  const                         { return value; }
			::CGrafPtr GetCGrafPtr() const                         { return reinterpret_cast< ::CGrafPtr >( value ); }
			
		public:
			GrafPtr() : value(0)  {}
			GrafPtr( ::GrafPtr  value ) : value( value )  {}
			GrafPtr( ::CGrafPtr value ) : value( reinterpret_cast< ::GrafPtr >( value ) )  {}
			
			operator ::GrafPtr()  const                            { return GetGrafPtr();  }
			operator ::CGrafPtr() const                            { return GetCGrafPtr(); }
			
			static GrafPtr Make( ::GrafPtr  v )  { return GrafPtr( v ); }
			static GrafPtr Make( ::CGrafPtr v )  { return GrafPtr( v ); }
			
			template < class T > T Get() const;
			
			friend bool operator==( GrafPtr a, GrafPtr b )     { return a.value == b.value; }
			friend bool operator==( GrafPtr a, ::GrafPtr b )   { return a.Get< ::GrafPtr >() == b; }
			friend bool operator==( ::GrafPtr a, GrafPtr b )   { return a == b.Get< ::GrafPtr >(); }
			friend bool operator==( GrafPtr a, ::CGrafPtr b )  { return a.Get< ::CGrafPtr >() == b; }
			friend bool operator==( ::CGrafPtr a, GrafPtr b )  { return a == b.Get< ::CGrafPtr >(); }
			
			friend bool operator!=( GrafPtr a, GrafPtr b )     { return a.value != b.value; }
			friend bool operator!=( GrafPtr a, ::GrafPtr b )   { return a.Get< ::GrafPtr >() != b; }
			friend bool operator!=( ::GrafPtr a, GrafPtr b )   { return a != b.Get< ::GrafPtr >(); }
			friend bool operator!=( GrafPtr a, ::CGrafPtr b )  { return a.Get< ::CGrafPtr >() != b; }
			friend bool operator!=( ::CGrafPtr a, GrafPtr b )  { return a != b.Get< ::CGrafPtr >(); }
	};
	
	template <> inline ::GrafPtr  GrafPtr::Get< ::GrafPtr  >() const  { return GetGrafPtr();  }
	template <> inline ::CGrafPtr GrafPtr::Get< ::CGrafPtr >() const  { return GetCGrafPtr(); }
	
#endif
	
	typedef GrafPtr CGrafPtr;
	
   using ::InitCursor;

   Point GetPortPenSize( CGrafPtr );
   using ::SetPortPenSize;
   
   class PortPenSize_Details
     {
      private:
         CGrafPtr port;
      
      public:
         typedef Point Value;
         typedef Value GetResult;
         typedef Value SetParameter;
         
         PortPenSize_Details( CGrafPtr thePort )     : port( thePort ) {}
         GetResult Get() const                       { return GetPortPenSize( port ); }
         void Set( SetParameter size ) const         { SetPortPenSize( port, size ); }
     };
   
  template <> struct Disposer< RgnHandle >: public std::unary_function< RgnHandle, void >
     {
      void operator()( RgnHandle h ) const
        {
         ::DisposeRgn( h );
        }
     };

//	Region handling
	Owned<RgnHandle> NewRgn (void);
	
//	extern void OpenRgn(void)
	using ::OpenRgn;

//	extern void CloseRgn(RgnHandle dstRgn)
	using ::CloseRgn;
	
//	extern void CopyRgn ( RgnHandle srcRgn, RgnHandle dstRgn );
	using ::CopyRgn;
	
	Owned<RgnHandle> CopyRgn ( RgnHandle srcRgn );

//	extern void SetEmptyRgn(RgnHandle rgn);
	using ::SetEmptyRgn;

  }

#endif
