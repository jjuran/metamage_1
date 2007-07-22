/*	================
 *	Genie/Console.hh
 *	================
 */

#ifndef GENIE_CONSOLE_HH
#define GENIE_CONSOLE_HH

// Standard C++
#include <list>
#include <map>
#include <memory>
#include <string>

// boost
#include <boost/shared_ptr.hpp>

// Nitrogen
#include "Nitrogen/Files.h"

// Io
#include "Io/TextInput.hh"

// Pedestal
#include "Pedestal/Console.hh"
#include "Pedestal/Scroller.hh"
#include "Pedestal/Window.hh"
#include "Pedestal/View.hh"

// Genie
#include "Genie/IO/ConsoleTTY.hh"  // only for ID()


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace Ped = Pedestal;
	
	
	//class ConsoleTTYHandle;
	
	typedef std::size_t ConsoleID;
	
	class ConsoleWindowClosure : public Ped::WindowClosure
	{
		private:
			ConsoleTTYHandle* itsTerminal;
			ConsoleID itsConsoleID;
			bool itHasBeenRequested;
			bool itHasDisassociated;
		
		protected:
			// prevent slicing
			~ConsoleWindowClosure()  {}
		
		public:
			ConsoleWindowClosure( ConsoleTTYHandle* terminal ) : itsTerminal( terminal ),
			                                                     itsConsoleID( terminal->ID() ),
			                                                     itHasBeenRequested(),
			                                                     itHasDisassociated()
			{
			}
			
			ConsoleID ID() const  { return itsConsoleID; }
			
			const std::string& TTYName() const;
			
			bool RequestWindowClosure( N::WindowRef );
			
			bool ClosureHasBeenRequested() const  { return itHasBeenRequested; }
			
			void DisassociateFromTerminal()  { itHasDisassociated = true; }
	};
	
	
	class ConsolePane;
	class ConsoleWindow;
	
	class Console : public ConsoleWindowClosure
	{
		private:
			std::auto_ptr< ConsoleWindow >  itsWindow;
			N::Str255                       itsLatentTitle;
			std::string                     itsCurrentInput;
			int                             itsWindowSalvagePolicy;
			int                             itsLeaderWaitStatus;
			bool                            itIsBlocking;
		
		public:
			Console( ConsoleTTYHandle* terminal );
			
			bool IsOpen() const  { return itsWindow.get() != NULL; }
			
			N::Str255 DefaultTitle() const;
			
			N::Str255 GetTitle() const  { return itsLatentTitle; }
			
			void SetTitle( ConstStr255Param title );
			
			void Open();
			
			bool IsReadable() const;
			
			int Read (       char* data, std::size_t byteCount );
			int Write( const char* data, std::size_t byteCount );
			
			int GetWindowSalvagePolicy() const  { return itsWindowSalvagePolicy; }
			
			void SetWindowSalvagePolicy( int policy )  { itsWindowSalvagePolicy = policy; }
			
			void SetLeaderWaitStatus( int status )  { itsLeaderWaitStatus = status; }
			
			bool ShouldSalvageWindow() const;
			
			ConsoleID Salvage();
	};
	
	
	typedef std::map< Console*, boost::weak_ptr< Console > > ConsoleMap;
	
	const ConsoleMap& GetConsoleMap();
	
	class IOHandle;
	
	boost::shared_ptr< IOHandle > NewConsoleDevice();
	
	void SpawnNewConsole( const FSSpec& program );
	void SpawnNewConsole();
	
	boost::shared_ptr< Console > NewConsole( ConsoleTTYHandle* terminal );
	
	void CloseConsole( const boost::shared_ptr< Console >& console );
	
}

#endif

