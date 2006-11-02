// OSErrno.hh

#ifndef OSERRNO_HH
#define OSERRNO_HH

// Nitrogen
#include "Nitrogen/OSStatus.h"

// POSeven
#include "POSeven/Errno.hh"


namespace Nucleus
{
	
	template <>
	struct Converter< POSeven::Errno, Nitrogen::OSStatus > : public std::unary_function< Nitrogen::OSStatus, POSeven::Errno >
	{
		POSeven::Errno operator()( Nitrogen::OSStatus error ) const
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
				
				default:               result = -1;         break;
			}
			
			return POSeven::Errno( result );
		}
	};
	
}

#endif

