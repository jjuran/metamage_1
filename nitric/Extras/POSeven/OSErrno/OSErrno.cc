// OSErrno.cc

#include "OSErrno/OSErrno.hh"


namespace OSErrno
{
	
	POSeven::Errno ErrnoFromOSStatus( const Nitrogen::OSStatus& error )
	{
		int result = 0;
		
		switch ( error.Get() )
		{
			case fnfErr         :  result = ENOENT;     break;
			case memFullErr     :  result = ENOMEM;     break;
			case userCanceledErr:  result = ECANCELED;  break;
			case fBsyErr        :  result = EBUSY;      break;
			case dupFNErr       :  result = EEXIST;     break;
			case diffVolErr     :  result = EXDEV;      break;
			case dirNFErr       :  result = ENOTDIR;    break;
			case notAFileErr    :  result = EISDIR;     break;
			case paramErr       :  result = EINVAL;     break;
			case dskFulErr      :  result = ENOSPC;     break;
			case vLckdErr       :  result = EROFS;      break;
			
			default:
				throw error;
		}
		
		return POSeven::Errno( result );
	}
	
}

