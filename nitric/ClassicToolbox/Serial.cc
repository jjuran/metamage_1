/*	=========
 *	Serial.cc
 *	=========
 */

#include "ClassicToolbox/Serial.hh"


namespace Nitrogen
{
	
	// does nothing, but guarantees construction of theRegistration
	NUCLEUS_DEFINE_ERRORS_DEPENDENCY( SerialDriver )
	
	
	static void RegisterSerialDriverErrors();
	
	
#if NUCLEUS_RICH_ERRORCODES
#pragma force_active on
	
	class SerialDriverErrorsRegistration
	{
		public:
			SerialDriverErrorsRegistration()  { RegisterSerialDriverErrors(); }
	};
	
	static SerialDriverErrorsRegistration theRegistration;
	
#pragma force_active reset
#endif
	
	
	template < class Type >
	Type* CheckNULL( Type* ptr )
	{
		if ( static_cast< const void* >( ptr ) == NULL )
		{
			throw ParamErr();  // FIXME:  Should assert
		}
		
		return ptr;
	}
	
	std::size_t SerGetBuf( DriverRefNum inputDriverRefNum )
	{
		long count;
		ThrowOSStatus( ::SerGetBuf( inputDriverRefNum, &count ) );
		return count;
	}
	
	void SerSetBuf( DriverRefNum inputDriverRefNum, void* buf, std::size_t bufSize )
	{
		const std::size_t maxSize = 32767;
		
		ThrowOSStatus( ::SerSetBuf( inputDriverRefNum,
		                            reinterpret_cast< ::Ptr >( buf ),
		                            std::min( bufSize, maxSize ) ) );
	}
	
	SerStaRec SerStatus( DriverRefNum outputDriverRefNum )
	{
		SerStaRec serStatus;
		ThrowOSStatus( ::SerStatus( outputDriverRefNum, &serStatus ) );
		return serStatus;
	}
	
	void SerReset( DriverRefNum outputDriverRefNum, SerConfig serConfig )
	{
		ThrowOSStatus( ::SerReset( outputDriverRefNum, serConfig ) );
	}
	
	void SerHShake( DriverRefNum outputDriverRefNum, const SerShk& serShk )
	{
		Control< kSERDHandshake >( outputDriverRefNum, serShk );
	}
	
	void AssertDTR( DriverRefNum outputDriverRefNum )
	{
		Control< kSERDAssertDTR >( outputDriverRefNum );
	}
	
	void NegateDTR( DriverRefNum outputDriverRefNum )
	{
		Control< kSERDNegateDTR >( outputDriverRefNum );
	}
	
	void ClearXOFF( DriverRefNum outputDriverRefNum )
	{
		Control< kSERDClearXOffFlag >( outputDriverRefNum );
	}
	
	void RegisterSerialDriverErrors()
	{
		RegisterOSStatus< openErr   >();
		RegisterOSStatus< portInUse >();
		RegisterOSStatus< portNotCf >();
	}
	
}

