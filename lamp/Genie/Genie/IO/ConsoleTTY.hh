/*	=============
 *	ConsoleTTY.hh
 *	=============
 */

#ifndef GENIE_IO_CONSOLETTY_HH
#define GENIE_IO_CONSOLETTY_HH

 // Genie
 #include "Genie/IO/TTY.hh"


namespace Genie
{
	
	class Console;
	
	class ConsoleTTYHandle : public TTYHandle
	{
		private:
			std::size_t id;
			boost::shared_ptr< Console > console;
			
			void CheckConsole();
		
		public:
			static TypeCode Type()  { return kConsoleTTYType; }
			
			TypeCode ActualType() const  { return Type(); }
			
			ConsoleTTYHandle( std::size_t id );
			
			~ConsoleTTYHandle();
			
			unsigned int SysPoll() const;
			
			int SysRead( char* data, std::size_t byteCount );
			
			int SysWrite( const char* data, std::size_t byteCount );
			
			void IOCtl( unsigned long request, int* argp );
			
			std::size_t ID() const  { return id; }
			
			void SaveLeaderWaitStatus( int status );
	};
	
}

#endif

