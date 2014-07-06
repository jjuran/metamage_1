/*
	OpenTransport_share.hh
	----------------------
*/

#ifndef MACAPP_OPENTRANSPORTSHARE_HH
#define MACAPP_OPENTRANSPORTSHARE_HH


namespace mac {
namespace app {
	
	typedef void (*OSStatus_handler)( long err );
	
	void default_OSStatus_handler( long err );
	
	void InitOpenTransport_shared( OSStatus_handler handler );
	
	void CloseOpenTransport_shared();
	
	
	class OpenTransport_share
	{
		public:
			OpenTransport_share( OSStatus_handler handler = &default_OSStatus_handler )
			{
				InitOpenTransport_shared( handler );
			}
			
			OpenTransport_share( const OpenTransport_share& )
			{
				// Open Transport already initialized, so errors not possible
				
				InitOpenTransport_shared( 0 );  // NULL
			}
			
			~OpenTransport_share()
			{
				CloseOpenTransport_shared();
			}
	};
	
}
}

#endif
