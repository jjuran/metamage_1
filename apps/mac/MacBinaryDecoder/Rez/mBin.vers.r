#define COPYRIGHT  "2007-2016 Josh Juran"

#define COPY "\0xA9"

type 'mBin' as 'STR ';

resource 'mBin' (0, "MacBinary Decoder")
{
	"Copyright " COPY " " COPYRIGHT
};

resource 'vers' (1) {
	0x00,
	0x50,
	release,
	0,
	smRoman,
	"0.5.0",
	"0.5.0, " COPY COPYRIGHT
};
