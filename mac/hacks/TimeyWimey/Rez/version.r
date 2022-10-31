#define COPYRIGHT  "2022 Josh Juran"

#define COPY "\0xA9"

type 'Timy' as 'STR ';

resource 'Timy' (0)
{
	"Copyright " COPY " " COPYRIGHT
};

resource 'vers' (1)
{
	0x01,
	0x00,
	release,
	0,
	smRoman,
	"1.0",
	"1.0, " COPY " " COPYRIGHT
};
