/*	============
 *	GenieMain.cc
 *	============
 */

// Nitrogen
#include "Nitrogen/AEDataModel.hh"

// Pedestal
#include "Pedestal/Application.hh"
#include "Pedestal/Commands.hh"

// Genie
#include "Genie/ReplyHandler.hh"
#include "Genie/ProcessList.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace Ped = Pedestal;
	
	
	class TerminalsOwner
	{
		public:
			void NewWindow();
	};
	
	static TerminalsOwner gTerminals;
	
	
	class App : public Ped::Application
	{
		private:
			GenieHandlerReply myReplyEventHandler;
		
		public:
			App();
			~App()  { kill_all_processes(); }
	};
	
	
	static bool About( Ped::CommandCode )
	{
		spawn_process( "/sbin/about" );
		
		return true;
	}
	
	static bool NewDocument( Ped::CommandCode )
	{
		gTerminals.NewWindow();
		
		return true;
	}
	
	App::App()
	{
		SetCommandHandler( Ped::kCmdAbout, &About       );
		SetCommandHandler( Ped::kCmdNew,   &NewDocument );
	}
	
	void TerminalsOwner::NewWindow()
	{
		spawn_process( "/bin/jgetty" );
	}
	
}


//	main
//	----

int main()
{
	Genie::App app;
	
	return app.Run();
}

