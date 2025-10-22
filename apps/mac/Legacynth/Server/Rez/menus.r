#include "Apple-menu.r"

resource 'MENU' (129) {
	2,
	textMenuProc,
	0x7FFFFFF4,
	enabled,
	"File",
	{
		"Start Server", noIcon, "S", noMark, plain,
		"Stop Server", noIcon, ".", noMark, plain,
		"Close", noIcon, "W", noMark, plain,
		"-", noIcon, noKey, noMark, plain,
		"Quit", noIcon, "Q", noMark, plain
	}
};

resource 'MENU' (130) {
	3,
	textMenuProc,
	0x7FFFFFC0,
	enabled,
	"Edit",
	{
		"Undo", noIcon, "Z", noMark, plain,
		"-", noIcon, noKey, noMark, plain,
		"Cut", noIcon, "X", noMark, plain,
		"Copy", noIcon, "C", noMark, plain,
		"Paste", noIcon, "V", noMark, plain,
		"Clear", noIcon, noKey, noMark, plain,
	}
};
