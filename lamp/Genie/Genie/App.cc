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
		spawn_process( "/bin/jgetty" );
		
		return true;
	}
	
	App::App()
	{
		SetCommandHandler( Ped::kCmdAbout, &About       );
		SetCommandHandler( Ped::kCmdNew,   &NewDocument );
	}
	
}


//	main
//	----

int main()
{
	Genie::App app;
	
	return app.Run();
}

