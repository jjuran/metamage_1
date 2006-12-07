// OSErrno.cc

#include "OSErrno/OSErrno.hh"

// Nitrogen
#include "Nitrogen/OpenTransport.h"


namespace OSErrno
{
	
	POSeven::Errno ErrnoFromOSStatus( const Nitrogen::OSStatus& error )
	{
		int result = 0;
		
	#if OTUNIXERRORS
		
		if ( ::IsEError( error ) )
		{
			return POSeven::Errno( ::OSStatus2E( error ) );
		}
		
	#endif
		
		switch ( error.Get() )
		{
			case eofErr         :  result = 0;          break;
			case fnfErr         :  result = ENOENT;     break;
			case ioErr          :  result = EIO;        break;
			case fnOpnErr       :  // fall through
			case rfNumErr       :  result = EBADF;      break;
			case memFullErr     :  result = ENOMEM;     break;
			case userCanceledErr:  result = ECANCELED;  break;
			case fBsyErr        :  result = EBUSY;      break;
			case dupFNErr       :  result = EEXIST;     break;
			case diffVolErr     :  result = EXDEV;      break;
			case dirNFErr       :  result = ENOTDIR;    break;
			case notAFileErr    :  result = EISDIR;     break;
			case posErr         :  // fall through
			case paramErr       :  result = EINVAL;     break;
			case dskFulErr      :  result = ENOSPC;     break;
			case vLckdErr       :  result = EROFS;      break;
			
			default:
				throw error;
		}
		
		return POSeven::Errno( result );
	}
	
}

