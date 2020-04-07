/*
	errno_from_mac_error.hh
	-----------------------
*/

#include "mac_sys/errno_from_mac_error.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#if ! __LP64__  &&  ! defined( MAC_OS_X_VERSION_10_8 )
#ifndef __OPENTRANSPORT__
#include <OpenTransport.h>
#endif
#endif

// Standard C
#include <errno.h>


namespace mac {
namespace sys {
	
	int errno_from_mac_error( long err )
	{
		if ( err <= kEPERMErr  &&  err >= kENOMSGErr )
		{
			return 1 + kEPERMErr - err;
		}
		
		int result;
		
		switch ( err )
		{
			case nsvErr         :  // fall through
			case resNotFound    :  // fall through
			case afpItemNotFound:  // fall through
			case fnfErr         :  result = ENOENT;        break;
			case badMDBErr      :  // fall through
			case fsDSIntErr     :  // fall through
			case ioErr          :  result = EIO;           break;
			case fnOpnErr       :  // fall through
			case wrPermErr      :  // fall through
			case rfNumErr       :  result = EBADF;         break;
			case cfragNoClientMemErr:// fall through
			case kOTOutOfMemoryErr:// fall through
			case memFullErr     :  result = ENOMEM;        break;
			case opWrErr        :  // fall through
			case permErr        :  // fall through
			case afpAccessDenied:  // fall through
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
			case kOTFlowErr     :  // fall through
			case kOTNoDataErr   :  result = EAGAIN;        break;
			case bdNamErr       :  result = ENAMETOOLONG;  break;
			case kOTBadAddressErr:  result = EADDRINUSE;   break;
			case unimpErr       :  result = ENOSYS;        break;
			
			default:
				result = -1;
				break;
		}
		
		return result;
	}
	
}
}
