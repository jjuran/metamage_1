/*	===========
 *	Terminal.cc
 *	===========
 */

#include "Genie/Terminal.hh"

// Standard C++
#include <memory>

// Nitrogen core
#include "Nitrogen/OnlyOnce.h"

// Io
#include "Io/StringBuffer.hh"

// Genie
#include "Genie/Console.hh"
#include "Genie/ResourceTable.hh"


namespace Genie
{
	
	struct Tube
	{
		Io::StringBuffer input;
		Io::StringBuffer output;
	};
	
}

namespace Nitrogen
{
	
	template <>
	struct OwnedDefaults< Genie::Tube* >
	{
		typedef DisposeWithDelete Disposer;
	};
	
}

namespace Genie
{
	
	namespace N = Nitrogen;
	
	int CharacterDevice::Read( char* data, std::size_t byteCount )
	{
		throw Io::EndOfInput();
	}
	
	class PseudoTerminalMaster : public CharacterDevice
	{
		private:
			N::Shared< Tube* > fTube;
		
		public:
			PseudoTerminalMaster( const N::Shared< Tube* >& tube ) : fTube( tube )  {}
			
			bool IsATTY() const  { return true; }
			
			const char* DeviceName() const  { return "/dev/ptmx"; }  // Close enough for now
			
			// These are from the user's perspective:
			// We write input for the user to read, and read output the user has written.
			
			int Read (       char* data, std::size_t byteCount )
			{
				return fTube.Get()->output.Read( data, byteCount );
			}
			
			int Write( const char* data, std::size_t byteCount )
			{
				return fTube.Get()->input.Write( data, byteCount );
			}
	};
	
	class PseudoTerminalSlave : public CharacterDevice
	{
		private:
			N::Shared< Tube* > fTube;
			std::string fName;
		
		public:
			PseudoTerminalSlave( const N::Shared< Tube* >& tube, int id )
			:
				fTube( tube        ),
				fName( "/dev/pts/" )
			{
				fName += N::Convert< std::string >( id );
			}
			
			bool IsATTY() const  { return true; }
			
			const char* DeviceName() const  { return fName.c_str(); }
			
			int Read (       char* data, std::size_t byteCount )
			{
				return fTube.Get()->input.Read( data, byteCount );
			}
			
			int Write( const char* data, std::size_t byteCount )
			{
				return fTube.Get()->output.Write( data, byteCount );
			}
	};
	
	typedef ResourceTable< CharacterDevice > CharacterDeviceTable;
	
	void RegisterCharacterDeviceRefMod();
	void RegisterCharacterDeviceRefMod()
	{
		RegisterIOType( kCharacterDeviceDescriptor,
		                CharacterDeviceTable::RefMod,
		                CharacterDeviceTable::Read,
		                CharacterDeviceTable::Write );
	}
	
	NewPseudoTerminal_Result NewPseudoTerminal()
	{
		N::OnlyOnce< RegisterCharacterDeviceRefMod >();
		
		static int gNextID = 0;
		
		N::Shared< Tube* > tube = N::Owned< Tube* >::Seize( new Tube );
		
		std::auto_ptr< PseudoTerminalMaster > master( new PseudoTerminalMaster( tube ) );
		std::auto_ptr< PseudoTerminalSlave  > slave ( new PseudoTerminalSlave ( tube, gNextID++ ) );
		
		return std::make_pair( IORef( kCharacterDeviceDescriptor,
		                              CharacterDeviceTable::Add( master ) ),
		                       IORef( kCharacterDeviceDescriptor,
		                              CharacterDeviceTable::Add( slave ) ) );
	}
	
	
	class Terminal : public CharacterDevice
	{
		private:
			std::size_t fID;
			std::string fName;
			Console* fConsole;
		
		public:
			Terminal( std::size_t id )
			:
				fID( id ),
				fName( "/dev/tty" + N::Convert< std::string >( fID ) ),
				fConsole( NULL )
			{}
			
			~Terminal();
			
			bool IsATTY() const  { return true; }
			
			const char* DeviceName() const  { return fName.c_str(); }
			
			void CheckConsole();
			
			int Read (       char* data, std::size_t byteCount )
			{
				CheckConsole();
				
				return fConsole->Read( data, byteCount );
			}
			
			int Write( const char* data, std::size_t byteCount )
			{
				CheckConsole();
				
				return fConsole->Write( data, byteCount );
			}
	};
	
	Terminal::~Terminal()
	{
		CloseConsole( fConsole );
	}
	
	void Terminal::CheckConsole()
	{
		if ( fConsole == NULL )
		{
			fConsole = NewConsole( this );
		}
	}
	
	IORef NewTerminal()
	{
		N::OnlyOnce< RegisterCharacterDeviceRefMod >();
		
		static std::size_t gLastID = 0;
		
		std::auto_ptr< Terminal > terminal( new Terminal( ++gLastID ) );
		
		std::size_t offset = CharacterDeviceTable::Add( terminal );
		
		return IORef( kCharacterDeviceDescriptor, offset );
	}
	
}

