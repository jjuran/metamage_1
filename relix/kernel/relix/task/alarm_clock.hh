/*
	alarm_clock.hh
	--------------
*/

#ifndef RELIX_TASK_ALARMCLOCK_HH
#define RELIX_TASK_ALARMCLOCK_HH


namespace relix
{
	
	class alarm_clock
	{
		private:
			typedef long long int64_t;
			
			int64_t its_time;
		
		public:
			alarm_clock() : its_time()
			{
			}
			
			unsigned set( unsigned seconds );
			
			bool check();
	};
	
}

#endif

