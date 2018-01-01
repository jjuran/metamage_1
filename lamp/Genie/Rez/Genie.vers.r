#define COPYRIGHT  "1999-2018 Josh Juran"

#define COPY "\0xA9"

resource 'vers' (1) {
	0x00,
	0x72,
	alpha,
	1,
	smRoman,
	"0.7.2-alpha.1",
	"0.7.2-alpha.1, " COPY COPYRIGHT
};

/* Owner resource */

type 'Poof' as 'STR ';

resource 'Poof' (0, "MacRelix")
{
	"Copyright " COPY " " COPYRIGHT
};
