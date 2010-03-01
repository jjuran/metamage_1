/*	==========
 *	Maildir.cc
 *	==========
 */

#include "Maildir.hh"

// Nitrogen Extras / Utilities
#include "Utilities/Files.h"

// Io
#include "Io/Stream.hh"


namespace Artifact
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	
	static FSSpec GetCur( const N::FSDirSpec& maildir )
	{
		return maildir & "cur";
	}
	
	static FSSpec GetNew( const N::FSDirSpec& maildir )
	{
		return maildir & "new";
	}
	
	static FSSpec GetTmp( const N::FSDirSpec& maildir )
	{
		return maildir & "tmp";
	}
	
	static N::FSDirSpec GetCurDir( const N::FSDirSpec& maildir )
	{
		return NN::Convert< N::FSDirSpec >( GetCur( maildir ) );
	}
	
	static N::FSDirSpec GetNewDir( const N::FSDirSpec& maildir )
	{
		return NN::Convert< N::FSDirSpec >( GetNew( maildir ) );
	}
	
	static N::FSDirSpec GetTmpDir( const N::FSDirSpec& maildir )
	{
		return NN::Convert< N::FSDirSpec >( GetTmp( maildir ) );
	}
	
	bool IsMaildir( const N::FSDirSpec& dir )
	{
		return N::FSpTestDirectoryExists( GetCur( dir ) )
			&& N::FSpTestDirectoryExists( GetNew( dir ) )
			&& N::FSpTestDirectoryExists( GetTmp( dir ) );
	}
	
	bool IsMaildir( const FSSpec& dir )
	{
		return N::FSpTestDirectoryExists( dir ) && IsMaildir( NN::Convert< N::FSDirSpec >( dir ) );
	}
	
	static N::FSDirSpec CreateMissingDirectory( const FSSpec& dir )
	{
		if ( !N::FSpTestDirectoryExists( dir ) )
		{
			return N::FSpDirCreate( dir );
		}
		
		return NN::Convert< N::FSDirSpec >( dir );
	}
	
	N::FSDirSpec CreateMaildir( const FSSpec& dir )
	{
		N::FSDirSpec result = CreateMissingDirectory( dir );
		
		CreateMissingDirectory( GetCur( result ) );
		CreateMissingDirectory( GetNew( result ) );
		CreateMissingDirectory( GetTmp( result ) );
		
		return result;
	}
	
	template < class FileAccess, class CommitFunc, class RollbackFunc >
	class IncomingMessageDelivery
	{
		private:
			n::owned< FileAccess > myFileAccess;
			CommitFunc myCommit;
			RollbackFunc myRollback;
			bool myIsComplete, myRolledBack;
		
		public:
			IncomingMessageDelivery( n::owned< FileAccess > fileAccess,
			                         const CommitFunc& commit,
			                         const RollbackFunc& rollback )
			:
				myFileAccess( fileAccess ),
				myCommit    ( commit     ),
				myRollback  ( rollback   ),
				myIsComplete( false      ),
				myRolledBack( false      )
			{}
			
			~IncomingMessageDelivery()  { Rollback(); }
			
			void WriteLine( const std::string& line )
			{
				std::string entireLine = line + "\r\n";
				
				//Io::S( myFileAccess ) << entireLine;
				
				N::FSWrite( myFileAccess, entireLine.size(), entireLine.c_str() );
			}
			
			void CloseFile()  { myFileAccess = n::owned< FileAccess >(); }
			
			void Commit()
			{
				if ( myIsComplete || myRolledBack )  return;
				
				CloseFile();
				myCommit();
				myIsComplete = true;
			}
			
			void Rollback()
			{
				if ( myIsComplete || myRolledBack )  return;
				
				CloseFile();
				myRollback();
				myRolledBack = true;
			}
	};
	
	class MoveFileToDir
	{
		private:
			FSSpec myFile;
			N::FSDirSpec myNewDir;
		
		public:
			MoveFileToDir( const FSSpec& file, const N::FSDirSpec& newDir )
			:
				myFile  ( file   ),
				myNewDir( newDir )
			{}
			
			void operator()() const
			{
				N::FSpCatMove( myFile, myNewDir );
			}
	};
	
	class DeleteFile
	{
		private:
			FSSpec myFile;
		
		public:
			DeleteFile( const FSSpec& file ) : myFile( file )  {}
			
			void operator()() const
			{
				N::FSpDelete( myFile );
			}
	};
	
	template < class Dest >
	class WriteLine
	{
		private:
			Dest& myDest;
		
		public:
			WriteLine( Dest& dest ) : myDest( dest )  {}
			void operator()( const std::string& line )  { myDest.WriteLine( line ); }
	};
	
	void SaveMessageToMaildir( const N::FSDirSpec& maildir,
	                           const std::string& filename,
	                           const std::vector< std::string >& lines )
	{
		typedef IncomingMessageDelivery< N::FSFileRefNum,
		                                 MoveFileToDir,
		                                 DeleteFile >
		        MessageDelivery;
		
		FSSpec msgFile = GetTmpDir( maildir ) & filename;
		
		MessageDelivery msg( N::FSpOpenDF( N::FSpCreate( msgFile,
		                                                 N::OSType( 'R*ch' ),
		                                                 N::OSType( 'TEXT' ) ),
		                                   N::fsWrPerm ),
		                     MoveFileToDir( msgFile, GetNewDir( maildir ) ),
		                     DeleteFile( msgFile ) );
		
		std::for_each( lines.begin(),
		               lines.end(),
		               WriteLine< MessageDelivery >( msg ) );
		
		msg.Commit();
	}
	
	void MarkNewMessageAsRead( const N::FSDirSpec& maildir,
	                           const std::string& filename )
	{
		FSSpec message = GetNewDir( maildir ) & filename;
		N::FSpCatMove( message, GetCurDir( maildir ) );
	}
	
}  // namespace Artifact

