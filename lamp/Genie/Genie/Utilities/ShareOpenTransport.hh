/*	=====================
 *	ShareOpenTransport.hh
 *	=====================
 */

#ifndef SHAREOPENTRANSPORT_HH
#define SHAREOPENTRANSPORT_HH


namespace Genie
{
	
	void InitOpenTransport_Shared();
	
	void CloseOpenTransport_Shared();
	
	
	class OpenTransportShare
	{
		public:
			OpenTransportShare()
			{
				InitOpenTransport_Shared();
			}
			
			OpenTransportShare( const OpenTransportShare& )
			{
				InitOpenTransport_Shared();
			}
			
			~OpenTransportShare()
			{
				CloseOpenTransport_Shared();
			}
	};
	
	
	inline OpenTransportShare ShareOpenTransport()  { return OpenTransportShare(); }
	
}

#endif

