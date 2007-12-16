/*	========================
 *	InternallyNonblocking.hh
 *	========================
 */

#ifndef GENIE_IO_INTERNALLYNONBLOCKING_HH
#define GENIE_IO_INTERNALLYNONBLOCKING_HH


namespace Genie
{
	
	template < class Stream >
	class InternallyNonblocking : public Stream
	{
		private:
			bool itIsNonblocking;
		
		protected:
			~InternallyNonblocking()  {}
		
		public:
			InternallyNonblocking() : itIsNonblocking( false )  {}
			
			bool IsNonblocking() const  { return itIsNonblocking; }
			
			void SetNonblocking  ()  { itIsNonblocking = true;  }
			void ClearNonblocking()  { itIsNonblocking = false; }
	};
	
}

#endif

