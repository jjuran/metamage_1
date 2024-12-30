/*
	error.hh
	--------
*/

#ifndef MACSYS_ERROR_HH
#define MACSYS_ERROR_HH


namespace mac {
namespace sys {
	
#if __LP64__
	
	typedef   signed int  SInt32;
	typedef unsigned int  UInt32;
	
#else
	
	typedef   signed long  SInt32;
	typedef unsigned long  UInt32;
	
#endif
	
	typedef short  OSErr;
	typedef SInt32 OSStatus;
	
	enum Error
	{
		noErr = 0,
		
		kMinimumOSStatus = (SInt32) 0x80008000,
	};
	
	inline
	Error muxed_OSStatus( OSStatus err )
	{
		return Error( err );
	}
	
	inline
	Error muxed_OSErr( OSErr err )
	{
		return Error( err );
	}
	
	inline
	Error muxed_errno( int errnum )
	{
		return Error( 0x80000000 | errnum );
	}
	
	inline
	bool is_errno( Error muxed )
	{
		return muxed < kMinimumOSStatus;
	}
	
	inline
	bool is_OSStatus( Error muxed )
	{
		return (UInt32) muxed >= (UInt32) kMinimumOSStatus;
	}
	
	inline
	short errno_from_muxed( Error muxed )
	{
		return is_errno( muxed ) ? muxed : 0;
	}
	
}
}

#endif
