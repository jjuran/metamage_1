/*	=============
 *	LongJumper.hh
 *	=============
 */

#ifndef GENIE_PROCESS_LONGJUMPER_HH
#define GENIE_PROCESS_LONGJUMPER_HH


namespace Genie
{
	
	class LongJumper
	{
		private:
			void (*itsLongJmp)(int);
		
		public:
			typedef void (*LongJmp)(int);
			
			LongJumper() : itsLongJmp()  {}
			
			LongJmp GetLongJmp() const  { return itsLongJmp; }
			
			void SetLongJmp( LongJmp f )  { itsLongJmp = f; }
	};
	
}

#endif

