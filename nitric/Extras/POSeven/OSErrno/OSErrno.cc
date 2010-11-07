// OSErrno.cc

#include "OSErrno/OSErrno.hh"

// Nitrogen
#include "Nitrogen/OpenTransport.hh"


namespace OSErrno
{
	
	poseven::errno_t ErrnoFromOSStatus( const Nitrogen::OSStatus& error )
	{
		int result = 0;
		
	#if OTUNIXERRORS
		
		if ( ::IsEError( error ) )
		{
			return poseven::errno_t( ::OSStatus2E( error ) );
		}
		
	#endif
		
		switch ( error.Get() )
		{
			case nsvErr         :  // fall through
			case resNotFound    :  // fall through
			case afpItemNotFound:  // fall through
			case fnfErr         :  result = ENOENT;        break;
			case ioErr          :  result = EIO;           break;
			case fnOpnErr       :  // fall through
			case rfNumErr       :  result = EBADF;         break;
			case kOTOutOfMemoryErr:// fall through
			case memFullErr     :  result = ENOMEM;        break;
			case opWrErr        :  // fall through
			case permErr        :  // fall through
			case portInUse      :  result = EACCES;        break;
			case userCanceledErr:  result = ECANCELED;     break;
			case fBsyErr        :  result = EBUSY;         break;
			case dupFNErr       :  result = EEXIST;        break;
			case diffVolErr     :  result = EXDEV;         break;
			case errFSNotAFolder:  // fall through
			case dirNFErr       :  result = ENOTDIR;       break;
			case notAFileErr    :  result = EISDIR;        break;
			case posErr         :  // fall through
			case paramErr       :  result = EINVAL;        break;
			case dskFulErr      :  result = ENOSPC;        break;
			case wPrErr         :  // fall through
			case vLckdErr       :  result = EROFS;         break;
			case kOTNoDataErr   :  result = EAGAIN;        break;
			case bdNamErr       :  result = ENAMETOOLONG;  break;
			
			default:
				throw error;
		}
		
		return poseven::errno_t( result );
	}
	
}

