// MacMemory.cp

#ifndef NITROGEN_MACMEMORY_H
#include "Nitrogen/MacMemory.h"
#endif

namespace Nitrogen
  {
   void MemError()
     {
      OnlyOnce<RegisterMemoryManagerErrors>();
      ThrowOSStatus( ::MemError() );
     }
   
   Owned<Handle> NewHandle( std::size_t size )
     {
      Handle result = ::NewHandle( static_cast< ::Size >( size ) );
      if ( result == 0 )
         MemError();
      return Owned<Handle>::Seize( result );
     };
   
   void RegisterMemoryManagerErrors()
     {
      OnlyOnce< RegisterOSStatus< menuPrgErr              > >();
      OnlyOnce< RegisterOSStatus< negZcbFreeErr           > >();
      OnlyOnce< RegisterOSStatus< paramErr                > >();
      OnlyOnce< RegisterOSStatus< memROZErr               > >();
      OnlyOnce< RegisterOSStatus< memFullErr              > >();
      OnlyOnce< RegisterOSStatus< nilHandleErr            > >();
      OnlyOnce< RegisterOSStatus< memAdrErr               > >();
      OnlyOnce< RegisterOSStatus< memWZErr                > >();
      OnlyOnce< RegisterOSStatus< memPurErr               > >();
      OnlyOnce< RegisterOSStatus< memBCErr                > >();
      OnlyOnce< RegisterOSStatus< memLockedErr            > >();
      OnlyOnce< RegisterOSStatus< notEnoughMemoryErr      > >();
      OnlyOnce< RegisterOSStatus< notHeldErr              > >();
      OnlyOnce< RegisterOSStatus< cannotMakeContiguousErr > >();
      OnlyOnce< RegisterOSStatus< notLockedErr            > >();
      OnlyOnce< RegisterOSStatus< interruptsMaskedErr     > >();
      OnlyOnce< RegisterOSStatus< cannotDeferErr          > >();
     }
  }
