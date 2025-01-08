/*
	audio.hh
	--------
*/

#ifndef PEARL_AUDIO_HH
#define PEARL_AUDIO_HH

namespace Pearl
{

struct audio_stream
{
	private:
		// non-copyable
		audio_stream           ( const audio_stream& );
		audio_stream& operator=( const audio_stream& );

	public:
		audio_stream();
		~audio_stream();

		bool start();
		bool stop();
};

}

#endif
