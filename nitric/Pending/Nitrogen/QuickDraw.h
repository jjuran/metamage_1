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
